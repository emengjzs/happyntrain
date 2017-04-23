#include "epoll-selector.h"

#if defined(__linux__)

#include <unistd.h>
#include <iostream>

#include "eventloop.h"

using namespace std;

namespace happyntrain {

struct epoll_event EpollSelector::GetEpollEvent(Channel* channel) const {
  struct epoll_event ev = c_struct_init<struct epoll_event>();
  ev.events = channel->events();
  ev.data.ptr = static_cast<void*>(channel);
  return ev;
}

EpollSelector::EpollSelector(int i) : _id(GetNewSelectorId()) {
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

void EpollSelector::AddChannel(Channel* channel) {
  DEBUG("Add Channel %lu (fd:%d) to Selector %lu", channel->id(), channel->fd(),
        this->_id);
  struct epoll_event event = GetEpollEvent(channel);
  int r = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, channel->fd(), &event);
  EXPECT(r == 0, "EPOLL_CTL_ADD Failed");
  _activeChannels.insert(channel);
}

void EpollSelector::RemoveChannel(Channel* channel) {
  DEBUG("Remove Channel %lu (fd:%d) to Selector %lu", channel->id(),
        channel->fd(), this->_id);
  _activeChannels.erase(channel);
}

void EpollSelector::UpdateChannel(Channel* channel) {
  DEBUG("Update Channel %lu (fd:%d) to Selector %lu", channel->id(),
        channel->fd(), this->_id);
  struct epoll_event event = GetEpollEvent(channel);
  int r = epoll_ctl(_epoll_fd, EPOLL_CTL_MOD, channel->fd(), &event);
  EXPECT(r == 0, "EPOLL_CTL_MOD Failed");
}
}

#endif 