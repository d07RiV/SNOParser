#define NOMINMAX
#include <windows.h>
#include <windowsx.h>

#include "frameui/framewnd.h"
#include "frameui/fontsys.h"
#include "image.h"

#include "controlframes.h"

ButtonFrame::ButtonFrame(std::string const& text, Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create("Button", text, WS_CHILD | WS_TABSTOP | style, 0);
  setFont(FontSys::getSysFont());
  setId(id);
}
LRESULT ButtonFrame::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_ERASEBKGND) {
    if ((GetWindowLong(hWnd, GWL_STYLE) & BS_TYPEMASK) == BS_GROUPBOX) {
      HDC hDC = (HDC) wParam;
      HBRUSH hBrush = (HBRUSH) GetClassLongPtr(GetParent(hWnd), GCLP_HBRBACKGROUND);
      RECT rc;
      GetClientRect(hWnd, &rc);
      FillRect(hDC, &rc, hBrush);
      InvalidateRect(hWnd, NULL, FALSE);
      return TRUE;
    }
  }
  return M_UNHANDLED;
}

///////////////////////////////////////////////////////

HotkeyFrame::HotkeyFrame(Frame* parent, int id)
  : WindowFrame(parent)
{
  create(HOTKEY_CLASS, "", WS_CHILD | WS_TABSTOP | ES_WANTRETURN, 0);
  setFont(FontSys::getSysFont());
  setId(id);
}

///////////////////////////////////////////////////////

LinkFrame::LinkFrame(std::string const& text, Frame* parent, int id)
  : WindowFrame(parent)
  , _color(0xFF0000)
  , _flags(0)
  , hFont(NULL)
  , uFont(NULL)
  , pressed(false)
  , hover(false)
{
  if (WNDCLASSEX* wcx = createclass("LinkFrmClass")) {
    wcx->hCursor = LoadCursor(NULL, IDC_HAND);
    RegisterClassEx(wcx);
  }
  create(text, WS_CHILD, 0);
  setFont(FontSys::getSysFont());
  resetSize();
  setId(id);
}

void LinkFrame::resetSize() {
  HDC hDC = GetDC(hWnd);
  SelectObject(hDC, hFont);
  SIZE sz;
  std::string const& text = getText();
  GetTextExtentPoint32(hDC, text.c_str(), text.length(), &sz);
  ReleaseDC(hWnd, hDC);
  setSize(sz.cx, sz.cy);
}
void LinkFrame::setColor(uint32 color) {
  _color = color;
  InvalidateRect(hWnd, NULL, TRUE);
}
void LinkFrame::setFlags(int flags) {
  _flags = flags;
  InvalidateRect(hWnd, NULL, TRUE);
}

LRESULT LinkFrame::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_SETFONT:
    hFont = (wParam ? (HFONT) wParam : FontSys::getSysFont());
    uFont = FontSys::changeFlags(FontSys::getFlags(hFont) | FONT_UNDERLINE, hFont);
    if (lParam) InvalidateRect(hWnd, NULL, TRUE);
    return 0;
  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hDC = BeginPaint(hWnd, &ps);

      POINT pt;
      GetCursorPos(&pt);
      ScreenToClient(hWnd, &pt);

      SetBkMode(hDC, OPAQUE);
      SetBkColor(hDC, GetSysColor(COLOR_BTNFACE));
      SetTextColor(hDC, _color);
      if (pressed || hover) SelectObject(hDC, uFont);
      else SelectObject(hDC, hFont);
      RECT rc = {0, 0, width(), height()};
      std::string const& text = getText();
      DrawText(hDC, text.c_str(), text.length(), &rc, _flags);

      EndPaint(hWnd, &ps);
    }
    return 0;
  case WM_MOUSEMOVE:
    {
      int x = LOWORD(lParam);
      int y = HIWORD(lParam);
      if (!hover && x > 0 && y > 0 && x < width() && y < height()) {
        hover = true;
        TRACKMOUSEEVENT tme;
        memset(&tme, 0, sizeof tme);
        tme.cbSize = sizeof tme;
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = hWnd;
        TrackMouseEvent(&tme);
        if (!pressed) InvalidateRect(hWnd, NULL, FALSE);
      }
    }
    return 0;
  case WM_MOUSELEAVE:
    {
      hover = false;
      TRACKMOUSEEVENT tme;
      memset(&tme, 0, sizeof tme);
      tme.cbSize = sizeof tme;
      tme.dwFlags = TME_CANCEL | TME_LEAVE;
      tme.hwndTrack = hWnd;
      TrackMouseEvent(&tme);
      if (!pressed) InvalidateRect(hWnd, NULL, TRUE);
    }
    return 0;
  case WM_LBUTTONDOWN:
    SetCapture(hWnd);
    pressed = true;
    return 0;
  case WM_LBUTTONUP:
    if (pressed) {
      ReleaseCapture();
      pressed = false;
      if (hover) notify(WM_COMMAND, MAKELONG(id(), BN_CLICKED), (LPARAM) hWnd);
    }
    return 0;
  }
  return M_UNHANDLED;
}

///////////////////////////////////////////////////////

LRESULT EditFrame::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_CTLCOLOREDIT || message == WM_CTLCOLORSTATIC) {
    HDC hDC = (HDC) wParam;
    if ((fgcolor & 0xFF000000) == 0) SetTextColor(hDC, fgcolor);
    if ((bgcolor & 0xFF000000) == 0) SetBkColor(hDC, bgcolor);
    if (background) return (uint32) background;
    return M_UNHANDLED;
  }
  return M_UNHANDLED;
}
EditFrame::EditFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  bgcolor = 0xFFFFFFFF;
  fgcolor = 0xFFFFFFFF;
  background = NULL;
  create("Edit", "", style | WS_CHILD | WS_TABSTOP, WS_EX_CLIENTEDGE);
  setFont(FontSys::getSysFont());
  setId(id);
}
EditFrame::~EditFrame() {
  if (background) DeleteObject(background);
}
void EditFrame::setFgColor(uint32 color) {
  fgcolor = color & 0xFFFFFF;
  invalidate();
}
void EditFrame::setBgColor(uint32 color) {
  bgcolor = color & 0xFFFFFF;
  if (background) DeleteObject(background);
  background = CreateSolidBrush(color);
  invalidate();
}

///////////////////////////////////////////////////////

void ComboFrame::onMove(void* data) {
  if (hWnd) {
    uint32 flags = SWP_NOREPOSITION;
    HWND hWndInsertAfter = NULL;
    if (visible()) {
      if (IsWindowVisible(hWnd)) {
        flags |= SWP_NOZORDER;
      } else {
        flags |= SWP_SHOWWINDOW;// | SWP_NOZORDER;
        hWndInsertAfter = HWND_TOP;// getParentWindow();
      }
    } else {
      flags |= SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW;
    }
    if (data) {
      DeferWindowPos((HDWP)data, hWnd, hWndInsertAfter, left(), top(), width(), boxHeight, flags);
    } else {
      SetWindowPos(hWnd, hWndInsertAfter, left(), top(), width(), boxHeight, flags);
    }
  }
}
ComboFrame::ComboFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  boxHeight = 500;
  create("ComboBox", "", style | WS_CHILD | WS_TABSTOP | WS_VSCROLL, 0);
  setFont(FontSys::getSysFont());
  setId(id);
  setHeight(21);
}
void ComboFrame::reset() {
  SendMessage(hWnd, CB_RESETCONTENT, 0, 0);
}
int ComboFrame::addString(std::string const& text, int data) {
  int id = SendMessage(hWnd, CB_ADDSTRING, 0, (LPARAM) text.c_str());
  if (id != CB_ERR) SendMessage(hWnd, CB_SETITEMDATA, id, data);
  return id;
}
void ComboFrame::delString(int pos) {
  SendMessage(hWnd, CB_DELETESTRING, pos, 0);
}
int ComboFrame::getItemData(int item) const {
  return SendMessage(hWnd, CB_GETITEMDATA, item, 0);
}
void ComboFrame::setItemData(int item, int data) {
  SendMessage(hWnd, CB_SETITEMDATA, item, data);
}
int ComboFrame::getCurSel() const {
  return SendMessage(hWnd, CB_GETCURSEL, 0, 0);
}
void ComboFrame::setCurSel(int sel) {
  SendMessage(hWnd, CB_SETCURSEL, sel, 0);
}

///////////////////////////////////////////////////////

StaticFrame::StaticFrame(Frame* parent, int id, int style, int exStyle)
  : WindowFrame(parent)
{
  create("Static", "", style | SS_NOTIFY | WS_CHILD, exStyle);
  setFont(FontSys::getSysFont());
  setId(id);
}
StaticFrame::StaticFrame(std::string const& text, Frame* parent, int id, int style, int exStyle)
  : WindowFrame(parent)
{
  create("Static", text, style | SS_NOTIFY | WS_CHILD, exStyle);
  setFont(FontSys::getSysFont());
  resetSize();
  setId(id);
}
void StaticFrame::setImage(HANDLE image, int type) {
  SendMessage(hWnd, STM_SETIMAGE, (WPARAM) type, (LPARAM) image);
}
void StaticFrame::resetSize() {
  HDC hDC = GetDC(hWnd);
  SelectObject(hDC, getFont());
  SIZE sz;
  std::string text = getText();
  GetTextExtentPoint32(hDC, text.c_str(), text.length(), &sz);
  ReleaseDC(hWnd, hDC);
  setSize(sz.cx, sz.cy);
}

///////////////////////////////////////////////////////

#include "richedit.h"

struct EditStreamCookie {
  std::string const& str;
  int pos;
  EditStreamCookie(std::string const& str)
    : str(str)
    , pos(0)
  {}
};
DWORD CALLBACK RichEditFrame::StreamCallback(DWORD_PTR cookie, LPBYTE buff, LONG cb, LONG* pcb)
{
  EditStreamCookie* ck = (EditStreamCookie*) cookie;
  *pcb = ck->str.length() - ck->pos;
  if (*pcb > cb) *pcb = cb;
  if (*pcb) memcpy(buff, ck->str.c_str() + ck->pos, *pcb);
  ck->pos += *pcb;
  return 0;
}
RichEditFrame::RichEditFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(RICHEDIT_CLASS, "", style | WS_CHILD | WS_TABSTOP, WS_EX_CLIENTEDGE);
  setFont(FontSys::getSysFont());
  setId(id);
}
void RichEditFrame::setBackgroundColor(uint32 color) {
  SendMessage(hWnd, EM_SETBKGNDCOLOR, 0, color);
}
void RichEditFrame::setRichText(std::string const& text) {
  EditStreamCookie cookie(text);
  EDITSTREAM es;
  es.dwCookie = (DWORD_PTR) &cookie;
  es.dwError = 0;
  es.pfnCallback = StreamCallback;
  SendMessage(hWnd, EM_EXLIMITTEXT, 0, (text.length() < 32768 ? 32768 : text.length() + 1));
  SendMessage(hWnd, EM_STREAMIN, SF_RTF, (uint32) &es);
}

/////////////////////////////////

SliderFrame::SliderFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(TRACKBAR_CLASS, "", style | WS_CHILD | WS_TABSTOP, 0);
  setFont(FontSys::getSysFont());
  setId(id);
}

void SliderFrame::setPos(int pos) {
  SendMessage(hWnd, TBM_SETPOS, TRUE, pos);
}
void SliderFrame::setRange(int minValue, int maxValue) {
  SendMessage(hWnd, TBM_SETRANGEMIN, TRUE, minValue);
  SendMessage(hWnd, TBM_SETRANGEMAX, TRUE, maxValue);
}
void SliderFrame::setLineSize(int size) {
  SendMessage(hWnd, TBM_SETLINESIZE, 0, size);
}
void SliderFrame::setPageSize(int size) {
  SendMessage(hWnd, TBM_SETPAGESIZE, 0, size);
}
void SliderFrame::setTicFreq(int freq) {
  SendMessage(hWnd, TBM_SETTICFREQ, freq, 0);
}
int SliderFrame::getPos() {
  return SendMessage(hWnd, TBM_GETPOS, 0, 0);
}

/////////////////////////////////////////

ProgressFrame::ProgressFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(PROGRESS_CLASS, "", style | WS_CHILD | WS_TABSTOP, 0);
  setFont(FontSys::getSysFont());
  setId(id);
}

void ProgressFrame::setPos(int pos) {
  SendMessage(hWnd, PBM_SETPOS, pos, 0);
}
void ProgressFrame::setRange(int minValue, int maxValue) {
  SendMessage(hWnd, PBM_SETRANGE32, minValue, maxValue);
}

/////////////////////////////////////////

UpDownFrame::UpDownFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(UPDOWN_CLASS, "", WS_CHILD | style, 0);
  setId(id);
}

/////////////////////////////////////////

TabFrame::TabFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(WC_TABCONTROL, "", WS_CHILD | style | WS_CLIPSIBLINGS, 0);
  setFont(FontSys::getSysFont());
  setId(id);
}
Frame* TabFrame::addTab(std::string const& text, Frame* frame) {
  if (!frame) frame = new Frame(this);
  size_t pos = tabs.size();
  tabs.push_back(frame);

  TCITEM item;
  memset(&item, 0, sizeof item);
  item.mask = TCIF_TEXT;
  item.pszText = const_cast<char*>(&text[0]);
  TabCtrl_InsertItem(hWnd, pos, &item);
  frame->show(pos == TabCtrl_GetCurSel(hWnd));

  RECT rc;
  GetClientRect(hWnd, &rc);
  int prevWidth = rc.right;
  int prevHeight = rc.bottom;
  TabCtrl_AdjustRect(hWnd, FALSE, &rc);
  frame->setPoint(PT_TOPLEFT, rc.left, rc.top);
  frame->setPoint(PT_BOTTOMRIGHT, rc.right - prevWidth, rc.bottom - prevHeight);

  return frame;
}
void TabFrame::clear() {
  for (Frame* tab : tabs) tab->hide();
  tabs.clear();
  TabCtrl_DeleteAllItems(hWnd);
}

LRESULT TabFrame::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  if (message == WM_NOTIFY) {
    NMHDR* hdr = (NMHDR*) lParam;
    if (hdr->hwndFrom == hWnd && hdr->code == TCN_SELCHANGE) {
      int sel = TabCtrl_GetCurSel(hWnd);
      for (size_t i = 0; i < tabs.size(); i++) {
        if (i != sel) tabs[i]->hide();
      }
      tabs[sel]->show();
      return 0;
    }
  }
  return M_UNHANDLED;
}
void TabFrame::setCurSel(int sel) {
  TabCtrl_SetCurSel(hWnd, sel);
  for (size_t i = 0; i < tabs.size(); i++) {
    if (i != sel) tabs[i]->hide();
  }
  tabs[sel]->show();
}

////////////////////////////////////

LRESULT ColorFrame::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  static COLORREF custColors[16] = {
    0x000000, 0x800000, 0x008000, 0x808000, 0x000080, 0x800080, 0x008080, 0xC0C0C0,
    0x808080, 0xFF0000, 0x00FF00, 0xFFFF00, 0x0000FF, 0xFF00FF, 0x00FFFF, 0xFFFFFF,
  };
  switch (message) {
  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hDC = BeginPaint(hWnd, &ps);
      std::string text = getText();
      if (!text.empty()) {
        RECT rc;
        GetClientRect(hWnd, &rc);
        SetBkColor(hDC, color);
        SetTextColor(hDC, color ^ 0xFFFFFF);
        SelectObject(hDC, FontSys::getSysFont());
        DrawText(hDC, text.c_str(), text.length(), &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
      }
      EndPaint(hWnd, &ps);
    }
    break;
  case WM_ERASEBKGND:
    {
      HDC hDC = (HDC) wParam;
      RECT rc;
      GetClientRect(hWnd, &rc);
      SetBkColor(hDC, color);
      ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    }
    return TRUE;
  case WM_LBUTTONDOWN:
    SetCapture(hWnd);
    break;
  case WM_LBUTTONUP:
    if (GetCapture() == hWnd) {
      ReleaseCapture();

      CHOOSECOLOR cc;
      memset(&cc, 0, sizeof cc);
      cc.lStructSize = sizeof cc;
      cc.hwndOwner = GetParent(hWnd);
      cc.rgbResult = color;
      cc.Flags = CC_FULLOPEN | CC_RGBINIT;
      cc.lpCustColors = custColors;
      if (ChooseColor(&cc)) {
        setColor(cc.rgbResult);
        notify(WM_COMMAND, MAKELONG(id(), BN_CLICKED), (LPARAM) hWnd);
      }
    }
    break;
  default:
    return M_UNHANDLED;
  }
  return 0;
}
ColorFrame::ColorFrame(uint32 clr, Frame* parent, uint32 id)
  : WindowFrame(parent)
{
  color = clr;
  create("", WS_CHILD, WS_EX_CLIENTEDGE);
  setId(id);
}
ColorFrame::~ColorFrame()
{
}

////////////////////////////////////

TreeViewFrame::TreeViewFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(WC_TREEVIEW, "", style | WS_CHILD | WS_VISIBLE, WS_EX_CLIENTEDGE);
  setId(id);
  setFont(FontSys::getSysFont());
}
void TreeViewFrame::setItemText(HTREEITEM item, std::string const& text) {
  TVITEM tvi;
  memset(&tvi, 0, sizeof tvi);
  tvi.hItem = item;
  tvi.mask = TVIF_TEXT;
  tvi.pszText = const_cast<char*>(&text[0]);
  TreeView_SetItem(hWnd, &tvi);
}
LPARAM TreeViewFrame::getItemData(HTREEITEM item) {
  TVITEM tvi;
  memset(&tvi, 0, sizeof tvi);
  tvi.hItem = item;
  tvi.mask = TVIF_PARAM;
  TreeView_GetItem(hWnd, &tvi);
  return tvi.lParam;
}

////////////////////////////////////

DateTimeFrame::DateTimeFrame(Frame* parent, int id, int style)
  : WindowFrame(parent)
{
  create(DATETIMEPICK_CLASS, "", style | WS_CHILD | WS_VISIBLE, 0);
  setId(id);
  setFont(FontSys::getSysFont());
}
void DateTimeFrame::setFormat(char const* fmt)
{
  DateTime_SetFormat(hWnd, fmt);
}

bool DateTimeFrame::isDateSet() const {
  SYSTEMTIME st;
  return (DateTime_GetSystemtime(hWnd, &st) == GDT_VALID);
}
uint64 DateTimeFrame::getDate() const {
  SYSTEMTIME st;
  if (DateTime_GetSystemtime(hWnd, &st) != GDT_VALID) return 0;
  FILETIME ft;
  SystemTimeToFileTime(&st, &ft);
  uint64 result = uint64(ft.dwLowDateTime) | (uint64(ft.dwHighDateTime) << 32);
  result = result / 10000000ULL - 11644473600ULL;
  return result;
}
void DateTimeFrame::setNoDate() {
  DateTime_SetSystemtime(hWnd, GDT_NONE, NULL);
}
void DateTimeFrame::setDate(uint64 date) {
  date = (date + 11644473600ULL) * 10000000ULL;
  FILETIME ft;
  ft.dwLowDateTime = uint32(date);
  ft.dwHighDateTime = uint32(date >> 32);
  SYSTEMTIME st;
  FileTimeToSystemTime(&ft, &st);
  DateTime_SetSystemtime(hWnd, GDT_VALID, &st);
}
