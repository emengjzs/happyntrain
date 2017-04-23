#pragma once

#include <set>

#include <sys/epoll.h>

#include "eventloop.h"
#include "selector.h"
#include "util.h"

namespace happyntrain {

class EpollSelector : public Selector<EpollSelector> {
  int _epoll_fd;
  std::set<Channel*> _activeChannels;
  struct epoll_event _activeEvents[kMaxSelectEvents];

 public:
  explicit EpollSelector(int i);
  ~EpollSelector();

  void AddChannel();
  void RemoveChannel();
  void UpdateChannel();
};

using NIOSelector = EpollSelector;
}