#pragma once

#include "framewnd.h"
#include "controlframes.h"

class Scrollable : public WindowFrame {
public:
  Scrollable(Frame* parent)
    : WindowFrame(parent)
    , scrollPos(0)
    , scrollAccum(0)
  {}

protected:
  int scrollPos;
  int scrollAccum;
  void scroll(int pos);
  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override;
};

class SearchList : public Scrollable {
public:
  SearchList(Frame* parent, int id = 0);

  void clear();
  void insert(uint32 id, std::string const& text);
  void search(std::string const& query);
  void update();

  void sort();

  int selected() const {
    return cursel;
  }
  uint32 itemdata(int index) const {
    return display[index]->id;
  }

private:
  struct Item {
    uint32 id;
    istring text;
    Item() {}
    Item(uint32 id, std::string const& text) : id(id), text(text) {}
  };
  friend bool operator <(Item const& lhs, Item const& rhs) {
    return lhs.text < rhs.text;
  }
  std::vector<Item> items;
  std::vector<Item*> display;
  std::string query;

  int cursel;
  int itemHeight;

  void resize();
  void render(HDC hDC);
  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override;
};

#define WM_OPTIONCHANGE     (WM_USER+100)
class OptionList : public Scrollable {
public:
  OptionList(Frame* parent, int id = 0);

  void clear() {
    items.clear();
    options.clear();
    choices.clear();
  }
  void addItem(std::string const& text, int choice = -1) {
    items.push_back(text);
    choices.push_back(choice);
  }
  void addOption(std::string const& text);
  void update();
  int getOption(int item) const {
    return choices[item];
  }

private:
  std::vector<std::string> items;
  std::vector<std::string> options;
  std::vector<int> choices;

  class Editor;
  friend class Editor;
  Editor* editor;
  int cursel;

  int itemHeight;
  void render(HDC hDC);
  LRESULT onMessage(uint32 message, WPARAM wParam, LPARAM lParam) override;
};
