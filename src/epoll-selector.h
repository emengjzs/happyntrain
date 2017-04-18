#pragma once


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