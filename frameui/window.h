#pragma once

#include <windows.h>
#include "common.h"

class Window
{
  static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  static ATOM windowClass;
  static std::map<HWND, Window*> handleMap;
  struct TTData;
  TTData* ttData;
  std::string regClass;
  bool continueModal;
protected:
  HWND hWnd;
  WNDPROC origProc;
  virtual LRESULT onWndMessage(uint32 message, WPARAM wParam, LPARAM lParam);
  WNDCLASSEX* createclass(std::string const& wndClass);
  void create(int x, int y, int width, int height, std::string const& text, uint32 style, uint32 exStyle,
    HWND parent = NULL);
  void create(std::string const& wndClass, int x, int y, int width, int height, std::string const& text, uint32 style,
    uint32 exStyle, HWND parent = NULL);
  void subclass(std::string const& wndClass, int x, int y, int width, int height, std::string const& text, uint32 style,
    uint32 exStyle, HWND parent = NULL);

  struct ToolInfo {
    RECT rc;
    std::string text;
  };
  virtual int toolHitTest(POINT pt, ToolInfo* ti);
  void endModal();
public:
  Window();
  virtual ~Window();

  operator HWND() const {
    return hWnd;
  }
  HWND getHandle() const {
    return hWnd;
  }

  static Window* fromHandle(HWND hWnd);

  // random functions
  void setText(std::string const& text);
  std::string getText() const;

  void setFont(HFONT hFont);
  HFONT getFont() const;
  void setTooltipFont(HFONT hFont);
  HFONT getTooltipFont();

  void enable(bool e = true);
  void disable() {
    enable(false);
  }
  void showWindow(bool s = true);
  void hideWindow() {
    showWindow(false);
  }
  void setRedraw(bool r);

  int id() const;
  void setId(int id);

  void invalidate(bool erase = true);

  void enableTooltips(bool enable = true);
  void setTooltip(char const* tip);

  int doModal();

  static std::string getWindowText(HWND hWnd);
};
