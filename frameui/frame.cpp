#include <stdlib.h>
#include <memory.h>
#include "frame.h"

RootFrame::RootFrame()
  : Frame(NULL)
{
  frames.push_back(this);
  master = this;
  frames[0]->master = this;
  frames[0]->mr_pos = 0;
  moveid = 0;
  updating = false;
  frames[0]->mr_valid = true;
  frames[0]->_x = 0;
  frames[0]->_y = 0;
  frames[0]->_width = 0;
  frames[0]->_height = 0;
}
RootFrame::~RootFrame() {
  frames[0]->master = NULL;
  for (size_t i = 1; i < frames.size(); i++) {
    frames[i]->master = NULL;
    delete frames[i];
  }
}
void RootFrame::shutdown() {
  for (Frame* frame : frames) {
    frame->onMessage(FM_SHUTDOWN, 0, 0);
  }
}
void RootFrame::setSize(int width, int height) {
  frames[0]->_width = width;
  frames[0]->_height = height;
  deepUpdateFrame(frames[0]);
}
void RootFrame::addFrame(Frame* r) {
  r->mr_pos = frames.size();
  frames.push_back(r);
}
void RootFrame::removeFrame(Frame* r) {
  for (size_t i = r->mr_pos + 1; i < frames.size(); ++i) {
    for (int j = 0; j < Frame::Anchor::count; ++j) {
      if (frames[i]->anchors[j].rel == r) {
        frames[i]->anchors[j].active = false;
      }
    }
    frames[i]->mr_pos = i - 1;
    frames[i - 1] = frames[i];
  }
  frames.pop_back();
}

void RootFrame::setPoint(Frame* r, int point, Frame* rel, int relPoint, int x, int y) {
  uint32 xRel = 0;
  if ((relPoint & 0x0F) == 0x01) { // hCenter
    xRel = 0x4000;
  } else if ((relPoint & 0x0F) == 0x02) { // hRight
    xRel = 0x8000;
  }
  uint32 yRel = 0;
  if ((relPoint & 0xF0) == 0x10) { // vCenter
    yRel = 0x4000;
  } else if ((relPoint & 0xF0) == 0x20) { // vBottom
    yRel = 0x8000;
  }
  setPointEx(r, point, rel, xRel, yRel, x, y);
}
void RootFrame::setPointEx(Frame* r, int point, Frame* rel, uint32 xRel, uint32 yRel, int x, int y) {
  if (rel == NULL) rel = frames[0];
  // move the tree
  if (!updating) moveid++;
  int cur = r->mr_pos;
  int target = rel->mr_pos;
  if (cur < target && updating) return;
  r->mr_moving = moveid;
  int count = 1;
  if (cur < target) {
    std::vector<Frame*> temp;
    temp.push_back(r);
    while (++cur <= target) {
      for (int i = 0; i < Frame::Anchor::count; i++) {
        if (frames[cur]->anchors[i].rel &&
            frames[cur]->anchors[i].rel->mr_moving == moveid)
        {
          frames[cur]->mr_moving = moveid;
          temp.push_back(frames[cur]);
          break;
        }
      }
      if (cur == target && frames[cur]->mr_moving == moveid) return;
    }
    count = 0;
    for (cur = r->mr_pos; cur <= target; cur++) {
      if (frames[cur]->mr_moving == moveid) {
        count++;
      } else {
        frames[cur]->mr_pos = cur - count;
        frames[cur - count] = frames[cur];
      }
    }
    for (size_t i = 0; i < temp.size(); i++) {
      temp[i]->mr_pos = target - count + i + 1;
      frames[target - count + i + 1] = temp[i];
    }
  } else {
    target = cur;
  }

  // add the anchor
  if ((point & 0x0F) == 0x00) { // hLeft
    r->anchors[Frame::Anchor::hCenter].active = false;
    if (r->anchors[Frame::Anchor::hRight].active) {
      r->anchors[Frame::Anchor::hWidth].active = false;
    }
    Frame::Anchor& a = r->anchors[Frame::Anchor::hLeft];
    a.active = true;
    a.rel = rel;
    a.relRatio = xRel;
    a.offset = x;
  } else if ((point & 0x0F) == 0x01) { // hCenter
    if (r->anchors[Frame::Anchor::hLeft].active == false &&
        r->anchors[Frame::Anchor::hRight].active == false)
    {
      Frame::Anchor& a = r->anchors[Frame::Anchor::hCenter];
      a.active = true;
      a.rel = rel;
      a.relRatio = xRel;
      a.offset = x;
    }
  } else { // hRight
    r->anchors[Frame::Anchor::hCenter].active = false;
    if (r->anchors[Frame::Anchor::hLeft].active) {
      r->anchors[Frame::Anchor::hWidth].active = false;
    }
    Frame::Anchor& a = r->anchors[Frame::Anchor::hRight];
    a.active = true;
    a.rel = rel;
    a.relRatio = xRel;
    a.offset = x;
  }
  if ((point & 0xF0) == 0x00) { // vTop
    r->anchors[Frame::Anchor::vCenter].active = false;
    if (r->anchors[Frame::Anchor::vBottom].active) {
      r->anchors[Frame::Anchor::vHeight].active = false;
    }
    Frame::Anchor& a = r->anchors[Frame::Anchor::vTop];
    a.active = true;
    a.rel = rel;
    a.relRatio = yRel;
    a.offset = y;
  } else if ((point & 0xF0) == 0x10) { // vCenter
    if (r->anchors[Frame::Anchor::vTop].active == false &&
        r->anchors[Frame::Anchor::vBottom].active == false)
    {
      Frame::Anchor& a = r->anchors[Frame::Anchor::vCenter];
      a.active = true;
      a.rel = rel;
      a.relRatio = yRel;
      a.offset = y;
    }
  } else { // vBottom
    r->anchors[Frame::Anchor::vCenter].active = false;
    if (r->anchors[Frame::Anchor::vTop].active) {
      r->anchors[Frame::Anchor::vHeight].active = false;
    }
    Frame::Anchor& a = r->anchors[Frame::Anchor::vBottom];
    a.active = true;
    a.rel = rel;
    a.relRatio = yRel;
    a.offset = y;
  }
  // update positions
  if (!updating) {
    updating = true;
    void* moveData = beginMoving();
    for (size_t cur = target - count + 1; cur < frames.size(); cur++) {
      for (int i = 0; i < Frame::Anchor::count && frames[cur]->mr_moving != moveid; i++) {
        if (frames[cur]->anchors[i].rel &&
            frames[cur]->anchors[i].rel->mr_moving == moveid) {
          frames[cur]->mr_moving = moveid;
        }
      }
      if (frames[cur]->mr_moving == moveid) updateFrame(frames[cur], moveData);
    }
    endMoving(moveData);
    updating = false;
  }
}
void RootFrame::setWidth(Frame* r, int width) {
  if (r->anchors[Frame::Anchor::hLeft].active &&
      r->anchors[Frame::Anchor::hRight].active) {
    return;
  }
  Frame::Anchor& c = r->anchors[Frame::Anchor::hWidth];
  c.active = true;
  c.offset = width;
  deepUpdateFrame(r);
}
void RootFrame::setHeight(Frame* r, int height) {
  if (r->anchors[Frame::Anchor::vTop].active &&
      r->anchors[Frame::Anchor::vBottom].active) {
    return;
  }
  Frame::Anchor& c = r->anchors[Frame::Anchor::vHeight];
  c.active = true;
  c.offset = height;
  deepUpdateFrame(r);
}
void RootFrame::deepUpdateFrame(Frame* r) {
  if (!updating) {
    updating = true;
    void* moveData = beginMoving();
    moveid++;
    r->mr_moving = moveid;
    for (size_t cur = r->mr_pos; cur < frames.size(); cur++) {
      for (int i = 0; i < Frame::Anchor::count && frames[cur]->mr_moving != moveid; i++) {
        if (frames[cur]->anchors[i].rel &&
            frames[cur]->anchors[i].rel->mr_moving == moveid) {
          frames[cur]->mr_moving = moveid;
        }
      }
      if (frames[cur]->mr_moving == moveid) updateFrame(frames[cur], moveData);
    }
    endMoving(moveData);
    updating = false;
  }
}
inline int getRegionX(Frame* r, uint32 ratio) {
  return r->left() + ((r->width() * ratio) >> 15);
}
inline int getRegionY(Frame* r, uint32 ratio) {
  return r->top() + ((r->height() * ratio) >> 15);
}
void RootFrame::updateFrame(Frame* r, void* data) {
  int oldLeft = r->left();
  int oldTop = r->top();
  int oldRight = r->right();
  int oldBottom = r->bottom();
  if (r->anchors[Frame::Anchor::hLeft].active &&
      r->anchors[Frame::Anchor::hRight].active)
  {
    Frame::Anchor& la = r->anchors[Frame::Anchor::hLeft];
    Frame::Anchor& ra = r->anchors[Frame::Anchor::hRight];
    if (la.rel && la.rel->mr_valid && ra.rel && ra.rel->mr_valid) {
      r->_x = getRegionX(la.rel, la.relRatio) + la.offset;
      r->_width = getRegionX(ra.rel, ra.relRatio) + ra.offset - r->_x;
    } else {
      if (r->mr_valid) {
        r->mr_valid = false;
        r->onChangeVisibility(data);
      }
      return;
    }
  } else if (r->anchors[Frame::Anchor::hWidth].active) {
    r->_width = r->anchors[Frame::Anchor::hWidth].offset;
    Frame::Anchor& la = r->anchors[Frame::Anchor::hLeft];
    Frame::Anchor& ra = r->anchors[Frame::Anchor::hRight];
    Frame::Anchor& ca = r->anchors[Frame::Anchor::hCenter];
    if (la.active && la.rel && la.rel->mr_valid) {
      r->_x = getRegionX(la.rel, la.relRatio) + la.offset;
    } else if (ra.active && ra.rel && ra.rel->mr_valid) {
      r->_x = getRegionX(ra.rel, ra.relRatio) + ra.offset - r->_width;
    } else if (ca.active && ca.rel && ca.rel->mr_valid) {
      r->_x = getRegionX(ca.rel, ca.relRatio) + ca.offset - r->_width / 2;
    } else {
      if (r->mr_valid) {
        r->mr_valid = false;
        r->onChangeVisibility(data);
      }
      return;
    }
  }
  if (r->anchors[Frame::Anchor::vTop].active &&
      r->anchors[Frame::Anchor::vBottom].active)
  {
    Frame::Anchor& ta = r->anchors[Frame::Anchor::vTop];
    Frame::Anchor& ba = r->anchors[Frame::Anchor::vBottom];
    if (ta.rel && ta.rel->mr_valid && ba.rel && ba.rel->mr_valid) {
      r->_y = getRegionY(ta.rel, ta.relRatio) + ta.offset;
      r->_height = getRegionY(ba.rel, ba.relRatio) + ba.offset - r->_y;
    } else {
      if (r->mr_valid) {
        r->mr_valid = false;
        r->onChangeVisibility(data);
      }
      return;
    }
  } else if (r->anchors[Frame::Anchor::vHeight].active) {
    r->_height = r->anchors[Frame::Anchor::vHeight].offset;
    Frame::Anchor& ta = r->anchors[Frame::Anchor::vTop];
    Frame::Anchor& ba = r->anchors[Frame::Anchor::vBottom];
    Frame::Anchor& ca = r->anchors[Frame::Anchor::vCenter];
    if (ta.active && ta.rel && ta.rel->mr_valid) {
      r->_y = getRegionY(ta.rel, ta.relRatio) + ta.offset;
    } else if (ba.active && ba.rel && ba.rel->mr_valid) {
      r->_y = getRegionY(ba.rel, ba.relRatio) + ba.offset - r->_height;
    } else if (ca.active && ca.rel && ca.rel->mr_valid) {
      r->_y = getRegionY(ca.rel, ca.relRatio) + ca.offset - r->_height / 2;
    } else {
      if (r->mr_valid) {
        r->mr_valid = false;
        r->onChangeVisibility(data);
      }
      return;
    }
  }
  if (r->left() != oldLeft || r->top() != oldTop ||
      r->right() != oldRight || r->bottom() != oldBottom ||
      !r->mr_valid)
  {
    if (!r->mr_valid) {
      r->mr_valid = true;
      r->onChangeVisibility(data);
    } else {
      r->onMove(data);
    }
  }
}

////////////////////////////

Frame::Frame(Frame* _parent)
  : parent(_parent)
  , master(nullptr)
  , mr_pos(0)
  , mr_moving(0)
  , mr_valid(false)
  , _x(0)
  , _y(0)
  , _width(0)
  , _height(0)
  , _visible(true)
{
  memset(anchors, 0, sizeof anchors);
  if (parent) master = parent->master;
  if (master) master->addFrame(this);

  firstChild = NULL;
  lastChild = NULL;
  if (parent) {
    prevSibling = parent->lastChild;
    if (parent->lastChild) parent->lastChild->nextSibling = this;
    else parent->firstChild = this;
    parent->lastChild = this;
  } else {
    prevSibling = NULL;
  }
  nextSibling = NULL;
}
Frame::~Frame() {
  setParent(NULL);
  Frame* cur = firstChild;
  while (cur) {
    Frame* next = cur->nextSibling;
    cur->parent = NULL;
    cur->prevSibling = NULL;
    cur->nextSibling = NULL;
    cur = next;
  }
  if (master) master->removeFrame(this);
}

void Frame::setParent(Frame* _parent) {
  if (parent == _parent) return;
  if (parent) {
    if (prevSibling) prevSibling->nextSibling = nextSibling;
    else parent->firstChild = nextSibling;
    if (nextSibling) nextSibling->prevSibling = prevSibling;
    else parent->lastChild = prevSibling;
    prevSibling = NULL;
    nextSibling = NULL;
  }
  parent = _parent;
  if (parent) {
    prevSibling = parent->lastChild;
    nextSibling = NULL;
    if (parent->lastChild) parent->lastChild->nextSibling = this;
    else parent->firstChild = this;
    parent->lastChild = this;
  }
}

void Frame::onChangeVisibility(void* data) {
  onMove(data);
  for (Frame* cur = firstChild; cur; cur = cur->nextSibling) {
    if (cur->_visible) cur->onChangeVisibility(data);
  }
}
void Frame::show(bool s) {
  if (s != _visible) {
    _visible = s;
    void* moveData = master->beginMoving();
    onChangeVisibility(moveData);
    master->endMoving(moveData);
  }
}

LRESULT Frame::notify(uint32 message, WPARAM wParam, LPARAM lParam) {
  Frame* cur = this;
  uint32 result = M_UNHANDLED;
  while (cur && (result = cur->onMessage(message, wParam, lParam)) == M_UNHANDLED) {
    cur = cur->getParent();
    if (cur == this) break;
  }
  return result;
}

void Frame::setWidth(int width) {
  master->setWidth(this, width);
}
void Frame::setHeight(int height) {
  master->setHeight(this, height);
}
void Frame::setPoint(int point, Frame* rel, int relPoint, int x, int y) {
  master->setPoint(this, point, rel, relPoint, x, y);
}
void Frame::setPointEx(int point, Frame* rel, double xRel, double yRel, int x, int y) {
  master->setPointEx(this, point, rel, uint32(xRel * 0x8000), uint32(yRel * 0x8000), x, y);
}
void Frame::clearAllPoints() {
  for (int i = 0; i < Anchor::count; i++) {
    anchors[i].active = false;
  }
  anchors[Anchor::hWidth].active = true;
  anchors[Anchor::vHeight].active = true;
  master->deepUpdateFrame(this);
}

////////////////////////////////////
