#pragma once

#include <windows.h>
#include "frameui/frame.h"
#include "frameui/window.h"

class WindowFrame : public Frame, public Window {
  HWND ownerWindow;
protected:
  void onMove(void* data);
  LRESULT onWndMessage(uint32 message, WPARAM wParam, LPARAM lParam);
protected:
  void create(std::string const& text, uint32 style, uint32 exStyle);
  void create(std::string const& wndClass, std::string const& text, uint32 style, uint32 exStyle);
  HWND getOwner() const {
    return ownerWindow;
  }
  HWND getParentWindow() const;
public:
  WindowFrame(Frame* parent);
};

class RootWindow : public RootFrame, public Window {
  LRESULT onControlMessage(HWND hControl, uint32 message, WPARAM wParam, LPARAM lParam);
  LRESULT onWndMessage(uint32 message, WPARAM wParam, LPARAM lParam);

  void* beginMoving();
  void endMoving(void* data);

  friend class WindowFrame;
  uint32 r_message;
  Frame* r_frame;

  Frame* c_frame;
public:
  RootWindow();
  ~RootWindow();

  static void setCapture(Frame* frame);
};
