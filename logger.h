#pragma once

#include <string>
#include <vector>

class Logger {
public:
  struct Task;
  static Task* root;
private:
  friend struct Task;
  static Task* top;
public:
  static void* begin(size_t count, char const* name = nullptr, void* task = nullptr);
  static void item(char const* name, void* task = nullptr);
  static void end(bool pop = false, void* task = nullptr);

  class Loop {
  public:
    typedef std::vector<std::string> List;
    class Iterator {
    public:
      Iterator& operator++() {
        if (++iter_ != list_.end()) {
          Logger::item(iter_->c_str(), task_);
        }
        return *this;
      }
      bool operator!=(Iterator const& rhs) const {
        return iter_ != rhs.iter_;
      }
      std::string const& operator*() {
        return *iter_;
      }
    private:
      friend class Loop;
      Iterator(List const& list, List::const_iterator iter, void* task)
        : list_(list)
        , iter_(iter)
        , task_(task)
      {
        if (iter_ != list.end()) {
          Logger::item(iter_->c_str(), task_);
        }
      }
      List const& list_;
      List::const_iterator iter_;
      void* task_;
    };

    Loop(List&& list, char const* task = nullptr)
      : list_(list)
    {
      task_ = Logger::begin(list.size(), task);
    }
    ~Loop() {
      Logger::end(false, task_);
    }
    Iterator begin() const {
      return Iterator(list_, list_.begin(), task_);
    }
    Iterator end() const {
      return Iterator(list_, list_.end(), task_);
    }
  private:
    List list_;
    void* task_;
  };
};
