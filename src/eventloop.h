#pragma once

#include <functional>

#include "util.h"

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

class Channel : NoCopy {};
}