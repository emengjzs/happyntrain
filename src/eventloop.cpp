#include "eventloop.h"

#include <fcntl.h>
#include <unistd.h>

#include "selector.h"
#include "util/net.h"

using namespace std;

namespace happyntrain {

// --------------------------
// EventLoop
// --------------------------

EventLoop::EventLoop(int taskCapacity) {}

EventLoop::~EventLoop() {}

// Handle the evnetLoop once;
void EventLoop::LoopOnce() {}

// Start the eventloop
void EventLoop::Loop() {}

// Shut down the eventloop
void EventLoop::ShutDown() {}

// Submit a Runnable task
void EventLoop::SubmitTask(const Runnable& task) {}

// ??
void EventLoop::WakeUp() {}

//
class Channel;

SequenceCreator<uint64_t> Channel::next_id_(0);

Channel::Channel(Selector* selector, int fd) : socket_fd_(fd), id_(next_id_()) {
  int errcode = network::setNonBlock(socket_fd_);
  EXPECT(errcode == 0, "fd %d cannot set nonblock.", socket_fd_);
  selector_->AddChannel(this);
}

void Channel::Close() {
  if (socket_fd_ > 0) {
    DEBUG("Closing Channel %lu (fd: %d)", id_, socket_fd_);
    selector_->RemoveChannel(this);
    ::close(socket_fd_);
    socket_fd_ = -1;
    EmitReadable();
  }
}

}  // end happyntrain