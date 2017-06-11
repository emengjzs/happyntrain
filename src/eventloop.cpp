#include "eventloop.h"

#include <unistd.h>

#include <algorithm>
#include <utility>

#include "selector.h"
#include "util/logging.h"
#include "util/net.h"

using namespace std;
using namespace happyntrain::concurrent;
using namespace happyntrain::timer;
using namespace happyntrain::network;

namespace happyntrain {

// --------------------------
class EventLoop;
// --------------------------

EventLoop::EventLoop(int taskCapacity)
    : selector_(CreateNewSelector()),
      terminate_(false),
      taskQueue_(),
      cleanTaskQueue_(),
      timerTaskManager_(), 
      wakeUpHandler_() {
  InitWakeUpEventChannel();
}

EventLoop::~EventLoop() {}

void EventLoop::InitWakeUpEventChannel() { wakeUpHandler_.Init(this); }

// Handle the evnetLoop once;
void EventLoop::LoopOnce() {
  // Phase1: Timer Task
  timerTaskManager_.ExecuteTimeoutTasks();
  // Phase2: Select IO event
  int select_wait_time = min(int64_t(1000), timerTaskManager_.GetNextTimeout());
  selector_->SelectOnce(select_wait_time);
  // Phase3: CleanUp Task for connections, etc...
  CompleteCleanUpTasks();
}

// Start the eventloop
void EventLoop::Run() {
  while (!terminate_) {
    LoopOnce();
  }
}

// Shut down the eventloop
void EventLoop::ShutDown() {
  terminate_ = true;
  WakeUp();
}

// Submit a Runnable task
TaskId EventLoop::SubmitTask(uint64_t delayMs, Runnable&& task) {
  return timerTaskManager_.AddTask(std::forward<Runnable>(task), delayMs);
}

void EventLoop::SubmitTask(Runnable&& task) {
  DEBUG("+ Task(%p)", &task);
  taskQueue_.Push(std::forward<Runnable>(task));
  WakeUp();
}

void EventLoop::CompleteTasks() {
  while (!taskQueue_.empty()) {
    taskQueue_.Pop()();
  }
}

void EventLoop::CompleteCleanUpTasks() {
  while (! cleanTaskQueue_.empty()) {
    Runnable& task = cleanTaskQueue_.front();
    task();
    cleanTaskQueue_.pop();
  }
}

// Wake up the eventloop to complete the task submitted.
void EventLoop::WakeUp() { wakeUpHandler_.WakeUp(); }

// -----------------------
class WakeUpHandler;
// -------------------------

EventLoop::WakeUpHandler::WakeUpHandler() : eventLoop_(NULL), wakeUp_(false), eventfd_() {}

void EventLoop::WakeUpHandler::Init(EventLoop* eventLoop) {
  eventLoop_ = eventLoop;
  EXPECT(eventfd_.fd() > 0, "Event fd create failed.");
  INFO("+ eventfd(%d)", eventfd_.fd());
  eventChannel_ = Ptr<Channel>(new Channel(eventLoop->selector_.get(), eventfd_.fd()));
  eventChannel_->SetReadEnable().OnRead(
      [this] { this->OnRead(); });
}

void EventLoop::WakeUpHandler::OnRead() {
  uint64_t v = 0;
  if (eventChannel_->fd() >= 0) {
    v = eventfd_.read();
    INFO("Read eventfd, fd(%d), v(%lu)", eventChannel_->fd(), v);
  }
  if (wakeUp_) {
    WARN("%s", "This eventloop has been waked up before! Skip this.");
    return;
  }
  if (v <= 0) {
    WARN("Eventfd read error, v(%lu), eventfd(%d)", v,
         eventChannel_->fd());
    return;
  }
  wakeUp_ = true;
  eventLoop_->CompleteTasks();
  wakeUp_ = false;
}

void EventLoop::WakeUpHandler::WakeUp() {
  INFO("Wake up eventloop(%ld)", eventLoop_->selector_->id());
  bool r = eventfd_.write(1);
  EXPECT(r == true, "Write eventfd failed: r(%d) fd(%d)", r,
         eventChannel_->fd());
}

// ---------------------
class Channel;
// ----------------------
SequenceCreator<uint64_t> Channel::next_id_(0);

Channel::Channel(Selector* selector, int fd)
    : selector_(selector), fd_(fd), id_(next_id_()), events_flag_(0) {
  // int errcode = network::setNonBlock(fd_);
  // EXPECT(errcode == 0, "fd %d cannot set nonblock.", fd_);
  selector_->AddChannel(this);
}

void Channel::Close() {
  if (fd_ > 0) {
    DEBUG("Delete Channel(%lu) fd(%d)", id_, fd_);
    selector_->RemoveChannel(this);
    ::close(fd_);
    fd_ = -1;
    EmitReadable();
  }
}

}  // end happyntrain
