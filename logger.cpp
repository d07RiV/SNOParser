#include "logger.h"
#define NOMINMAX
#include <windows.h>
#include "common.h"
#include <algorithm>
#include <list>

Logger Logger::instance;

struct Logger::Task {
  HANDLE handle;
  Task* parent;
  COORD pos;
  std::string name;
  std::string line;
  int count;
  int index;
  int height;
  uint32 time;
  std::list<Task> sub;
  typedef std::list<Task>::iterator Iter;

  Task();
  Task(Task* parent, int count, std::string const& name);
  ~Task();

  void erase();
  void draw();
  void write(std::string const& text);
  void move(int y);
  void item(char const* text);
  void close();
  Iter find(Task* task);
  void update(Iter from);
  void shift(Iter from);
  void rupdate(Iter from);
  void rshift(Iter from);
  void remove(Task* task);
  Task* insert(int count, std::string const& name);
};

static Logger::Task _root;
Logger::Task* Logger::root = &_root;
Logger::Task* Logger::top = nullptr;

void Logger::Task::move(int y) {
  erase();
  pos.Y = y;
  draw();
}
void Logger::Task::item(char const* text) {
  ++index;
  if (GetTickCount() > time + 200) {
    write(name.empty() ? text : fmtstring("%s: %s", name.c_str(), text));
  }
}
void Logger::Task::erase() {
  std::string buf(8 + line.length(), ' ');
  SetConsoleCursorPosition(handle, pos);
  WriteConsole(handle, buf.c_str(), buf.length(), nullptr, nullptr);
  SetConsoleCursorPosition(handle, { 0, pos.Y });
}
void Logger::Task::draw() {
  std::string prev = line;
  line.clear();
  write(prev);
}
void Logger::Task::write(std::string const& text) {
  std::string buf;
  if (count < 0) {
    buf = "[ Log ] ";
  } else if (!count || index >= count) {
    buf = "[ Done] ";
  } else {
    buf = fmtstring("[%4.1lf%%] ", 100.0 * std::max(index, 0) / count);
  }
  SetConsoleCursorPosition(handle, pos);
  SetConsoleTextAttribute(handle, 2);
  WriteConsole(handle, buf.c_str(), buf.length(), nullptr, nullptr);
  SetConsoleTextAttribute(handle, 7);
  WriteConsole(handle, text.c_str(), text.length(), nullptr, nullptr);
  if (line.length() > text.length()) {
    buf.assign(line.length() - text.length(), ' ');
    WriteConsole(handle, buf.c_str(), buf.length(), nullptr, nullptr);
    SetConsoleCursorPosition(handle, { pos.X + 8 + text.length(), pos.Y });
  }
  line = text;
  time = GetTickCount();
}
void Logger::Task::close() {
  index = count;
  write(name);
  if (sub.size()) {
    for (auto& x : sub) {
      x.erase();
    }
    sub.clear();
    int delta = height - 1;
    for (Task* cur = this; cur; cur = cur->parent) {
      cur->height -= delta;
    }
    shift(sub.end());
  }
}
Logger::Task::Iter Logger::Task::find(Task* task) {
  for (Iter it = sub.begin(); it != sub.end(); ++it) {
    if (&*it == task) return it;
  }
  return sub.end();
}
void Logger::Task::update(Iter from) {
  Task* prev = nullptr;
  if (from != sub.begin()) {
    prev = &*std::prev(from);
  }
  for (; from != sub.end(); ++from) {
    from->move(prev ? prev->pos.Y + prev->height : pos.Y + 1);
    from->update(from->sub.begin());
    prev = &*from;
  }
}
void Logger::Task::shift(Iter from) {
  update(from);
  if (parent) {
    Iter self = parent->find(this);
    if (self != parent->sub.end()) {
      parent->shift(std::next(self));
    }
  }
}
void Logger::Task::rupdate(Iter from) {
  if (from == sub.end()) return;
  Task* to = &*from;
  int prev = pos.Y + height;
  for (auto it = sub.rbegin(); it != sub.rend(); ++it) {
    it->rupdate(it->sub.begin());
    it->move(prev - it->height);
    prev = it->pos.Y;
    if (&*it == to) break;
  }
}
void Logger::Task::rshift(Iter from) {
  if (parent) {
    Iter self = parent->find(this);
    if (self != parent->sub.end()) {
      parent->rshift(std::next(self));
    }
  }
  rupdate(from);
}
void Logger::Task::remove(Task* task) {
  Iter pos = find(task);
  if (pos == sub.end()) return;
  for (Task* cur = this; cur; cur = cur->parent) {
    cur->height -= task->height;
  }
  task->erase();
  Iter next = std::next(pos);
  sub.erase(pos);
  shift(next);
}
Logger::Task* Logger::Task::insert(int count, std::string const& name) {
  for (Task* cur = this; cur; cur = cur->parent) {
    ++cur->height;
  }
  rshift(sub.end());
  sub.emplace_back(this, count, name);
  return &sub.back();
}

Logger::Task::Task()
  : handle(GetStdHandle(STD_OUTPUT_HANDLE))
  , parent(nullptr)
  , count(0)
  , index(0)
  , time(0)
  , height(1)
{
  CONSOLE_SCREEN_BUFFER_INFO info;
  GetConsoleScreenBufferInfo(handle, &info);
  pos.X = -2;
  pos.Y = info.dwCursorPosition.Y - 1;
}
Logger::Task::Task(Task* parent, int count, std::string const& name)
  : handle(GetStdHandle(STD_OUTPUT_HANDLE))
  , parent(parent)
  , count(count)
  , index(-1)
  , time(0)
  , name(name)
  , height(1)
{
  pos.X = parent->pos.X + 2;
  if (parent->sub.empty()) {
    pos.Y = parent->pos.Y + 1;
  } else {
    Task* last = &parent->sub.back();
    pos.Y = last->pos.Y + last->height;
  }
  write(name);
}
Logger::Task::~Task() {
  //if (parent) {
  //  erase();
  //}
}

void* Logger::begin(size_t count, char const* name, void* task_) {
  Task* task = (task_ ? (Task*)task_ : top);
  if (!task) task = root;
  return top = task->insert(count, std::string(name ? name : ""));
}
void Logger::item(char const* name, void* task_) {
  Task* task = (task_ ? (Task*)task_ : top);
  task->item(name);
}
void Logger::end(bool pop, void* task_) {
  Task* task = (task_ ? (Task*)task_ : top);
  if (top == task) top = task->parent;
  if (pop) {
    task->parent->remove(task);
  } else {
    task->close();
  }
}

void Logger::log(char const* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  std::string text = varfmtstring(fmt, ap);
  root->insert(-1, text);
  va_end(ap);
  if (!instance.logfile) {
    instance.logfile = new File("log.txt", "at");
    instance.logfile->printf("============\n");
  }
  instance.logfile->printf("%s\n", text.c_str());
}
