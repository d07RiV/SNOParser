#pragma once

#include "common.h"
#include "file.h"

template<
  typename p_type,
  int redBits, int redShift,
  int greenBits, int greenShift,
  int blueBits, int blueShift,
  int alphaBits, int alphaShift
>
struct PixelFormat {
  typedef p_type color_t;

  template<int p_bits, int p_shift>
  struct PixelComponent {
    static const int bits = p_bits;
    static const int shift = p_shift;
    static const color_t max = (color_t(1) << p_bits) - 1;
    static const color_t mask = max << p_shift;

    static color_t to(int value) {
      return (static_cast<color_t>(value) * max / 255) << shift;
    }
    static color_t to_clamp(int value) {
      if (value < 0) value = 0;
      if (value > 255) value = 255;
      return to(value);
    }
    static int from(color_t color) {
      if (!bits) return 255;
      return ((color >> shift) & max) * 255 / max;
    }
  };

  typedef PixelComponent<redBits, redShift> red;
  typedef PixelComponent<greenBits, greenShift> green;
  typedef PixelComponent<blueBits, blueShift> blue;
  typedef PixelComponent<alphaBits, alphaShift> alpha;

  static color_t color(int r, int g, int b, int a = alpha::max) {
    return red::to(r) | green::to(g) | blue::to(b) | alpha::to(a);
  }
  static color_t color_clamp(int r, int g, int b, int a = alpha::max) {
    return red::to_clamp(r) | green::to_clamp(g) | blue::to_clamp(b) | alpha::to_clamp(a);
  }

  template<class PF>
  static color_t from(typename PF::color_t src) {
    return red::to(PF::red::from(src)) |
      green::to(PF::green::from(src)) |
      blue::to(PF::blue::from(src)) |
      alpha::to(PF::alpha::from(src));
  }

  template<class PF>
  static typename PF::color_t to(color_t src) {
    return PF::red::to(red::from(src)) |
      PF::green::to(green::from(src)) |
      PF::blue::to(blue::from(src)) |
      PF::alpha::to(alpha::from(src));
  }

  template<class PF>
  static color_t mix(typename PF::color_t a, int ka, typename PF::color_t b, int kb) {
    return color(
      (PF::red::from(a) * ka + PF::red::from(b) * kb) / (ka + kb),
      (PF::green::from(a) * ka + PF::green::from(b) * kb) / (ka + kb),
      (PF::blue::from(a) * ka + PF::blue::from(b) * kb) / (ka + kb),
      (PF::alpha::from(a) * ka + PF::alpha::from(b) * kb) / (ka + kb)
      );
  }
  static color_t mix(color_t a, int ka, color_t b, int kb) {
    return color(
      (red::from(a) * ka + red::from(b) * kb) / (ka + kb),
      (green::from(a) * ka + green::from(b) * kb) / (ka + kb),
      (blue::from(a) * ka + blue::from(b) * kb) / (ka + kb),
      (alpha::from(a) * ka + alpha::from(b) * kb) / (ka + kb)
    );
  }
};

template<int A, int B, int G, int R>
using FormatABGR = PixelFormat<UInt<R+G+B+A>, R, 0, G, R, B, R+G, A, R+G+B>;
template<int X, int B, int G, int R>
using FormatXBGR = PixelFormat<UInt<R+G+B+X>, R, 0, G, R, B, R+G, 0, 0>;
template<int A, int R, int G, int B>
using FormatARGB = PixelFormat<UInt<R+G+B+A>, R, B+G, G, B, B, 0, A, B+G+R>;
template<int X, int R, int G, int B>
using FormatXRGB = PixelFormat<UInt<R+G+B+X>, R, B+G, G, B, B, 0, 0, 0>;

typedef FormatARGB<8,8,8,8> DefaultFormat;

namespace ImageFormat {
  enum Type {
    Unknown     = 0,
    PNG         = 1,

    NumFormats
  };
}

template<typename p_Format>
class ImageBase {
public:
  typedef p_Format Format;
  typedef typename p_Format::color_t color_t;

  ImageBase() : data_(nullptr) {}
  template<class PF>
  ImageBase(ImageBase<PF> const& image)
    : data_(create(image.width(), image.height()))
  {
    ImageBase<PF>::color_t const* src = image.bits();
    for (uint32 i = 0; i < data_->width_ * data_->height_; ++i) {
      data_->bits_[i] = Format::from<PF>(src[i]);
    }
  }
  ImageBase(ImageBase<Format> const& image)
    : data_(image.data_)
  {
    if (data_) data_->addref();
  }
  ImageBase(ImageBase<Format>&& image)
    : data_(image.data_)
  {
    image.data_ = nullptr;
  }
  ~ImageBase() {
    data_->release();
  }

  operator bool() const {
    return data_ != nullptr;
  }

  void release() {
    data_->release();
    data_ = nullptr;
  }

  template<class PF>
  ImageBase<Format>& operator=(ImageBase<PF> const& image) {
    if (!image.data_) {
      data_->release();
      data_ = nullptr;
      return *this;
    }
    if (data_->width_ * data_->height_ != image.width() * image.height()) {
      if (data_->unique()) {
        delete[] data_->bits_;
        data_->bits_ = new color_t[image.width() * image.height()];
      } else {
        data_->release();
        data_ = create(image.width(), image.height());
      }
    } else {
      splice();
    }
    data_->width_ = image.width();
    data_->height_ = image.height();
    ImageBase<PF>::color_t const* src = image.bits();
    for (uint32 i = 0; i < data_->width_ * data_->height_; ++i) {
      data_->bits_[i] = Format::from<PF>(src[i]);
    }
    return *this;
  }
  ImageBase<Format>& operator=(ImageBase<Format> const& image) {
    if (image.data_ == data_) {
      return *this;
    }
    data_->release();
    data_ = image.data_;
    if (data_) data_->addref();
    return *this;
  }
  ImageBase<Format>& operator=(ImageBase<Format>&& image) {
    if (image.data_ == data_) {
      return *this;
    }
    data_->release();
    data_ = image.data_;
    image.data_ = nullptr;
    return *this;
  }

  ImageBase(uint32 width, uint32 height, color_t color = 0)
    : data_(create(width, height))
  {
    for (uint32 i = 0; i < width * height; ++i) {
      data_->bits_[i] = color;
    }
  }
  ImageBase(uint32 width, uint32 height, color_t* bits) {
    data_ = new Data;
    data_->width_ = width;
    data_->height_ = height;
    data_->bits = bits;
  }

  color_t const* bits() const {
    return data_->bits_;
  }
  color_t* mutable_bits() {
    splice();
    return data_->bits_;
  }
  size_t size() const {
    return data_->width_ * data_->height_ * sizeof(color_t);
  }
  uint32 width() const {
    return data_->width_;
  }
  uint32 height() const {
    return data_->height_;
  }

  ImageBase<Format> subimage(uint32 left, uint32 top, uint32 right, uint32 bottom) const {
    ImageBase<Format> result;
    result.data_ = create(right - left, bottom - top);
    for (uint32 y = top; y < bottom; ++y) {
      color_t const* src = data_->bits_ + y * data_->width_ + left;
      color_t* dst = result.data_->bits_ + (y - top) * result.data_->width_;
      for (uint32 x = left; x < right; ++x) {
        *dst++ = *src++;
      }
    }
    return result;
  }
  ImageBase<Format> subimagef(float left, float top, float right, float bottom) const {
    return subimage(
      static_cast<uint32>(data_->width_ * left + 0.5f),
      static_cast<uint32>(data_->height_ * top + 0.5f),
      static_cast<uint32>(data_->width_ * right + 0.5f),
      static_cast<uint32>(data_->height_ * bottom + 0.5f)
    );
  }

  ImageBase(File& file, ImageFormat::Type format = ImageFormat::Unknown);
  ImageBase(std::string const& path, ImageFormat::Type format = ImageFormat::Unknown);
  bool write(File& file, ImageFormat::Type format = ImageFormat::PNG);
  bool read(File& file, ImageFormat::Type format = ImageFormat::Unknown);
  bool write(std::string const& path, ImageFormat::Type format = ImageFormat::Unknown);
  bool read(std::string const& path, ImageFormat::Type format = ImageFormat::Unknown);

private:
  struct Data : public RefCounted {
    uint32 width_;
    uint32 height_;
    color_t* bits_ = nullptr;
    ~Data() {
      delete[] bits_;
    }
  };
  Data* data_;
  static Data* create(uint32 width, uint32 height) {
    Data* ptr = new Data;
    ptr->width_ = width;
    ptr->height_ = height;
    ptr->bits_ = new color_t[width * height];
    return ptr;
  }
  void splice() {
    if (!data_->unique()) {
      Data* tmp = create(data_->width_, data_->height_);
      memcpy(tmp->bits_, data_->bits_, sizeof(color_t) * data_->width_ * data_->height_);
      data_->release();
      data_ = tmp;
    }
  }
};

typedef ImageBase<DefaultFormat> Image;

namespace ImagePrivate {
  ImageFormat::Type getFormat(std::string const& name);
  bool imWrite(Image const& image, File& file, ImageFormat::Type format);
  Image imRead(File& file, ImageFormat::Type format);
}

template<class PF>
ImageBase<PF>::ImageBase(File& file, ImageFormat::Type format)
: ImageBase(ImagePrivate::imRead(file, format))
{}
template<class PF>
ImageBase<PF>::ImageBase(std::string const& path, ImageFormat::Type format)
: ImageBase(ImagePrivate::imRead(File(path),
    format == ImageFormat::Unknown ? ImagePrivate::getFormat(path) : format))
{}
template<class PF>
bool ImageBase<PF>::write(File& file, ImageFormat::Type format) {
  return ImagePrivate::imWrite(*this, file, format);
}
template<class PF>
bool ImageBase<PF>::read(File& file, ImageFormat::Type format) {
  *this = ImagePrivate::imRead(file, format);
  return data_ != nullptr;
}
template<class PF>
bool ImageBase<PF>::write(std::string const& path, ImageFormat::Type format) {
  return ImagePrivate::imWrite(*this, File(path, "wb"),
    format == ImageFormat::Unknown ? ImagePrivate::getFormat(path) : format);
}
template<class PF>
bool ImageBase<PF>::read(std::string const& path, ImageFormat::Type format) {
  *this = ImagePrivate::imRead(File(path),
    format == ImageFormat::Unknown ? ImagePrivate::getFormat(path) : format);
  return data_ != nullptr;
}
