#pragma once

#include <atomic>

#include "util.h"

namespace happyntrain {

const int kMaxSelectEvents = 2000;

template <typename SelectorImpt>
class Selector : NoCopy {
  int64_t _id;

 public:
  Selector() : _id(GetNewId()) {}
  ~Selector() {}

  inline void AddChannel() { static_cast<SelectorImpt*>(this)->AddChannel(); }

  inline void RemoveChannel() {
    static_cast<SelectorImpt*>(this)->RemoveChannel();
  }

  inline void UpdateChannel() {
    static_cast<SelectorImpt*>(this)->UpdateChannel();
  }

 private:
  static int64_t GetNewId() {
    static std::atomic<int64_t> id(0);
    return id++;
  }
};

#if defined(__linux__)
#include "epoll-selector.h"
#endif

// end happyntrain
}