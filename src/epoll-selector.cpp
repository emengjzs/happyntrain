#include "epoll-selector.h"

#include <sys/epoll.h>
#include <unistd.h>

#include "selector.h"
#include "util.h"

using namespace std;

namespace happyntrain {

EpollSelector::EpollSelector() {
  // FD_CLOEXEC
  _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
  EXPECT(_epoll_fd > 0, "")
}

EpollSelector::~EpollSelector() { ::close(_epoll_fd); }

void EpollSelector::AddChannel() {}

void EpollSelector::RemoveChannel() {}

void EpollSelector::UpdateChannel() {}
}