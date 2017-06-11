#pragma once

#include <poll.h>

#include <atomic>
#include <queue>

#include "selector.h"
#include "timertask.h"
#include "util/concurrent.h"
#include "util/core.h"
#include "util/net.h"
#include "util/eventfd.h"

namespace happyntrain {

class EventLoop : NoCopy {
 private:
  Ptr<Selector> selector_;
  // A singal to turnoff eventloop
  std::atomic<bool> terminate_;
  concurrent::LinkedBlockingQueue<Runnable> taskQueue_;
  std::queue<Runnable> cleanTaskQueue_;
  timer::TimerTaskManager timerTaskManager_;


  class WakeUpHandler : NoCopy {
    Ptr<Channel> eventChannel_;
    EventLoop* eventLoop_;
    bool wakeUp_;
    fd::EventFD eventfd_;

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
  void CompleteCleanUpTasks();

 public:
  explicit EventLoop(int taskCapacity);
  ~EventLoop();

  // not thread safe!!
  static Ref<EventLoop> GetDefaultEventLoop() { return newInstance<EventLoop>(0); }

  // Handle the evnetLoop once;
  void LoopOnce();

  // Start the eventloop
  void Run();

  // Shut down the eventloop
  void ShutDown();

  // Submit a Runnable task, run it after delayMs ms.
  timer::TaskId SubmitTask(uint64_t delayMs, Runnable&& task);

  // Submit a Runnable task, thread-safe
  void SubmitTask(Runnable&& task);

  // Submit a Runnable clean up task, not thread-safe
  void SubmitCleanUpTask(Runnable&& task) { cleanTaskQueue_.emplace(std::forward<Runnable>(task)); }

  // Wake up the eventloop to complete the task submitted.
  void WakeUp();

  template <typename FileDiscriptor>
  inline Ptr<Channel> RegisterChannel(const FileDiscriptor& fileDiscriptor);
 
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
  Runnable read_handler_;
  Runnable write_handler_;
  Runnable error_handler_;

  Channel& SetEventsFlag(const int flag, bool enable) {
    events_flag_ = enable ? events_flag_ | flag : events_flag_ & (~flag);
    selector_->UpdateChannel(this);
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

  Channel& SetReadWriteEnable() {
    return SetEventsFlag(kReadEventFlag | kWriteEventFlag, true); 
  }

  Channel& OnWrite(const Runnable& onwrite) {
    write_handler_ = onwrite;
    return *this;
  }

  Channel& OnRead(const Runnable& onread) {
    read_handler_ = onread;
    return *this;
  }

  void DisableHandler() { read_handler_ = write_handler_ = error_handler_ = []{}; }
  void EmitReadable() { read_handler_(); }
  void EmitWritable() { write_handler_(); }

  void Close();
};

template <typename FileDiscriptor>
Ptr<Channel> EventLoop::RegisterChannel(const FileDiscriptor& fileDiscriptor) {
  return Ptr<Channel>(new Channel(selector_.get(), fileDiscriptor.fd()));
}

// end happyntrain
}
