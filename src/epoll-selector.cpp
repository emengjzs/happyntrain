#include "epoll-selector.h"

#include <unistd.h>
#include <iostream>

using namespace std;

namespace happyntrain {

EpollSelector::EpollSelector(int i) : Selector() {
  DEBUG("param: %d", i);
  // FD_CLOEXEC
  _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
  EXPECT(_epoll_fd > 0, "epoll create failed");
  INFO("epoll %d created.", _epoll_fd);
}

EpollSelector::~EpollSelector() {
  ::close(_epoll_fd);
  INFO("epoll %d destroyed.", _epoll_fd);
}

void EpollSelector::AddChannel() {
  cout << " add epollselector! channel" << endl;
}

void EpollSelector::RemoveChannel() {}

void EpollSelector::UpdateChannel() {}
}