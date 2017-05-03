#pragma once
#if defined(__linux__)

#include <set>

#include <sys/epoll.h>

#include "selector-base.h"

namespace happyntrain {

class Channel;

class EpollSelector : NoCopy {
  int64_t id_;
  int epoll_fd_;
  int last_active_event_idx_;
  std::set<Channel*> active_channels_;
  struct epoll_event active_events_[kMaxSelectEvents];

  static inline struct epoll_event GetEpollEvent(Channel* channel);

 public:
  explicit EpollSelector(int i);
  ~EpollSelector();

  int64_t id() const { return id_; }

  void AddChannel(Channel* channel);
  void RemoveChannel(Channel* channel);
  void UpdateChannel(Channel* channel);

  // Wait events for almost wait_ms ms.
  void SelectOnce(int wait_ms);
};

// end happyntrain
}

#endif