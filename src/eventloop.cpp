#include "eventloop.h"
#include <fcntl.h>

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

SequenceCreator<uint64_t> Channel::_next_id(0);

Channel::Channel(Selector* selector, int fd) : _socket_fd(fd), _id(_next_id()) {
  int errcode = network::setNonBlock(_socket_fd);
  EXPECT(errcode == 0, "fd %d cannot set nonblock.", _socket_fd);
  _selector->AddChannel(this);
}

}  // end happyntrain