#pragma once
#if defined(__linux__)

#include <set>

#include <sys/epoll.h>

#include "selector-base.h"

namespace happyntrain {

class Channel;

class EpollSelector {
  int64_t id_;
  int epoll_fd_;
  std::set<Channel*> activeChannels_;
  struct epoll_event activeEvents_[kMaxSelectEvents];

  static inline struct epoll_event GetEpollEvent(Channel* channel);

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