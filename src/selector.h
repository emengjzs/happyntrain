#pragma once

#if defined(__linux__)

#include "memory"

#include "epoll-selector.h"

namespace happyntrain {
typedef EpollSelector Selector;

inline Ptr<Selector> CreateNewSelector() {
  return Ptr<Selector>(new Selector(kMaxSelectEvents));
}

// end happyntrain
}

#endif
