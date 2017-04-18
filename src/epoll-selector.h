#pragma once

#include <sys/epoll.h>

#include "util.h"

namespace happyntrain {

class EpollSelector : NoCopy {
  int _epoll_fd;

 public:
  EpollSelector();
  ~EpollSelector();

  void AddChannel();
  void RemoveChannel();
  void UpdateChannel();
};
}