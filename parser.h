#pragma once

#include "file.h"
#include "json.h"
#include "path.h"
#include "logger.h"

uint32 HashName(std::string const& str);
uint32 HashNameLower(std::string const& str);

class SnoParser {
protected:
  std::string name_;
  std::vector<uint8> data_;
public:
  std::string const& name() const {
    return name_;
  }
  uint32 size() const {
    return data_.size();
  }
  bool contains(uint32 offset, uint32 size) const {
    return size && offset <= 0xFFFFFFFF - size && offset + size <= data_.size();
  }
  uint8* data(uint32 offset) {
    return &data_[offset];
  }
  uint8 const* data(uint32 offset) const {
    return &data_[offset];
  }

  static __declspec(thread) SnoParser* context;
};

template<class T>
void DumpFile(File* from, File* to) {
  json::WriterVisitor writer(to);
  writer.setIndent(2);
  ParseFile(from, &writer);
  writer.onEnd();
}

class SnoLoader;

template<class T>
class SnoFile : public SnoParser {
  typename T::Type* object_;
public:
  SnoFile(File& file, std::string const& name = "")
    : object_(nullptr)
  {
    name_ = name;
    if (!file) return;
    data_.resize(file.size() - 16);
    file.seek(16);
    if (file.read(&data_[0], data_.size())) {
      SnoParser::context = this;
      object_ = new(&data_[0]) T::Type;
      SnoParser::context = nullptr;
    }
  }
  SnoFile(std::string const& name, SnoLoader* loader = SnoLoader::default)
    : SnoFile(loader->load<T>(name), name)
  {}

  operator bool() const {
    return object_ != nullptr;
  }
  operator typename T::Type*() {
    return object_;
  }
  operator typename T::Type const*() const {
    return object_;
  }

  typename T::Type* operator->() {
    return object_;
  }
  typename T::Type const* operator->() const {
    return object_;
  }

  void dump(File& to) {
    json::WriterVisitor writer(to);
    writer.setIndent(2);
    if (object_) {
      object_->serialize(&writer);
    } else {
      writer.onNull();
    }
    writer.onEnd();
  }
};

class SnoLoader {
protected:
  virtual std::vector<std::string> listdir(char const* type, char const* ext) = 0;
  virtual File loadfile(char const* type, char const* name, char const* ext) = 0;
public:
  virtual uint32 hash() const = 0;

  template<class T>
  std::vector<std::string> list() {
    return listdir(T::type(), T::ext());
  }

  template<class T>
  File load(std::string const& name) {
    return loadfile(T::type(), name.c_str(), T::ext());
  }

  template<class T>
  class SnoAll {
  public:
    class Iterator {
    public:
      ~Iterator() {
        delete file_;
      }

      bool operator==(Iterator const& rhs) {
        return iter_ == rhs.iter_;
      }
      bool operator!=(Iterator const& rhs) {
        return iter_ != rhs.iter_;
      }
      Iterator& operator++() {
        delete file_;
        file_ = nullptr;
        ++iter_;
        return *this;
      }

      SnoFile<T>& operator*() {
        return *parse();
      }
      SnoFile<T>* operator->() {
        return parse();
      }
    private:
      friend class SnoAll<T>;
      Iterator(SnoLoader* loader, std::vector<std::string>::const_iterator const& iter)
        : loader_(loader)
        , iter_(iter)
        , file_(nullptr)
      {}
      SnoFile<T>* parse() {
        if (file_ == nullptr) {
          Logger::item(iter_->c_str());
          file_ = new SnoFile<T>(*iter_, loader_);
        }
        return file_;
      }

      SnoLoader* loader_;
      std::vector<std::string>::const_iterator iter_;
      SnoFile<T>* file_;
    };

    Iterator begin() const {
      return Iterator(loader_, list_.begin());
    }
    Iterator end() const {
      return Iterator(loader_, list_.end());
    }
    ~SnoAll() {
      Logger::end();
    }
  private:
    friend class SnoLoader;
    SnoAll(SnoLoader* loader)
      : loader_(loader)
      , list_(loader->list<T>())
    {
      Logger::begin(list_.size(), fmtstring("Parsing %s", T::type()).c_str());
    }
    SnoLoader* loader_;
    std::vector<std::string> list_;
  };
  template<class T>
  SnoAll<T> all() {
    return SnoAll<T>(this);
  }

  template<class T>
  class SnoAllJson {
  public:
    class Iterator {
    public:
      bool operator==(Iterator const& rhs) {
        return iter_ == rhs.iter_;
      }
      bool operator!=(Iterator const& rhs) {
        return iter_ != rhs.iter_;
      }
      Iterator& operator++() {
        valid_ = false;
        ++iter_;
        return *this;
      }

      json::Value const& operator*() {
        return *parse();
      }
      json::Value const* operator->() {
        return parse();
      }
    private:
      friend class SnoAllJson<T>;
      Iterator(SnoLoader* loader, std::vector<std::string>::const_iterator const& iter)
        : loader_(loader)
        , iter_(iter)
        , valid_(false)
      {}
      json::Value* parse() {
        if (!valid_) {
          value_.clear();
          Logger::item(iter_->c_str());
          File src = loader_->load<T>(*iter_);
          if (src) {
            json::BuilderVisitor builder(value_);
            T::parse(src, &builder);
            builder.onEnd();
          }
          valid_ = true;
        }
        return &value_;
      }

      SnoLoader* loader_;
      std::vector<std::string>::const_iterator iter_;
      bool valid_;
      json::Value value_;
    };

    Iterator begin() const {
      return Iterator(loader_, list_.begin());
    }
    Iterator end() const {
      return Iterator(loader_, list_.end());
    }
    ~SnoAllJson() {
      Logger::end();
    }
  private:
    friend class SnoLoader;
    SnoAllJson(SnoLoader* loader)
      : loader_(loader)
      , list_(loader->list<T>())
    {
      Logger::begin(list_.size(), fmtstring("Parsing %s", T::type()).c_str());
    }
    SnoLoader* loader_;
    std::vector<std::string> list_;
  };
  template<class T>
  SnoAllJson<T> json() {
    return SnoAllJson<T>(this);
  }

  template<class T>
  void dump(std::string const& name) {
    File src = load<T>(name);
    if (!src) return;
    File dst(path::work() / T::type() / name + ".txt", "wt");
    if (!dst) return;
    json::WriterVisitor writer(dst);
    writer.setIndent(2);
    T::parse(src, &writer);
    writer.onEnd();
  }
  template<class T>
  void dump() {
    for (auto& name : Logger::Loop(list<T>(), fmtstring("Dumping %s", T::type()).c_str())) {
      dump<T>(name);
    }
  }

  template<class T>
  static std::vector<std::string> List() {
    return default->list<T>();
  }
  template<class T>
  static File Load(std::string const& name) {
    return default->load<T>(name);
  }
  template<class T>
  static SnoAll<T> All() {
    return default->all<T>();
  }
  template<class T>
  static SnoAll<T> Json() {
    return default->json<T>();
  }
  template<class T>
  static void Dump(std::string const& name) {
    default->dump<T>(name);
  }
  template<class T>
  static void Dump() {
    default->dump<T>();
  }

  static SnoLoader* default;
};

class SnoSysLoader : public SnoLoader {
protected:
  std::string dir_;
  std::vector<std::string> listdir(char const* type, char const* ext);
  File loadfile(char const* type, char const* name, char const* ext);
public:
  SnoSysLoader(std::string dir = "");
  uint32 hash() const {
    return HashNameLower(dir_);
  }
  static SnoSysLoader default;
};

class SnoCascLoader : public SnoLoader {
protected:
  uint32 hash_;
  std::string lang_;
  void* handle_;
  std::vector<std::string> listdir(char const* type, char const* ext);
  File loadfile(char const* type, char const* name, char const* ext);
public:
  uint32 hash() const {
    return hash_;
  }
  SnoCascLoader(std::string dir, std::string lang = "");
  ~SnoCascLoader();
};
