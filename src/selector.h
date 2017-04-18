#pragma once

#include "util.h"

namespace happyntrain {

template <typename SelectorImpt>
class Selector : NoCopy {
  SelectorImpt _selector;
  int64_t _id;

 public:
  Selector() : _id(GetNewId()) {}
  ~Selector();

  inline void AddChannel() { _selector.AddChannel(); }
  inline void RemoveChannel() { _selector.RemoveChannel(); }
  inline voud UpdateChannel() { _selector.UpdateChannel(); }

 private:
  static int64_t GetNewId() {
    static std::atomic<int64_t> id(0);
    return id++;
  }
};
}