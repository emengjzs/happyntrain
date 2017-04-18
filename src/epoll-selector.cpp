#include "epoll-selector.h"

#include "selector.h"
#include "util.h"

using namespace std;

namespace happyntrain {

EpollSelector::EpollSelector() {
  _epoll_fd = epoll_create1(EPOLL_CLOEXEC);
  EXPECT_THAT()
}

EpollSelector::~EpollSelector() {}

void EpollSelector::AddChannel() {}

void EpollSelector::RemoveChannel() {}

voud EpollSelector::UpdateChannel() {}
}