#pragma once

#include "common.h"
#include "file.h"
#include <algorithm>

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
  static color_t blend(color_t dst, color_t src) {
    return mix(dst, 255 - alpha::from(src), src, alpha::from(src));
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
    Unknown       = 0,
    PNG           = 1,
    PNGGrayscale  = 2,

    NumFormats
  };
}

struct ImageFilter {
  //enum Type {
  //  Box,
  //  Triangle,
  //  Hermite,
  //  Bell,
  //  CubicBSpline,
  //  Lanczos3,
  //  Mitchell,
  //  Cosine,
  //  CatmullRom,
  //  Quadratic,
  //  QuadraticBSpline,
  //  CubicConvolution,
  //  Lanczos8,
  //};

  typedef double(*Function)(double);
  static double PI() { return 3.1415926535897932384626433832795; }

  static struct {
    static double radius() { return 0.5; }
    static double value(double x) {
      return (x <= 0.5 ? 1.0 : 0.0);
    }
  } Box;
  static struct {
    static double radius() { return 1.0; }
    static double value(double x) {
      return std::max(1.0 - x, 0.0);
    }
  } Triangle;
  static struct {
    static double radius() { return 1.0; }
    static double value(double x) {
      return (x < 1.0 ? (2 * x - 3) * x * x + 1 : 0.0);
    }
  } Hermite;
  static struct {
    static double radius() { return 1.5; }
    static double value(double x) {
      if (x < 0.5) return 0.75 - x * x;
      if (x < 1.5) return 0.5 * (1.5 - x) * (1.5 - x);
      return 0.0;
    }
  } Bell;
  static struct {
    static double radius() { return 2.0; }
    static double value(double x) {
      if (x < 1.0) return 0.5 * x * x * x - x * x + 2.0 / 3.0;
      if (x < 2.0) return (2.0 - x) * (2.0 - x) * (2.0 - x) / 6.0;
      return 0.0;
    }
  } CubicBSpline;
  static struct {
    static double radius() { return 3.0; }
    static double sinc(double x) {
      if (x == 0.0) return 1.0;
      x *= PI();
      return std::sin(x) / x;
    }
    static double value(double x) {
      return (x < 3.0 ? sinc(x) * sinc(x / 3.0) : 0.0);
    }
  } Lanczos3;
  static struct {
    static double radius() { return 2.0; }
    static double value(double x) {
      if (x < 1.0) return 7.0 / 6.0 * x * x * x - 2.0 * x * x + 8.0 / 9.0;
      if (x < 2.0) return -7.0 / 18.0 * x * x * x + 2.0 * x * x - 10.0 / 3.0 * x + 16.0 / 9.0;
      return 0.0;
    }
  } Mitchell;
  static struct {
    static double radius() { return 1.0; }
    static double value(double x) {
      if (x < 1.0) return (std::cos(x * PI()) + 1.0) / 2.0;
      return 0.0;
    }
  } Cosine;
  static struct {
    static double radius() { return 2.0; }
    static double value(double x) {
      if (x < 1.0) return 1.5 * x * x * x - 2.5 * x * x + 1.0;
      if (x < 2.0) return -0.5 * x * x * x + 2.5 * x * x - 4.0 * x + 2.0;
      return 0.0;
    }
  } CatmullRom;
  static struct {
    static double radius() { return 1.5; }
    static double value(double x) {
      if (x < 0.5) return -2.0 * x * x + 1.0;
      if (x < 1.5) return x * x - 2.5 * x + 1.5;
      return 0.0;
    }
  } Quadratic;
  static struct {
    static double radius() { return 1.5; }
    static double value(double x) {
      if (x < 0.5) return -x * x + 0.75;
      if (x < 1.5) return 0.5 * x * x - 1.5 * x + 1.125;
      return 0.0;
    }
  } QuadraticBSpline;
  static struct {
    static double radius() { return 3.0; }
    static double value(double x) {
      if (x < 1.0) return 4.0 / 3.0 * x * x * x - 7.0 / 3.0 * x * x + 1.0;
      if (x < 2.0) return -7.0 / 12.0 * x * x * x + 3.0 * x * x - 59.0 / 12.0 * x + 2.5;
      if (x < 3.0) return 1.0 / 12.0 * x * x * x - 2.0 / 3.0 * x * x + 1.75 * x - 1.5;
      return 0.0;
    }
  } CubicConvolution;
  static struct {
    static double radius() { return 8.0; }
    static double sinc(double x) {
      if (x == 0.0) return 1.0;
      x *= PI();
      return std::sin(x) / x;
    }
    static double value(double x) {
      return (x < 8.0 ? sinc(x) * sinc(x / 8.0) : 0.0);
    }
  } Lanczos8;
};

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
  ImageBase(uint32 width, uint32 height, color_t const* bits) {
    data_ = new Data;
    data_->width_ = width;
    data_->height_ = height;
    data_->bits_ = new color_t[width * height];
    memcpy(data_->bits_, bits, width * height * sizeof(color_t));
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

  ImageBase<Format> subimage(int left, int top, int right, int bottom) const {
    if (left < 0) left = 0;
    if (top < 0) top = 0;
    if (right > data_->width_) right = data_->width_;
    if (bottom > data_->height_) bottom = data_->height_;
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
      static_cast<int>(data_->width_ * left + 0.5f),
      static_cast<int>(data_->height_ * top + 0.5f),
      static_cast<int>(data_->width_ * right + 0.5f),
      static_cast<int>(data_->height_ * bottom + 0.5f)
    );
  }

  ImageBase(File& file, ImageFormat::Type format = ImageFormat::Unknown);
  ImageBase(std::string const& path, ImageFormat::Type format = ImageFormat::Unknown);
  bool write(File& file, ImageFormat::Type format = ImageFormat::PNG);
  bool read(File& file, ImageFormat::Type format = ImageFormat::Unknown);
  bool write(std::string const& path, ImageFormat::Type format = ImageFormat::Unknown);
  bool read(std::string const& path, ImageFormat::Type format = ImageFormat::Unknown);

  template<typename Filter = decltype(ImageFilter::Lanczos3)>
  ImageBase<Format> resize(uint32 width, uint32 height, Filter filter = ImageFilter::Lanczos3) const;
  void blt(int x, int y, ImageBase<Format> const& src, int sx, int sy, uint32 sw, uint32 sh) {
    int dx = x - sx, dy = y - sy;
    int x0 = std::max(0, std::max(sx, -dx));
    int y0 = std::max(0, std::max(sy, -dy));
    int x1 = std::min<int>(src.width(), std::min<int>(sx + sw, data_->width_ - dx));
    int y1 = std::min<int>(src.height(), std::min<int>(sy + sh, data_->height_ - dy));
    if (y1 <= y0 || x1 <= x0) return;
    splice();
    for (int y = y0; y < y1; ++y) {
      color_t const* psrc = src.bits() + y * src.width() + x0;
      color_t* pdst = data_->bits_ + (y + dy) * data_->width_ + x0 + dx;
      for (int cnt = x1 - x0; cnt--;) {
        *pdst = Format::blend(*pdst, *psrc++);
        ++pdst;
      }
    }
  }
  void blt(int x, int y, ImageBase<Format> const& src) {
    blt(x, y, src, 0, 0, src.width(), src.height());
  }

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
  Image imResize(Image const& image, uint32 width, uint32 height, double radius, ImageFilter::Function filter);
  void imBlit(Image& dst, Image const& src, int x, int y, int sx, int sy, uint32 sw, uint32 sh);

  template<typename PF>
  class Accum {
  public:
    void add(typename PF::color_t value, double weight) {
      r += weight * PF::red::from(value);
      g += weight * PF::green::from(value);
      b += weight * PF::blue::from(value);
      a += weight * PF::alpha::from(value);
      w += weight;
    }
    typename PF::color_t get() const {
      if (w == 0) return 0;
      return PF::color_clamp(
        static_cast<int>(r / w + 0.5),
        static_cast<int>(g / w + 0.5),
        static_cast<int>(b / w + 0.5),
        static_cast<int>(a / w + 0.5)
        );
    }
  private:
    double r = 0, g = 0, b = 0, a = 0, w = 0;
  };

  template<class PF, class Filter>
  class LinScaler {
  public:
    LinScaler(uint32 from, uint32 to) {
      double scale = static_cast<double>(to) / from;
      if (scale < 1) {
        double radius = Filter::radius() / scale;
        for (uint32 i = 0; i < to; ++i) {
          double center = (i + 0.5) / scale;
          int left = std::max(0, static_cast<int>(floor(center - radius)));
          int right = std::min<int>(from, static_cast<int>(ceil(center + radius)) + 1);
          pixels.emplace_back(left, right - left);
          for (int j = left; j < right; ++j) {
            factors.push_back(Filter::value(std::abs((center - j - 0.5) * scale)));
          }
        }
      } else {
        double radius = Filter::radius();
        for (uint32 i = 0; i < to; ++i) {
          double center = (i + 0.5) / scale;
          int left = std::max(0, static_cast<int>(floor(center - radius)));
          int right = std::min<int>(from, static_cast<int>(ceil(center + radius)) + 1);
          pixels.emplace_back(left, right - left);
          for (int j = left; j < right; ++j) {
            factors.push_back(Filter::value(std::abs(center - j - 0.5)));
          }
        }
      }
    }
    void scale(typename PF::color_t const* src, uint32 src_pitch,
      typename PF::color_t* dst, uint32 dst_pitch)
    {
      double const* factor = &factors[0];
      for (auto& p : pixels) {
        typename PF::color_t const* from = src + p.first * src_pitch;
        Accum<PF> accum;
        for (uint32 i = p.second; i--;) {
          accum.add(*from, *factor++);
          from += src_pitch;
        }
        *dst = accum.get();
        dst += dst_pitch;
      }
    }
  private:
    std::vector<double> factors;
    std::vector<std::pair<uint32, uint32>> pixels;
  };
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
template<class PF>
template<typename Filter>
ImageBase<PF> ImageBase<PF>::resize(uint32 width, uint32 height, Filter filter) const {
  ImageBase<PF> cur(*this);
  if (cur.width() != width) {
    ImagePrivate::LinScaler<Format, Filter> scaler(cur.width(), width);
    ImageBase<PF> next(width, cur.height());
    for (uint32 y = 0; y < cur.height(); ++y) {
      scaler.scale(cur.bits() + y * cur.width(), 1, next.mutable_bits() + y * next.width(), 1);
    }
    cur = next;
  }
  if (cur.height() != height) {
    ImagePrivate::LinScaler<Format, Filter> scaler(cur.height(), height);
    ImageBase<PF> next(cur.width(), height);
    for (uint32 x = 0; x < cur.width(); ++x) {
      scaler.scale(cur.bits() + x, cur.width(), next.mutable_bits() + x, next.width());
    }
    cur = next;
  }
  return cur;
}
