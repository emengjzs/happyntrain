#include "epoll-selector.h"

#if defined(__linux__)

#include <unistd.h>
#include <iostream>

#include "eventloop.h"

using namespace std;

namespace happyntrain {

class EpollSelector;

struct epoll_event EpollSelector::GetEpollEvent(Channel* channel) {
  struct epoll_event ev = c_struct_init<struct epoll_event>();
  ev.events = channel->events();
  ev.data.ptr = static_cast<void*>(channel);
  return ev;
}

EpollSelector::EpollSelector(int i) : id_(GetNewSelectorId()) {
  DEBUG("param: %d", i);
  // FD_CLOEXEC
  epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
  EXPECT(epoll_fd_ > 0, "epoll create failed");
  INFO("epoll %d created.", epoll_fd_);
}

EpollSelector::~EpollSelector() {
  ::close(epoll_fd_);
  INFO("epoll %d destroyed.", epoll_fd_);
}

void EpollSelector::AddChannel(Channel* channel) {
  DEBUG("Add Channel %lu (fd:%d) to Selector %lu", channel->id(), channel->fd(),
        this->id_);
  struct epoll_event event = GetEpollEvent(channel);
  int r = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &event);
  EXPECT(r == 0, "EPOLL_CTL_ADD Failed");
  activeChannels_.insert(channel);
}

void EpollSelector::RemoveChannel(Channel* channel) {
  DEBUG("Remove Channel %lu (fd:%d) to Selector %lu", channel->id(),
        channel->fd(), this->id_);
  activeChannels_.erase(channel);
}

void EpollSelector::UpdateChannel(Channel* channel) {
  DEBUG("Update Channel %lu (fd:%d) to Selector %lu", channel->id(),
        channel->fd(), this->id_);
  struct epoll_event event = GetEpollEvent(channel);
  int r = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, channel->fd(), &event);
  EXPECT(r == 0, "EPOLL_CTL_MOD Failed");
}

// end happyntrain
}

#endif