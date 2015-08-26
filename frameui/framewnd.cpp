#include <windows.h>

#include "framewnd.h"

WindowFrame::WindowFrame(Frame* parent)
  : Frame(parent)
{
  Frame* cur = getParent();
  while (cur->getParent()) cur = cur->getParent();
  RootWindow* frm = dynamic_cast<RootWindow*>(cur);
  if (frm) ownerWindow = frm->getHandle();
  else ownerWindow = nullptr;
}

HWND WindowFrame::getParentWindow() const {
  return HWND_TOP;
  Frame* cur = getParent();
  WindowFrame* curWnd = nullptr;
  while (cur && !(curWnd = dynamic_cast<WindowFrame*>(cur))) {
    cur = cur->getParent();
  }
  if (cur && curWnd) {
    return curWnd->getHandle();
  } else {
    return ownerWindow;
  }
}

void WindowFrame::onMove(void* data) {
  if (hWnd) {
    uint32 flags = SWP_NOREPOSITION;
    HWND hWndInsertAfter = NULL;
    if (visible()) {
      if (IsWindowVisible(hWnd)) {
        flags |= SWP_NOZORDER;
      } else {
        flags |= SWP_SHOWWINDOW;
        hWndInsertAfter = getParentWindow();
      }
    } else {
      flags |= SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW;
    }
    if (data) {
      DeferWindowPos((HDWP)data, hWnd, hWndInsertAfter, left(), top(), width(), height(), flags);
    } else {
      SetWindowPos(hWnd, hWndInsertAfter, left(), top(), width(), height(), flags);
    }
  }
}
void WindowFrame::create(std::string const& text, uint32 style, uint32 exStyle) {
  Window::create(0, 0, 10, 10, text, style, exStyle, ownerWindow);
}
void WindowFrame::create(std::string const& wndClass, std::string const& text, uint32 style, uint32 exStyle) {
  Window::subclass(wndClass, 0, 0, 10, 10, text, style, exStyle, ownerWindow);
}

LRESULT WindowFrame::onWndMessage(uint32 message, WPARAM wParam, LPARAM lParam)
{
  uint32 result;
  if ((result = onMessage(message, wParam, lParam)) != M_UNHANDLED) {
    return result;
  }
  Frame* cur = getParent();
  while (cur->getParent()) cur = cur->getParent();
  RootWindow* frm = dynamic_cast<RootWindow*>(cur);
  if (frm) {
    frm->r_message = message;
    frm->r_frame = getParent();
  }
  result = Window::onWndMessage(message, wParam, lParam);
  if (frm) {
    frm->r_message = 0;
    frm->r_frame = NULL;
  }
  return result;
}

///////////////////////////////////////////////////////

RootWindow::RootWindow() 
  : r_message(0)
  , r_frame(nullptr)
  , c_frame(nullptr)
{
}
RootWindow::~RootWindow() {
}

void* RootWindow::beginMoving() {
  return BeginDeferWindowPos(32);
}
void RootWindow::endMoving(void* data) {
  EndDeferWindowPos((HDWP) data);
}

LRESULT RootWindow::onControlMessage(HWND hControl, uint32 message, WPARAM wParam, LPARAM lParam) {
  if (hControl == NULL) return onMessage(message, wParam, lParam);
  Window* control = Window::fromHandle(hControl);
  while (control == NULL) {
    hControl = GetParent(hControl);
    if (hControl == NULL || hControl == hWnd) break;
    control = Window::fromHandle(hControl);
  }
  Frame* cur = dynamic_cast<WindowFrame*>(control);
  uint32 result = M_UNHANDLED;
  while (cur && (result = cur->onMessage(message, wParam, lParam)) == M_UNHANDLED) {
    cur = cur->getParent();
  }
  return result;
}

void RootWindow::setCapture(Frame* frame) {
  Frame* cur = frame;
  while (cur->getParent()) cur = cur->getParent();
  RootWindow* frm = dynamic_cast<RootWindow*>(cur);
  if (frm) {
    if (frm->c_frame) frm->c_frame->onMessage(WM_CAPTURECHANGED, 0, 0);
    else SetCapture(frm->getHandle());
    frm->c_frame = frame;
  }
}

LRESULT RootWindow::onWndMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_DESTROY:
    shutdown();
    break;
  case WM_NOTIFY:
    {
      LRESULT result = onControlMessage(((NMHDR*) lParam)->hwndFrom, message, wParam, lParam);
      return result == M_UNHANDLED ? 0 : result;
    }
    break;
  case WM_COMMAND:
    {
      LRESULT result = onControlMessage((HWND) lParam, message, wParam, lParam);
      return result == M_UNHANDLED ? 0 : result;
    }
    break;
  case WM_CTLCOLORBTN:
  case WM_CTLCOLORDLG:
  case WM_CTLCOLOREDIT:
  case WM_CTLCOLORLISTBOX:
  case WM_CTLCOLORSCROLLBAR:
  case WM_CTLCOLORSTATIC:
    {
      LRESULT result = onControlMessage((HWND) lParam, message, wParam, lParam);
      if (result != M_UNHANDLED) return result;
    }
    break;
  case WM_DRAWITEM:
    {
      DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*) lParam;
      if (dis->CtlType != ODT_MENU) {
        LRESULT result = onControlMessage(dis->hwndItem, message, wParam, lParam);
        if (result != M_UNHANDLED) return result;
      }
    }
    break;
  case WM_SIZE:
    {
      RECT rc;
      GetClientRect(hWnd, &rc);
      setSize(rc.right, rc.bottom);
    }
    break;
  case WM_CAPTURECHANGED:
  case WM_LBUTTONDBLCLK:
  case WM_LBUTTONDOWN:
  case WM_LBUTTONUP:
  case WM_MBUTTONDBLCLK:
  case WM_MBUTTONDOWN:
  case WM_MBUTTONUP:
  case WM_RBUTTONDBLCLK:
  case WM_RBUTTONDOWN:
  case WM_RBUTTONUP:
  case WM_MOUSEWHEEL:
  case WM_MOUSEHWHEEL:
  case WM_XBUTTONDBLCLK:
  case WM_XBUTTONDOWN:
  case WM_XBUTTONUP:
  case WM_MOUSEMOVE:
    if (c_frame) {
      if (message != WM_CAPTURECHANGED) {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        x -= c_frame->left();
        y -= c_frame->top();
        lParam = MAKELONG(x, y);
      }
      uint32 result = c_frame->onMessage(message, wParam, lParam);
      if (message == WM_CAPTURECHANGED) c_frame = NULL;
      if (result != M_UNHANDLED) return result;
      else return 0;
    }
    break;
  }
  LRESULT result = M_UNHANDLED;
  if (message == r_message && r_frame) {
    Frame* cur = r_frame;
    while (cur && (result = cur->onMessage(message, wParam, lParam)) == M_UNHANDLED) {
      cur = cur->getParent();
    }
  } else {
    result = onMessage(message, wParam, lParam);
  }
  if (result != M_UNHANDLED) return result;
  return Window::onWndMessage(message, wParam, lParam);
}
