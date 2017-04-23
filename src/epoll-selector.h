#pragma once
#if defined(__linux__)

#include <set>

#include <sys/epoll.h>

#include "selector-base.h"

namespace happyntrain {

class Channel;

class EpollSelector {
  int64_t _id;
  int _epoll_fd;
  std::set<Channel*> _activeChannels;
  struct epoll_event _activeEvents[kMaxSelectEvents];

  struct epoll_event GetEpollEvent(Channel* channel) const;

 public:
  explicit EpollSelector(int i);
  ~EpollSelector();

  void AddChannel(Channel* channel);
  void RemoveChannel(Channel* channel);
  void UpdateChannel(Channel* channel);
};

// end happyntrain
}

#endif