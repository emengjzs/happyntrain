#pragma once

#include <atomic>
#include <cstring>
#include <memory>
#include <functional>

#include "logging.h"

namespace happyntrain {

// -------------------------
// memory
// -------------------------
template <class T>
using Ref = std::shared_ptr<T>;

template <class T, typename... Vars>
inline Ref<T> newInstance(Vars&&... args) {
  return std::make_shared<T>(std::forward<Vars>(args)...);
}

// template <class T>
// using Ptr = std::unique_ptr<T>;

template <class T, class Deleter = std::default_delete<T>>
using Ptr = std::unique_ptr<T, Deleter>;

template <class T>
using Sharable = std::enable_shared_from_this<T>;

template <class T>
using WeakRef = std::weak_ptr<T>;

// -------------------------
// Effective C++
// -------------------------
// macro
#define DISABLE_COPY(Class)                \
 private:                                  \
  Class(const Class&) = delete;            \
  Class(Class&&) = delete;                 \
  Class& operator=(const Class&) = delete; \
  Class& operator=(const Class&&) = delete;

// zero-cost class
class NoCopy {
 public:
  NoCopy(){};
  DISABLE_COPY(NoCopy);
};


// --------------------------
// Fail checking
// --------------------------
#define EXPECT(expression, message, ...)                                     \
  do {                                                                       \
    if (!(expression)) {                                                     \
      ERROR("Fail: " message, ##__VA_ARGS__);                                \
      ERROR("File: %s, Line: %d, Fail Expression: (%s)", __FILE__, __LINE__, \
            #expression);                                                    \
    }                                                                        \
  } while (0);

#define EXIT_IF(expression, message, ...)                                \
  do {                                                                   \
    if ((expression)) {                                                  \
      ERROR("Fail: " message, ##__VA_ARGS__);                            \
      ERROR("File: %s, Line: %d, Unexpected Expression: (%s)", __FILE__, \
            __LINE__, #expression);                                      \
      exit(1);                                                           \
    }                                                                    \
  } while (0);

//------------------------
// c struct
//-------------------------
template <typename T>
inline T c_struct_init() {
  T t;
  memset(&t, 0, sizeof(t));
  return t;
}

#define C_STRUCT(type, name) struct type name = c_struct_init<struct type>()

// -----------------------
// atomic counter
// -----------------------
template <class T = uint64_t>
class SequenceCreator {
  std::atomic<T> id_;

 public:
  explicit SequenceCreator(T id) : id_(id) {}
  ~SequenceCreator() {}
  SequenceCreator() : SequenceCreator(0) {}

  T GetNextId() { return ++id_; }
  T operator()() { return GetNextId(); }
};

// Interface : () => void
using Runnable = std::function<void()>;

// end happyntrain
}
