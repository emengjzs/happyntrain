#include "eventloop.h"

#include <fcntl.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <functional>

#include "selector.h"
#include "util/logging.h"
#include "util/net.h"

using namespace std;
using namespace happyntrain::concurrent;

namespace happyntrain {

// --------------------------
class EventLoop;
// --------------------------

EventLoop::EventLoop(int taskCapacity)
    : selector_(CreateNewSelector()),
      terminate_(false),
      taskQueue_(),
      wakeUpHandler_() {
  InitWakeUpEventChannel();
}

EventLoop::~EventLoop() {}

void EventLoop::InitWakeUpEventChannel() { wakeUpHandler_.Init(this); }

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

void EventLoop::CompleteTasks() {
  while (!taskQueue_.empty()) {
    taskQueue_.Pop()();
  }
}
// -----------------------
class WakeUpHandler;
// -------------------------

EventLoop::WakeUpHandler::WakeUpHandler() : eventLoop_(NULL), wakeUp_(false) {}

void EventLoop::WakeUpHandler::Init(EventLoop* eventLoop) {
  eventLoop_ = eventLoop;
  int efd = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  EXPECT(efd > 0, "Event fd create failed.");
  INFO("+ eventfd(%d)", efd);
  eventChannel_ = new Channel(eventLoop->selector_.get(), efd);
  eventChannel_->SetReadEnable().OnRead(
      bind(&EventLoop::WakeUpHandler::OnRead, this));
}

void EventLoop::WakeUpHandler::OnRead() {
  uint64_t v = 0;
  int r = -1;
  if (eventChannel_->fd() >= 0) {
    r = read(eventChannel_->fd(), &v, sizeof(v));
    INFO("Read eventfd, fd(%d), v(%lu)", eventChannel_->fd(), v);
  }
  if (wakeUp_) {
    WARN("%s", "This eventloop has been waked up before! Skip this.");
    return;
  }
  if (r < 0 || v == 0) {
    WARN("Eventfd read error, r(%d), v(%lu), eventfd(%d)", r, v,
         eventChannel_->fd());
    return;
  }
  wakeUp_ = true;
  eventLoop_->CompleteTasks();
  wakeUp_ = false;
}

void EventLoop::WakeUpHandler::WakeUp() {
  INFO("Wake up eventloop(%ld)", eventLoop_->selector_->id());
  uint64_t n = 1;
  int r = write(eventChannel_->fd(), &n, sizeof(n));
  EXPECT(r == sizeof(n), "Write eventfd failed: r(%d) fd(%d)", r,
         eventChannel_->fd());
}

// ---------------------
class Channel;
// ----------------------
SequenceCreator<uint64_t> Channel::next_id_(0);

Channel::Channel(Selector* selector, int fd)
    : selector_(selector), socket_fd_(fd), id_(next_id_()), events_flag_(0) {
  int errcode = network::setNonBlock(socket_fd_);
  EXPECT(errcode == 0, "fd %d cannot set nonblock.", socket_fd_);
  selector_->AddChannel(this);
}

void Channel::Close() {
  if (socket_fd_ > 0) {
    DEBUG("x Channel(%lu) fd(%d)", id_, socket_fd_);
    selector_->RemoveChannel(this);
    ::close(socket_fd_);
    socket_fd_ = -1;
    EmitReadable();
  }
}

}  // end happyntrain
