#pragma once

#include <poll.h>

#include <atomic>
#include <functional>

#include "selector.h"
#include "util/concurrent.h"
#include "util/core.h"

namespace happyntrain {

// Interface : () => void
using Runnable = std::function<void()>;

class EventLoop : NoCopy {
 private:
  Ptr<Selector> selector_;
  // A singal to turnoff eventloop
  std::atomic<bool> terminate_;

  concurrent::LinkedBlockingQueue<Runnable> taskQueue_;

  class WakeUpHandler : NoCopy {
    Channel* eventChannel_;
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

 public:
  explicit EventLoop(int taskCapacity);
  ~EventLoop();

  // Handle the evnetLoop once;
  void LoopOnce();

  // Start the eventloop
  void Loop();

  // Shut down the eventloop
  void ShutDown();

  // Submit a Runnable task
  void SubmitTask(const Runnable& task);

  // ??
  void WakeUp();

  void CompleteTasks();

 private:
};

class EventLoopGroup : NoCopy {};

class Channel : NoCopy {
  // next_id_(0)
  static SequenceCreator<uint64_t> next_id_;

  Selector* selector_;
  int socket_fd_;
  uint64_t id_;
  short events_flag_;
  Runnable read_handler_;
  Runnable write_handler_;
  Runnable error_handler_;

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
  int fd() const { return socket_fd_; }
  int events() const { return events_flag_; }

  bool IsReadEnabled() const { return events_flag_ & kReadEventFlag; }
  bool IsWriteEnabled() const { return events_flag_ & kWriteEventFlag; }

  Channel& SetReadEnable(bool enable = true) {
    return SetEventsFlag(kReadEventFlag, enable);
  }

  Channel& SetWriteEnable(bool enable = true) {
    return SetEventsFlag(kWriteEventFlag, enable);
  }

  Channel& OnWrite(const Runnable& onwrite) {
    write_handler_ = onwrite;
    return *this;
  }

  Channel& OnRead(const Runnable& onread) {
    read_handler_ = onread;
    return *this;
  }

  void EmitReadable() { read_handler_(); }
  void EmitWritable() { write_handler_(); }

  void Close();
};

// end happyntrain
}
