#pragma once

#if defined(__linux__)

#include "memory"

#include "epoll-selector.h"

namespace happyntrain {
using Selector = EpollSelector;

inline Ptr<Selector> CreateNewSelector() {
  return Ptr<Selector>(new Selector(kMaxSelectEvents));
}

// end happyntrain
}

#endif
