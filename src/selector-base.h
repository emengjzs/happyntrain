#pragma once

#include <atomic>

#include "util/core.h"

namespace happyntrain {

const int kMaxSelectEvents = 2000;

static int64_t GetNewSelectorId() {
  static std::atomic<int64_t> id(0);
  return id++;
}

// end happyntrain
}