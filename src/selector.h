#pragma once

#if defined(__linux__)

#include "epoll-selector.h"

namespace happyntrain {
  using Selector = EpollSelector;
}

#endif

