#pragma once

#include <poll.h>

#include <atomic>

#include "selector.h"
#include "timertask.h"
#include "util/concurrent.h"
#include "util/core.h"

namespace happyntrain {

class EventLoop : NoCopy {
 private:
  Ptr<Selector> selector_;
  // A singal to turnoff eventloop
  std::atomic<bool> terminate_;
  int32_t selectCycleMs_;
  concurrent::LinkedBlockingQueue<concurrent::Runnable> taskQueue_;
  timer::TimerTaskManager timerTaskManager_;

  class WakeUpHandler : NoCopy {
    Ptr<Channel> eventChannel_;
    EventLoop* eventLoop_;
    bool wakeUp_;

   public:
    explicit WakeUpHandler();
    ~WakeUpHandler() {}
    void Init(EventLoop* eventLoop);
    void WakeUp();
    void OnRead();
  };

  WakeUpHandler wakeUpHandler_;

  void InitWakeUpEventChannel();
  void CompleteTasks();

 public:
  explicit EventLoop(int taskCapacity);
  ~EventLoop();

  // Handle the evnetLoop once;
  void LoopOnce();

  // Start the eventloop
  void Run();

  // Shut down the eventloop
  void ShutDown();

  // Submit a Runnable task, run it after delayMs ms.
  timer::TaskId SubmitTask(uint64_t delayMs, concurrent::Runnable&& task);

  // Submit a Runnable task, thread-safe
  void SubmitTask(concurrent::Runnable&& task);

  // Wake up the eventloop to complete the task submitted.
  void WakeUp();

 private:
};

class EventLoopGroup : NoCopy {};

class Channel : NoCopy {
  // next_id_(0)
  static SequenceCreator<uint64_t> next_id_;

  Selector* selector_;
  int fd_;
  uint64_t id_;
  short events_flag_;
  concurrent::Runnable read_handler_;
  concurrent::Runnable write_handler_;
  concurrent::Runnable error_handler_;

  Channel& SetEventsFlag(const int flag, bool enable) {
    DEBUG("%04x", events_flag_);
    events_flag_ = enable ? events_flag_ | flag : events_flag_ & (~flag);
    selector_->UpdateChannel(this);
    DEBUG("%04x", events_flag_);
    return *this;
  }

 public:
  static const int kReadEventFlag = POLLIN;
  static const int kWriteEventFlag = POLLOUT;

  Channel(Selector* selector, int fd);
  ~Channel() { Close(); }

  uint64_t id() const { return id_; }
  int fd() const { return fd_; }
  int events() const { return events_flag_; }

  bool IsReadEnabled() const { return events_flag_ & kReadEventFlag; }
  bool IsWriteEnabled() const { return events_flag_ & kWriteEventFlag; }

  Channel& SetReadEnable(bool enable = true) {
    return SetEventsFlag(kReadEventFlag, enable);
  }

  Channel& SetWriteEnable(bool enable = true) {
    return SetEventsFlag(kWriteEventFlag, enable);
  }

  Channel& OnWrite(const concurrent::Runnable& onwrite) {
    write_handler_ = onwrite;
    return *this;
  }

  Channel& OnRead(const concurrent::Runnable& onread) {
    read_handler_ = onread;
    return *this;
  }

  void EmitReadable() { read_handler_(); }
  void EmitWritable() { write_handler_(); }

  void Close();
};

// end happyntrain
}
