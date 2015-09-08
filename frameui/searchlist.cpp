#include "searchlist.h"
#include "fontsys.h"
#include <windowsx.h>
#include <algorithm>

////////////////////////////////////////

LRESULT Scrollable::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  SCROLLINFO si;
  int step;
  switch (message) {
  case WM_SIZE:
    memset(&si, 0, sizeof si);
    si.cbSize = sizeof si;
    si.fMask = SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nPage = HIWORD(lParam);
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
    scroll(scrollPos);
    return 0;
  case WM_VSCROLL:
    memset(&si, 0, sizeof si);
    si.cbSize = sizeof si;
    si.fMask = SIF_ALL;
    GetScrollInfo(hWnd, SB_VERT, &si);
    switch (LOWORD(wParam)) {
    case SB_TOP:
      si.nPos = si.nMin;
      break;
    case SB_BOTTOM:
      si.nPos = si.nMax;
      break;
    case SB_LINEUP:
      si.nPos -= 16;
      break;
    case SB_LINEDOWN:
      si.nPos += 16;
      break;
    case SB_PAGEUP:
      si.nPos -= si.nPage;
      break;
    case SB_PAGEDOWN:
      si.nPos += si.nPage;
      break;
    case SB_THUMBTRACK:
      si.nPos = si.nTrackPos;
      break;
    }
    scroll(si.nPos);
    SetFocus(hWnd);
    return 0;
  case WM_MOUSEWHEEL:
    SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &step, 0);
    if (step < 0) step = 3;
    scrollAccum += GET_WHEEL_DELTA_WPARAM(wParam) * step * 16;
    scroll(scrollPos - scrollAccum / WHEEL_DELTA);
    scrollAccum %= WHEEL_DELTA;
    return 0;
  }
  return M_UNHANDLED;
}

void Scrollable::scroll(int pos) {
  SCROLLINFO si;
  memset(&si, 0, sizeof si);
  si.cbSize = sizeof si;
  si.fMask = SIF_RANGE | SIF_PAGE;
  GetScrollInfo(hWnd, SB_VERT, &si);
  if (pos < si.nMin) pos = si.nMin;
  if (pos > si.nMax - si.nPage + 1) pos = si.nMax - si.nPage + 1;
  si.fMask = SIF_POS;
  if (pos != scrollPos) {
    si.nPos = pos;
    SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

    int deltaY = scrollPos - pos;
    scrollPos = pos;
    RECT rc;
    GetClientRect(hWnd, &rc);
    ScrollWindowEx(hWnd, 0, deltaY, &rc, &rc, NULL, NULL, SW_ERASE | SW_INVALIDATE);
  }
}

////////////////////////////////////////

SearchList::SearchList(Frame* parent, int id)
  : Scrollable(parent)
  , cursel(-1)
{
  itemHeight = FontSys::getTextSize(FontSys::getSysFont(), "123").cy + 3;

  if (WNDCLASSEX* wcx = createclass("SearchListClass")) {
    wcx->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcx->hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(wcx);
  }
  create("", WS_CHILD | WS_VSCROLL, WS_EX_CLIENTEDGE);
  setId(id);
}

void SearchList::clear() {
  items.clear();
  display.clear();
  resize();
}
void SearchList::insert(uint32 id, std::string const& text) {
  items.emplace_back(id, text);
}
void SearchList::insertEx(uint32 id, std::string const& text) {
  itemsEx.emplace_back(id, text);
}

static bool contains(std::vector<size_t> const& kmp, std::string const& query, std::string const& text) {
  if (kmp.empty()) return true;
  size_t cur = 0;
  for (size_t i = 0; i < text.length(); ++i) {
    while (cur && query[cur] != tolower((uint8) text[i])) {
      cur = kmp[cur - 1];
    }
    cur += (query[cur] == tolower((uint8)text[i]));
    if (cur == query.length()) return true;
  }
  return false;
}

void SearchList::search(std::string const& query_) {
  query = strlower(query_);
  std::vector<size_t> kmp(query.length(), 0);
  for (size_t i = 1; i < query.length(); ++i) {
    kmp[i] = kmp[i - 1];
    while (kmp[i] && query[kmp[i]] != query[i]) {
      kmp[i] = kmp[kmp[i] - 1];
    }
    kmp[i] += (query[kmp[i]] == query[i]);
  }
  Item* sel = nullptr;
  if (cursel >= 0) sel = display[cursel];
  display.clear();
  cursel = -1;
  scrollPos = 0;
  scrollAccum = 0;
  auto* list = &items;
  if (query.length() && itemsEx.size()) {
    list = &itemsEx;
  }
  for (auto& item : *list) {
    if (contains(kmp, query, item.text)) {
      if (&item == sel) cursel = display.size();
      display.push_back(&item);
    }
  }
  resize();
}

void SearchList::sort() {
  std::sort(items.begin(), items.end());
}
void SearchList::sortEx() {
  std::sort(itemsEx.begin(), itemsEx.end());
}
void SearchList::update() {
  search(query);
}
void SearchList::resize() {
  RECT rc;
  GetClientRect(hWnd, &rc);
  SCROLLINFO si;
  memset(&si, 0, sizeof si);
  si.cbSize = sizeof si;
  si.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
  si.nMin = 0;
  si.nMax = display.size() * itemHeight;
  si.nPage = rc.bottom;
  SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
  scroll(scrollPos);

  InvalidateRect(hWnd, NULL, TRUE);
}

void SearchList::render(HDC hDC) {
  RECT rc;
  GetClientRect(hWnd, &rc);
  SelectObject(hDC, FontSys::getSysFont());
  int top = (rc.top + scrollPos) / itemHeight;
  int bottom = (rc.bottom + scrollPos + itemHeight) / itemHeight;
  if (top < 0) top = 0;
  if (bottom > display.size()) bottom = display.size();

  uint32 fgHighlight = GetSysColor(COLOR_HIGHLIGHTTEXT);
  uint32 bgHighlight = GetSysColor(COLOR_HIGHLIGHT);
  uint32 fgNormal = 0x000000;
  uint32 bgNormal = 0xFFFFFF;

  for (int index = top; index < bottom; ++index) {
    Item* item = display[index];
    if (index == cursel) {
      SetTextColor(hDC, fgHighlight);
      SetBkColor(hDC, bgHighlight);
    } else {
      SetTextColor(hDC, fgNormal);
      SetBkColor(hDC, bgNormal);
    }
    RECT irc = rc;
    irc.top = index * itemHeight - scrollPos;
    irc.bottom = irc.top + itemHeight;
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &irc, NULL, 0, NULL);
    DrawText(hDC, item->text.c_str(), item->text.length(), &irc, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_VCENTER);
  }
  rc.top = bottom * itemHeight - scrollPos;
  if (rc.top < rc.bottom) {
    SetBkColor(hDC, bgNormal);
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
  }
}

LRESULT SearchList::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  int x, y, index;
  PAINTSTRUCT ps;
  HDC hDC;
  switch (message) {
  case WM_PAINT:
    hDC = BeginPaint(hWnd, &ps);
    render(hDC);
    EndPaint(hWnd, &ps);
    return 0;
  case WM_LBUTTONDOWN:
    SetFocus(hWnd);
    x = GET_X_LPARAM(lParam);
    y = GET_Y_LPARAM(lParam);
    index = (y + scrollPos) / itemHeight;
    if (index < 0 || index >= display.size()) {
      index = -1;
    }
    if (index != cursel) {
      cursel = index;
      notify(WM_COMMAND, MAKELONG(id(), BN_CLICKED), (LPARAM)hWnd);
      InvalidateRect(hWnd, NULL, TRUE);
    }
    return 0;
  }
  return Scrollable::onMessage(message, wParam, lParam);
}

///////////////////////////////////////////

class OptionList::Editor : public ComboFrame {
public:
  Editor(OptionList* parent, int id = 0)
    : ComboFrame(parent, id)
  {
  }

  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override {
    if (message == WM_KILLFOCUS || (message == WM_COMMAND && HIWORD(wParam) == CBN_CLOSEUP)) {
      hide();
      SetFocus(getOwner());
    }
    return M_UNHANDLED;
  }
};

OptionList::OptionList(Frame* parent, int id)
  : Scrollable(parent)
  , editor(nullptr)
  , cursel(-1)
{
  itemHeight = FontSys::getTextSize(FontSys::getSysFont(), "123").cy + 5;

  if (WNDCLASSEX* wcx = createclass("OptionListClass")) {
    wcx->hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcx->hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(wcx);
  }
  create("", WS_CHILD | WS_VSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE);
  setId(id);

  editor = new Editor(this);
  editor->addString("", -1);
}

void OptionList::addOption(std::string const& text) {
  if (editor) editor->addString(text, options.size());
  options.push_back(text);
}

void OptionList::update() {
  RECT rc;
  GetClientRect(hWnd, &rc);
  SCROLLINFO si;
  memset(&si, 0, sizeof si);
  si.cbSize = sizeof si;
  si.fMask = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
  si.nMin = 0;
  si.nMax = items.size() * itemHeight;
  si.nPage = rc.bottom;
  SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
  scroll(scrollPos);

  InvalidateRect(hWnd, NULL, TRUE);
}

void OptionList::render(HDC hDC) {
  RECT rc;
  GetClientRect(hWnd, &rc);
  SelectObject(hDC, FontSys::getSysFont());
  int top = (rc.top + scrollPos) / itemHeight;
  int bottom = (rc.bottom + scrollPos + itemHeight) / itemHeight;
  if (top < 0) top = 0;
  if (bottom > items.size()) bottom = items.size();

  uint32 fgNormal = 0x000000;
  uint32 bgNormal = 0xFFFFFF;
  uint32 bgLine = 0x808080;

  for (int index = top; index < bottom; ++index) {
    std::string item = items[index];
    std::string value;
    if (choices[index] >= 0) value = options[choices[index]];

    SetTextColor(hDC, fgNormal);
    SetBkColor(hDC, bgNormal);
    RECT irc = rc;
    irc.top = index * itemHeight - scrollPos;
    irc.bottom = irc.top + itemHeight;
    if (cursel == index && editor->visible()) {
      irc.right = rc.right / 2;
    }
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &irc, NULL, 0, NULL);
    irc.top += 1;
    irc.bottom -= 1;
    irc.left += 1;
    irc.right = rc.right / 2 - 2;
    DrawText(hDC, item.c_str(), item.length(), &irc, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_VCENTER);
    irc.left = irc.right + 3;
    irc.right = rc.right - 1;
    if (cursel != index || !editor->visible()) {
      DrawText(hDC, value.c_str(), value.length(), &irc, DT_SINGLELINE | DT_NOPREFIX | DT_LEFT | DT_VCENTER);
    }
    SetBkColor(hDC, bgLine);
    irc.right = rc.right / 2;
    irc.left = irc.right - 1;
    irc.top -= 1;
    irc.bottom += 1;
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &irc, NULL, 0, NULL);
    irc.left = rc.left;
    irc.right = rc.right;
    irc.top = irc.bottom - 1;
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &irc, NULL, 0, NULL);
  }
  rc.top = bottom * itemHeight - scrollPos;
  if (rc.top < rc.bottom) {
    SetBkColor(hDC, bgNormal);
    ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
  }
}

LRESULT OptionList::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
  int x, y, index;
  PAINTSTRUCT ps;
  HDC hDC;
  RECT rc;
  switch (message) {
  case WM_ERASEBKGND:
    return TRUE;
  case WM_PAINT:
    hDC = BeginPaint(hWnd, &ps);
    render(hDC);
    EndPaint(hWnd, &ps);
    return 0;
  case WM_SIZE:
    InvalidateRect(hWnd, NULL, TRUE);
    break;
  case WM_COMMAND:
    if (editor && cursel >= 0 && HIWORD(wParam) == CBN_SELCHANGE && (HWND) lParam == editor->getHandle()) {
      int sel = editor->getCurSel();
      choices[cursel] = (sel >= 0 ? editor->getItemData(sel) : -1);
      notify(WM_OPTIONCHANGE, id(), cursel);
    }
    return 0;
  case WM_LBUTTONDOWN:
    SetFocus(hWnd);
    x = GET_X_LPARAM(lParam);
    y = GET_Y_LPARAM(lParam);
    cursel = (y + scrollPos) / itemHeight;
    GetClientRect(hWnd, &rc);
    if (x < rc.right / 2) cursel = -1;
    if (cursel >= 0 && cursel < items.size() && editor) {
      editor->setPoint(PT_TOPLEFT, rc.right / 2 - 1, cursel * itemHeight - scrollPos);
      editor->setWidth(rc.right / 2 + 3);
      editor->show();
      SetFocus(editor->getHandle());
      ComboBox_SetCurSel(editor->getHandle(), choices[cursel] + 1);
      ComboBox_ShowDropdown(editor->getHandle(), TRUE);
    } else {
      cursel = -1;
    }
    return 0;
  }
  return Scrollable::onMessage(message, wParam, lParam);
}

///////////////////////////////////////////
//
//TestWindow::TestWindow() {
//  if (WNDCLASSEX* wcx = createclass("MainWndClass")) {
//    wcx->hbrBackground = HBRUSH(COLOR_BTNFACE + 1);
//    wcx->hCursor = LoadCursor(NULL, IDC_ARROW);
//    RegisterClassEx(wcx);
//  }
//  create(CW_USEDEFAULT, 0, 400, 700, "Test Window",
//    WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, 0);
//
//  query = new EditFrame(this, 100);
//  query->setPoint(PT_TOPLEFT, 0, 0);
//  query->setPoint(PT_TOPRIGHT, 0, 0);
//  query->setHeight(21);
//
//  list = new SearchList(this, 101);
//  list->setPoint(PT_TOPLEFT, query, PT_BOTTOMLEFT, 0, 4);
//  list->setPoint(PT_TOPRIGHT, query, PT_BOTTOMRIGHT, 0, 4);
//  list->setHeight(300);
//
//  list->insert(0, "fasdfasdf");
//  list->insert(1, "gasdfasdg");
//  list->insert(2, "sadfasdgq");
//  list->insert(3, "gqfeqwe");
//  list->insert(4, "gasdfawe");
//  list->insert(5, "asdfawe");
//  list->insert(6, "gasdfawe");
//  list->insert(7, "asdgawe");
//  list->insert(8, "gasdfawe");
//  list->insert(9, "badsfawe");
//  list->update();
//
//  opts = new OptionList(this, 102);
//  opts->setPoint(PT_TOPLEFT, list, PT_BOTTOMLEFT, 0, 4);
//  opts->setPoint(PT_BOTTOMRIGHT, 0, 0);
//
//  opts->addItem("Item 1", -1);
//  opts->addItem("Item 2", 0);
//  opts->addItem("Item 3", 1);
//  opts->addItem("Item 4", 2);
//  opts->addItem("Item 5", 3);
//  opts->addOption("Value 1");
//  opts->addOption("Value 2");
//  opts->addOption("Value 3");
//  opts->addOption("Value 4");
//  opts->update();
//}
//
//LRESULT TestWindow::onMessage(uint32 message, WPARAM wParam, LPARAM lParam) {
//  if (message == WM_COMMAND) {
//    int id = LOWORD(wParam);
//    if (id == 100) {
//      if (HIWORD(wParam) == EN_CHANGE) list->search(query->getText());
//    } else if (id == 101) {
//    }
//    return 0;
//  }
//  return M_UNHANDLED;
//}
//
//void TestWindow::loop() {
//  ShowWindow(hWnd, SW_SHOW);
//  while (hWnd) {
//    MSG msg;
//    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//      TranslateMessage(&msg);
//      DispatchMessage(&msg);
//      if (msg.message == WM_QUIT) {
//        DestroyWindow(hWnd);
//      }
//    }
//  }
//}
