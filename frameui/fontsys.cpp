#include "fontsys.h"
#include <algorithm>

FontSys FontSys::instance;

//#define MKSIZE(s)     (-MulDiv(s,instance.logPixelsY,72))
#define MKSIZE(s)     (-(s))

FontSys::FontStruct::FontStruct(LOGFONT const& lf) {
  font = CreateFontIndirect(&lf);
  face = lf.lfFaceName;
  size = lf.lfHeight;
  flags = 0;
  if (lf.lfWeight > FW_NORMAL) flags |= FONT_BOLD;
  if (lf.lfItalic) flags |= FONT_ITALIC;
  if (lf.lfUnderline) flags |= FONT_UNDERLINE;
  if (lf.lfStrikeOut) flags |= FONT_STRIKEOUT;
}

bool operator<(FontSys::FontStruct const& lhs, FontSys::FontStruct const& rhs) {
  if (lhs.size != rhs.size) return lhs.size < rhs.size;
  if (lhs.flags != rhs.flags) return lhs.flags < rhs.flags;
  return lhs.face < rhs.face;
}
bool operator==(FontSys::FontStruct const& lhs, FontSys::FontStruct const& rhs) {
  if (lhs.size != rhs.size) return false;
  if (lhs.flags != rhs.flags) return false;
  return lhs.face == rhs.face;
}

FontSys::FontSys() {
  LOGFONT lf;
  memset (&lf, 0, sizeof lf);
  lf.lfHeight = -11;
  lf.lfWeight = FW_NORMAL;
  strcpy(lf.lfFaceName, "MS Shell Dlg 2");
  lf.lfCharSet = DEFAULT_CHARSET;

  fonts.emplace_back(lf);

  hDC = CreateCompatibleDC(NULL);
  logPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
}

FontSys::~FontSys() {
  DeleteDC(hDC);
}

HFONT FontSys::getSysFont() {
  return instance.fonts[0].font;
}
HFONT FontSys::_getFont(int height, std::string const& face, int flags) {
  FontStruct fs;
  fs.size = height;
  fs.flags = flags;
  fs.face = face;
  auto pos = std::lower_bound(fonts.begin() + 1, fonts.end(), fs);
  if (pos != fonts.end() && *pos == fs) {
    return pos->font;
  }

  LOGFONT lf;
  getLogFont(&lf, -height, face, flags);
  return fonts.emplace(pos, lf)->font;
}

void FontSys::getLogFont(LOGFONT* lf, int size, std::string const& face, int flags) {
  GetObject(instance.fonts[0].font, sizeof(LOGFONT), lf);
  lf->lfHeight = MKSIZE(size);
  strcpy(lf->lfFaceName, face.c_str());
  lf->lfWeight = (flags & FONT_BOLD ? FW_BOLD : FW_NORMAL);
  lf->lfItalic = (flags & FONT_ITALIC ? TRUE : FALSE);
  lf->lfUnderline = (flags & FONT_UNDERLINE ? TRUE : FALSE);
  lf->lfStrikeOut = (flags & FONT_STRIKEOUT ? TRUE : FALSE);
}
HFONT FontSys::getFont(int size, std::string const& face, int flags) {
  return instance._getFont(MKSIZE(size), face, flags);
}
HFONT FontSys::getFont(int size, int flags) {
  return instance._getFont(MKSIZE(size), instance.fonts[0].face, flags);
}
HFONT FontSys::changeSize(int size, HFONT oldFont) {
  if (oldFont == NULL) oldFont = getSysFont();
  LOGFONT lf;
  GetObject(oldFont, sizeof lf, &lf);
  int flags = 0;
  if (lf.lfWidth > FW_NORMAL) flags |= FONT_BOLD;
  if (lf.lfItalic) flags |= FONT_ITALIC;
  if (lf.lfUnderline) flags |= FONT_UNDERLINE;
  if (lf.lfStrikeOut) flags |= FONT_STRIKEOUT;
  return instance._getFont(MKSIZE(size), lf.lfFaceName, flags);
}
HFONT FontSys::changeFlags(int flags, HFONT oldFont) {
  if (oldFont == NULL) oldFont = getSysFont();
  LOGFONT lf;
  GetObject(oldFont, sizeof lf, &lf);
  return instance._getFont(lf.lfHeight, lf.lfFaceName, flags);
}

SIZE FontSys::getTextSize(HFONT font, std::string const& text) {
  SIZE sz;
  SelectObject(instance.hDC, font);
  GetTextExtentPoint32(instance.hDC, text.c_str(), text.length(), &sz);
  return sz;
}
SIZE FontSys::getTextSize(HFONT font, char const* text, int length) {
  SIZE sz;
  SelectObject(instance.hDC, font);
  GetTextExtentPoint32(instance.hDC, text, length, &sz);
  return sz;
}
int FontSys::getMTextHeight(HFONT font, int width, std::string const& text) {
  SIZE sz;
  SelectObject(instance.hDC, font);
  std::string word;
  std::string curline;
  int height = 0;
  for (int i = 0; i <= text.length (); i++) {
    if (text[i] && text[i] != '\n' && !isspace((uint8)text[i])) {
      word += text[i];
    }
    if (text[i] == 0 || isspace((uint8) text[i])) {
      std::string tmp = curline + word;
      GetTextExtentPoint32(instance.hDC, tmp.c_str(), tmp.length(), &sz);
      int wd = sz.cx;
      if (text[i] == '\n' || (wd > width && !curline.empty())) {
        GetTextExtentPoint32(instance.hDC, curline.c_str(), curline.length(), &sz);
        height += sz.cy;
        curline.clear();
      }
      curline += word;
      if (text[i]) curline += text[i];
      word.clear();
    }
  }
  if (!curline.empty()) {
    GetTextExtentPoint32(instance.hDC, curline.c_str(), curline.length(), &sz);
    height += sz.cy;
  }
  return height;
}

int FontSys::getFlags(HFONT font) {
  if (font == NULL) font = getSysFont();
  LOGFONT lf;
  GetObject(font, sizeof lf, &lf);
  int flags = 0;
  if (lf.lfWidth > FW_NORMAL) flags |= FONT_BOLD;
  if (lf.lfItalic) flags |= FONT_ITALIC;
  if (lf.lfUnderline) flags |= FONT_UNDERLINE;
  if (lf.lfStrikeOut) flags |= FONT_STRIKEOUT;
  return flags;
}

void FontSys::setFontName(HFONT hFont, std::string const& name) {
  instance.byName[name] = hFont;
}
HFONT FontSys::getFontByName(std::string const& name) {
  auto it = instance.byName.find(name);
  return (it == instance.byName.end() ? instance.fonts[0].font : it->second);
}

HFONT FontSys::getFont(LOGFONT const& lf) {
  int flags = 0;
  if (lf.lfWidth > FW_NORMAL) flags |= FONT_BOLD;
  if (lf.lfItalic) flags |= FONT_ITALIC;
  if (lf.lfUnderline) flags |= FONT_UNDERLINE;
  if (lf.lfStrikeOut) flags |= FONT_STRIKEOUT;
  return instance._getFont(lf.lfHeight, lf.lfFaceName, flags);
}
