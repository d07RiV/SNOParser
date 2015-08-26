#pragma once

#include "common.h"

#define PT_TOPLEFT        0x00
#define PT_TOP            0x01
#define PT_TOPRIGHT       0x02
#define PT_LEFT           0x10
#define PT_CENTER         0x11
#define PT_RIGHT          0x12
#define PT_BOTTOMLEFT     0x20
#define PT_BOTTOM         0x21
#define PT_BOTTOMRIGHT    0x22

#define M_UNHANDLED       0x80000000
#define FM_SHUTDOWN       0x7F000001

class Frame {
  // master frame data
  friend class RootFrame;
  int mr_pos;
  int mr_moving;
  bool mr_valid;

  bool _visible;
  int _x;
  int _y;
  int _width;
  int _height;
  struct Anchor {
    enum {hLeft, hCenter, hRight, hWidth, vTop, vCenter, vBottom, vHeight, count};
    bool active;
    Frame* rel;
    uint32 relRatio;
    int offset;
  } anchors[8];

  RootFrame* master;
  Frame* parent;
  Frame* firstChild;
  Frame* lastChild;
  Frame* prevSibling;
  Frame* nextSibling;

  void onChangeVisibility(void* data);
protected:
  virtual void onMove(void* data) {}
public:
  Frame(Frame* parent);
  virtual ~Frame();

  // hierarchy
  void setParent(Frame* parent);
  Frame* getParent() const {
    return parent;
  }
  Frame* getFirstChild() const {
    return firstChild;
  }
  Frame* getLastChild() const {
    return lastChild;
  }
  Frame* getPrevSibling() const {
    return prevSibling;
  }
  Frame* getNextSibling() const {
    return nextSibling;
  }

  // visibility
  void show(bool s = true);
  void hide() {
    show(false);
  }
  bool visible() const {
    return _visible && mr_valid && (parent ? parent->visible() : true);
  }

  // positioning
  int left() const {
    return _x;
  }
  int top() const {
    return _y;
  }
  int right() const {
    return _x + _width;
  }
  int bottom() const {
    return _y + _height;
  }
  int width() const {
    return _width;
  }
  int height() const {
    return _height;
  }
  void setWidth(int width);
  void setHeight(int height);
  void setSize(int width, int height) {
    setWidth(width);
    setHeight(height);
  }
  void setPoint(int point, Frame* rel, int relPoint, int x, int y);
  void setPoint(int point, Frame* rel, int x, int y) {
    setPoint(point, rel, point, x, y);
  }
  void setPoint(int point, int x, int y) {
    setPoint(point, parent, point, x, y);
  }
  void setPointEx(int point, Frame* rel, double xRel, double yRel, int x, int y);
  void setPointEx(int point, double xRel, double yRel, int x, int y) {
    setPointEx(point, parent, xRel, yRel, x, y);
  }
  void clearAllPoints();
  void setAllPoints(Frame* rel) {
    setPoint(PT_TOPLEFT, rel, PT_TOPLEFT, 0, 0);
    setPoint(PT_BOTTOMRIGHT, rel, PT_BOTTOMRIGHT, 0, 0);
  }
  void setAllPoints() {
    setPoint(PT_TOPLEFT, parent, PT_TOPLEFT, 0, 0);
    setPoint(PT_BOTTOMRIGHT, parent, PT_BOTTOMRIGHT, 0, 0);
  }

  // M_UNHANDLED = unprocessed
  virtual LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
    return M_UNHANDLED;
  }
  LRESULT notify(uint32 message, WPARAM wParam, LPARAM lParam);
};

class RootFrame : public Frame {
  friend class Frame;

  std::vector<Frame*> frames;
  int moveid;
  bool updating;

  void addFrame(Frame* r);
  void updateFrame(Frame* r, void* data);
  void deepUpdateFrame(Frame* r);
  void setPoint(Frame* r, int point, Frame* rel, int relPoint, int x, int y);
  void setPointEx(Frame* r, int point, Frame* rel, uint32 xRel, uint32 yRel, int x, int y);
  void setWidth(Frame* r, int width);
  void setHeight(Frame* r, int height);
  void removeFrame(Frame* r);
protected:
  virtual void* beginMoving() { return 0; }
  virtual void endMoving(void* data) {}
  void shutdown();
public:
  RootFrame();
  ~RootFrame();

  void setSize(int width, int height);
};
