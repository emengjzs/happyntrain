#pragma once

#include <atomic>

#include "util.h"

namespace happyntrain {

const int kMaxSelectEvents = 2000;

template <typename SelectorImpt>
class Selector : NoCopy {
  SelectorImpt _selector;
  int64_t _id;

 public:
  template <typename... Vars>
  Selector(Vars&&... args) : _id(GetNewId()), _selector(args...) {}
  ~Selector() {}

  inline void AddChannel() { _selector.AddChannel(); }
  inline void RemoveChannel() { _selector.RemoveChannel(); }
  inline void UpdateChannel() { _selector.UpdateChannel(); }

 private:
  static int64_t GetNewId() {
    static std::atomic<int64_t> id(0);
    return id++;
  }
};

// end happyntrain
}