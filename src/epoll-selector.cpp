#include "epoll-selector.h"

#if defined(__linux__)

#include <unistd.h>
#include <iostream>

#include "eventloop.h"
#include "util/time.h"

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
  INFO("+ epollfd(%d)", epoll_fd_);
}

EpollSelector::~EpollSelector() {
  while (!active_channels_.empty()) {
    // Channel will delete himself from channel set automatically.
    (*active_channels_.begin())->Close();
  }
  ::close(epoll_fd_);
  INFO("- epollfd(%d)", epoll_fd_);
}

void EpollSelector::AddChannel(Channel* channel) {
  DEBUG("+ Channel(%lu) fd(%d) read(%d) write(%d) => Selector(%lu)",
        channel->id(), channel->fd(), channel->IsReadEnabled(),
        channel->IsWriteEnabled(), this->id_);
  struct epoll_event event = GetEpollEvent(channel);
  int r = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, channel->fd(), &event);
  EXPECT(r == 0, "EPOLL_CTL_ADD Failed");
  active_channels_.insert(channel);
}

void EpollSelector::RemoveChannel(Channel* channel) {
  DEBUG("- Channel(%lu) fd(%d) => Selector(%lu)", channel->id(), channel->fd(),
        this->id_);
  active_channels_.erase(channel);
}

void EpollSelector::UpdateChannel(Channel* channel) {
  DEBUG("* Channel(%lu) fd(%d) read(%d) write(%d) => Selector(%lu)",
        channel->id(), channel->fd(), channel->IsReadEnabled(),
        channel->IsWriteEnabled(), this->id_);
  struct epoll_event event = GetEpollEvent(channel);
  int r = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, channel->fd(), &event);
  EXPECT(r == 0, "EPOLL_CTL_MOD Failed");
}

void EpollSelector::SelectOnce(int wait_ms) {
  auto begin = time::time_begin();
  last_active_event_idx_ =
      epoll_wait(epoll_fd_, active_events_, kMaxSelectEvents, wait_ms);
  time::time_end(begin, "Epoll Select: %d events", last_active_event_idx_);
  EXPECT(last_active_event_idx_ >= 0, "Epoll Error: return %d",
         last_active_event_idx_);
  for (int i = --last_active_event_idx_; i >= 0;
       i--, last_active_event_idx_--) {
    Channel* channel = static_cast<Channel*>(active_events_[i].data.ptr);
    int events = active_events_[i].events;
    if (channel) {
      if (events & (Channel::kReadEventFlag | POLLERR)) {
        DEBUG("Channel [%lu] FD [%d] EVENT READ", channel->id(), channel->fd());
        channel->EmitReadable();
      } else if (events & Channel::kWriteEventFlag) {
        DEBUG("Channel [%lu] FD [%d] EVENT WRITE", channel->id(),
              channel->fd());
        channel->EmitWritable();
      } else {
        ERROR("Channel [%lu] FD [%d] EVENT UKNOWN %4x", channel->id(),
              channel->fd(), channel->events());
      }
    }
  }
}

// end happyntrain
}

#endif