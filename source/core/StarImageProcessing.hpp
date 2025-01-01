#ifndef STAR_IMAGE_PROCESSING_HPP
#define STAR_IMAGE_PROCESSING_HPP

#include "StarList.hpp"
#include "StarRect.hpp"
#include "StarJson.hpp"

namespace Star {

typedef Vector<uint8_t, 5> Vec5B;

STAR_CLASS(Image);

STAR_EXCEPTION(ImageOperationException, StarException);

Image scaleNearest(Image const& srcImage, Vec2F scale);
#if defined XSB_DISABLED // STAR_COMPILER_GNU
// FezzedOne: Needed to emulate MSVC-like floating-point behaviour on GCC.
Image scaleBilinear(Image const& srcImage, Vec2F scale) __attribute__ ((optimize("O2")));
#else
Image scaleBilinear(Image const& srcImage, Vec2F scale);
#endif
Image scaleBicubic(Image const& srcImage, Vec2F scale);

StringList colorDirectivesFromConfig(JsonArray const& directives);
String paletteSwapDirectivesFromConfig(Json const& swaps);

struct NullImageOperation {

};

struct ErrorImageOperation {
  std::exception_ptr exception;
};

struct HueShiftImageOperation {
  // Specify hue shift angle as -360 to 360 rather than -1 to 1
  static HueShiftImageOperation hueShiftDegrees(float degrees);

  // value here is normalized to 1.0
  float hueShiftAmount;
};

struct SaturationShiftImageOperation {
  // Specify saturation shift as amount normalized to 100
  static SaturationShiftImageOperation saturationShift100(float amount);

  // value here is normalized to 1.0
  float saturationShiftAmount;
};

struct BrightnessMultiplyImageOperation {
  // Specify brightness multiply as amount where 0 means "no change" and 100
  // means "x2" and -100 means "x0"
  static BrightnessMultiplyImageOperation brightnessMultiply100(float amount);

  float brightnessMultiply;
};

// Fades R G and B channels to the given color by the given amount, ignores A
struct FadeToColorImageOperation {
  FadeToColorImageOperation(Vec3B color, float amount);

  Vec3B color;
  float amount;

  Array<uint8_t, 256> rTable;
  Array<uint8_t, 256> gTable;
  Array<uint8_t, 256> bTable;
};

// Applies two FadeToColor operations in alternating rows to produce a scanline effect
struct ScanLinesImageOperation {
  FadeToColorImageOperation fade1;
  FadeToColorImageOperation fade2;
};

// Sets RGB values to the given color, and ignores the alpha channel
struct SetColorImageOperation {
  Vec3B color;
};

typedef HashMap<Vec4B, Vec4B> ColorReplaceMap;
typedef HashMap<Vec5B, Vec4B> ColorReplaceMap5;

struct ColorReplaceImageOperation {
  ColorReplaceMap5 colorReplaceMap;
};

struct AlphaMaskImageOperation {
  enum MaskMode {
    Additive,
    Subtractive
  };

  MaskMode mode;
  StringList maskImages;
  Vec2I offset;
};

struct BlendImageOperation {
  enum BlendMode {
    Multiply,
    Screen
  };

  BlendMode mode;
  StringList blendImages;
  Vec2I offset;
};

struct MultiplyImageOperation {
  Vec4B color;
};

struct BorderImageOperation {
  unsigned pixels;
  Vec4B startColor;
  Vec4B endColor;
  bool outlineOnly;
  bool includeTransparent;
};

struct ScaleImageOperation {
  enum Mode {
    Nearest,
    Bilinear,
    Bicubic,
    // FezzedOne: Unlike `Nearest`, ignores humanoid pre-scaling.
    NearestPixel
  };

  Mode mode;
  Vec2F scale;
  Vec2F rawScale;
};

struct CropImageOperation {
  RectI subset;
};

struct FlipImageOperation {
  enum Mode {
    FlipX,
    FlipY,
    FlipXY
  };
  Mode mode;
};

struct SetPixelImageOperation {
  Vec2U pixel;
  Vec4B colour;
};

struct BlendPixelImageOperation {
  Vec2U pixel;
  Vec4B colour;
};

struct CopyIntoImageOperation {
  Vec2U offset;
  String image;
};

struct DrawIntoImageOperation {
  Vec2U offset;
  String image;
};

typedef Variant<NullImageOperation, ErrorImageOperation, HueShiftImageOperation, SaturationShiftImageOperation, BrightnessMultiplyImageOperation, FadeToColorImageOperation,
  ScanLinesImageOperation, SetColorImageOperation, ColorReplaceImageOperation, AlphaMaskImageOperation, BlendImageOperation,
  MultiplyImageOperation, BorderImageOperation, ScaleImageOperation, CropImageOperation, FlipImageOperation,
  SetPixelImageOperation, BlendPixelImageOperation, CopyIntoImageOperation, DrawIntoImageOperation> ImageOperation;

ImageOperation imageOperationFromString(StringView string);
String imageOperationToString(ImageOperation const& operation);

void parseImageOperations(StringView params, function<void(ImageOperation&&)> outputter);

// Each operation is assumed to be separated by '?', with parameters
// separated by ';' or '='
List<ImageOperation> parseImageOperations(StringView params);

// Each operation separated by '?', returns string with leading '?'
String printImageOperations(List<ImageOperation> const& operations);

void addImageOperationReferences(ImageOperation const& operation, StringList& out);

StringList imageOperationReferences(List<ImageOperation> const& operations);

typedef function<Image const*(String const& refName)> ImageReferenceCallback;

void processImageOperation(ImageOperation const& operation, Image& input, ImageReferenceCallback refCallback = {});

Image processImageOperations(List<ImageOperation> const& operations, Image input, ImageReferenceCallback refCallback = {});

}

#endif
