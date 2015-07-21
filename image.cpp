#include "image.h"
#include "path.h"
#include <algorithm>

decltype(ImageFilter::Box) ImageFilter::Box;
decltype(ImageFilter::Triangle) ImageFilter::Triangle;
decltype(ImageFilter::Hermite) ImageFilter::Hermite;
decltype(ImageFilter::Bell) ImageFilter::Bell;
decltype(ImageFilter::CubicBSpline) ImageFilter::CubicBSpline;
decltype(ImageFilter::Lanczos3) ImageFilter::Lanczos3;
decltype(ImageFilter::Mitchell) ImageFilter::Mitchell;
decltype(ImageFilter::Cosine) ImageFilter::Cosine;
decltype(ImageFilter::CatmullRom) ImageFilter::CatmullRom;
decltype(ImageFilter::Quadratic) ImageFilter::Quadratic;
decltype(ImageFilter::QuadraticBSpline) ImageFilter::QuadraticBSpline;
decltype(ImageFilter::CubicConvolution) ImageFilter::CubicConvolution;
decltype(ImageFilter::Lanczos8) ImageFilter::Lanczos8;

namespace ImagePrivate {

  bool imReadPNG(Image& image, File& file);
  bool imWritePNG(Image const& image, File& file);

  typedef bool(*imReader)(Image& image, File& file);
  typedef bool(*imWriter)(Image const& image, File& file);

  imReader readers[ImageFormat::NumFormats] = {
    nullptr,
    imReadPNG,
  };
  imWriter writers[ImageFormat::NumFormats] = {
    imWritePNG,
    imWritePNG,
  };
  struct { char const* ext; ImageFormat::Type format; } extensions[] = {
    { ".png", ImageFormat::PNG },
  };

  ImageFormat::Type getFormat(std::string const& name) {
    std::string ext = path::ext(name);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    for (auto& fmt : extensions) {
      if (ext == fmt.ext) {
        return fmt.format;
      }
    }
    return ImageFormat::Unknown;
  }

  bool imWrite(Image const& image, File& file, ImageFormat::Type format) {
    if (writers[format]) {
      return writers[format](image, file);
    } else {
      return false;
    }
  }

  Image imRead(File& file, ImageFormat::Type format) {
    Image image;
    if (readers[format] && readers[format](image, file)) {
      return image;
    }
    for (auto reader : readers) {
      if (reader && reader(image, file)) {
        return image;
      }
    }
    image.release();
    return image;
  }

}
