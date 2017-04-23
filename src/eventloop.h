#pragma once

#include <poll.h>

#include <functional>

#include "selector.h"
#include "util/core.h"

namespace happyntrain {

// Interface : () => void
using Runnable = std::function<void()>;

class EventLoop : NoCopy {
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

 private:
};

class EventLoopGroup : NoCopy {};

class Channel : NoCopy {
  // _next_id(0)
  static SequenceCreator<uint64_t> _next_id;

  Selector* _selector;
  int _socket_fd;
  uint64_t _id;
  short _events_flag;
  Runnable _read_handler;
  Runnable _write_handler;
  Runnable _error_handler;

  Channel& SetEventsFlag(const int flag, bool enable) {
    _events_flag = enable ? _events_flag | flag : _events_flag & (~flag);
    return *this;
  }

 public:
  const int kReadEventFlag = POLLIN;
  const int kWriteEventFlag = POLLOUT;

  Channel(Selector* selector, int fd);
  ~Channel() { Close(); }

  uint64_t id() const { return _id; }
  int fd() const { return _socket_fd; }
  int events() const { return _events_flag; }

  bool IsReadEnabled() const { return _events_flag & kReadEventFlag; }
  bool IsWriteEnabled() const { return _events_flag & kReadEventFlag; }

  Channel& SetReadEnable(bool enable = true) {
    return SetEventsFlag(kReadEventFlag, enable);
  }

  Channel& SetWriteEnable(bool enable = true) {
    return SetEventsFlag(kWriteEventFlag, enable);
  }

  Channel& OnWrite(const Runnable& onwrite) {
    _write_handler = onwrite;
    return *this;
  }

  Channel& OnRead(const Runnable& onread) {
    _read_handler = onread;
    return *this;
  }

  void Close();
};

// end happyntrain
}