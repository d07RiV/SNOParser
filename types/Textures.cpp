#include "Textures.h"

typedef FormatXRGB<0, 5, 6, 5> FormatDXT;
Image LoadDXT1(uint32 width, uint32 height, uint8 const* data, size_t size) {
  if (size != width * height / 2) return Image();
  Image image(width, height);
  Image::color_t* bits = image.mutable_bits();
  uint16 const* colors = (uint16*) data;
  uint32 const* lookups = (uint32*) (data + width * height / 4);
  for (uint32 y = 0; y < height; y += 4) {
    for (uint32 x = 0; x < width; x += 4) {
      uint16 p = *colors++;
      uint16 q = *colors++;
      uint32 lookup = *lookups++;
      Image::color_t c[4] = {Image::Format::from<FormatDXT>(p),
        Image::Format::from<FormatDXT>(q)};
      if (p > q) {
        c[2] = Image::Format::mix(c[0], 2, c[1], 1);
        c[3] = Image::Format::mix(c[0], 1, c[1], 2);
      } else {
        c[2] = Image::Format::mix(c[0], 1, c[1], 1);
        c[3] = 0;
      }
      for (uint32 cy = y; cy < y + 4; ++cy) {
        Image::color_t* dst = bits + cy * width + x;
        for (uint32 cx = x; cx < x + 4; ++cx) {
          *dst++ = c[lookup & 3];
          lookup >>= 2;
        }
      }
    }
  }
  return image;
}

Image LoadDXT3(uint32 width, uint32 height, uint8 const* data, size_t size) {
  if (size != width * height) return Image();
  Image image(width, height);
  Image::color_t* bits = image.mutable_bits();
  uint64 const* alphas = (uint64*) data;
  uint16 const* colors = (uint16*) (data + width * height / 2);
  uint32 const* lookups = (uint32*) (data + width * height * 3 / 4);
  for (uint32 y = 0; y < height; y += 4) {
    for (uint32 x = 0; x < width; x += 4) {
      uint64 alpha = *alphas++;
      uint16 p = *colors++;
      uint16 q = *colors++;
      uint32 lookup = *lookups++;
      Image::color_t c[4] = {Image::Format::from<FormatDXT>(p),
        Image::Format::from<FormatDXT>(q)};
      c[2] = Image::Format::mix(c[0], 2, c[1], 1);
      c[3] = Image::Format::mix(c[0], 1, c[1], 2);
      for (uint32 cy = y; cy < y + 4; ++cy) {
        Image::color_t* dst = bits + cy * width + x;
        for (uint32 cx = x; cx < x + 4; ++cx) {
          int a = (alpha & 15) * 255 / 15;
          *dst++ = (c[lookup & 3] & ~Image::Format::alpha::mask) | Image::Format::alpha::to(a);
          lookup >>= 2;
          alpha >>= 4;
        }
      }
    }
  }
  return image;
}

Image LoadDXT5(uint32 width, uint32 height, uint8 const* data, size_t size) {
  if (size != width * height) return Image();
  Image image(width, height);
  Image::color_t* bits = image.mutable_bits();
  uint8 const* alphas = data;
  uint8 const* alphamaps = data + width * height / 8;
  uint16 const* colors = (uint16*) (data + width * height / 2);
  uint32 const* lookups = (uint32*) (data + width * height * 3 / 4);
  for (uint32 y = 0; y < height; y += 4) {
    for (uint32 x = 0; x < width; x += 4) {
      uint32 a0 = *alphas++;
      uint32 a1 = *alphas++;
      uint64 amap = 0;
      for (int i = 0; i < 48; i += 8) {
        amap |= (static_cast<uint64>(*alphamaps++) << i);
      }
      uint16 p = *colors++;
      uint16 q = *colors++;
      uint32 lookup = *lookups++;
      Image::color_t c[4] = {Image::Format::from<FormatDXT>(p),
        Image::Format::from<FormatDXT>(q)};
      c[2] = Image::Format::mix(c[0], 2, c[1], 1);
      c[3] = Image::Format::mix(c[0], 1, c[1], 2);
      uint32 a[8] = {a0, a1};
      if (a0 > a1) {
        a[2] = (a0 * 6 + a1 * 1) / 7;
        a[3] = (a0 * 5 + a1 * 2) / 7;
        a[4] = (a0 * 4 + a1 * 3) / 7;
        a[5] = (a0 * 3 + a1 * 4) / 7;
        a[6] = (a0 * 2 + a1 * 5) / 7;
        a[7] = (a0 * 1 + a1 * 6) / 7;
      } else {
        a[2] = (a0 * 4 + a1 * 1) / 5;
        a[3] = (a0 * 3 + a1 * 2) / 5;
        a[4] = (a0 * 2 + a1 * 3) / 5;
        a[5] = (a0 * 1 + a1 * 4) / 5;
        a[6] = 0;
        a[7] = 255;
      }
      for (uint32 cy = y; cy < y + 4; ++cy) {
        Image::color_t* dst = bits + cy * width + x;
        for (uint32 cx = x; cx < x + 4; ++cx) {
          *dst++ = (c[lookup & 3] & ~Image::Format::alpha::mask) | Image::Format::alpha::to(a[amap & 7]);
          lookup >>= 2;
          amap >>= 3;
        }
      }
    }
  }
  return image;
}

template<typename PF>
Image LoadRaw(uint32 width, uint32 height, uint8 const* data, size_t size) {
  if (size != width * height * sizeof(PF::color_t)) return Image();
  Image image(width, height);
  Image::color_t* dst = image.mutable_bits();
  PF::color_t const* src = (PF::color_t*) data;
  for (uint32 i = 0; i < width * height; ++i) {
    *dst++ = Image::Format::from<PF>(*src++);
  }
  return image;
}

const struct Loader {
  uint32 id;
  Image(*load)(uint32 width, uint32 height, uint8 const* data, size_t size);
} loaders[] = {
    {0, LoadRaw<FormatARGB<8, 8, 8, 8>>},
    {2, LoadRaw<FormatXRGB<8, 8, 8, 8>>},
    {3, LoadRaw<FormatARGB<4, 4, 4, 4>>},
    {4, LoadRaw<FormatARGB<1, 5, 5, 5>>},
    {5, LoadRaw<FormatXRGB<1, 5, 5, 5>>},
    {6, LoadRaw<FormatXRGB<0, 5, 6, 5>>},
    {9, LoadDXT1},
    {10, LoadDXT1},
    {11, LoadDXT3},
    {12, LoadDXT5},
    {21, LoadRaw<FormatABGR<8, 8, 8, 8>>},
    {23, LoadRaw<FormatARGB<8, 0, 0, 0>>},
};

Image Textures::Type::load(int mip) const {
  Loader const* loader = nullptr;
  for (auto& ldr : loaders) {
    if (ldr.id == pixelFormat) {
      loader = &ldr;
      break;
    }
  }
  if (!loader) {
    return Image();
  }
  uint32 mipWidth = width >> mip;
  uint32 mipHeight = height >> mip;
  return loader->load(mipWidth, mipHeight, (uint8*) data[mip].data(), data[mip].size());
}
