#pragma once
#include "common.h"
#include "image.h"
#include <windows.h>
#include <bitset>
#include <deque>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "math3d.h"
#include "frameui/framewnd.h"

class GL : public WindowFrame {
  HDC hDC;
  HGLRC hRC;
  HFONT hFont;
  std::bitset<65536> lgen;
  std::deque<uint32> frames;
  bool drag;
  int dragX, dragY;
  Quaternion quat;
  float dist;
  bool active;
  int northo;
  uint32 color;
  struct Trackable {
    Vector pos;
    float radius;
    std::string text;
  };
  std::vector<Trackable> trackables;
  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override;
protected:
  virtual void doRender() {}
  virtual void onKey(int vk) {}
public:

  void setColor(uint32 color_) {
    color = color_;
  }

  void text(int x, int y, wchar_t const* str);
  void text(int x, int y, char const* str);
  void text(Vector const& v, char const* str);
  void trackable(Vector const& v, float r, char const* str);
  void beginTrackables();
  void endTrackables();
  void startOrtho();
  void endOrtho();

  void sphere(Vector const& c, float r);
  void capsule(Vector const& a, Vector const& b, float r);

  GL(Frame* parent);
  ~GL();

  uint32 genTexture(Image const& img);

  Image capture();

  void render();
};
