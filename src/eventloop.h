#pragma once

#include "util.h"

namespace happyntrain {

class EventLoop : NoCopy {
public:
  explicit EventLoop(int taskCapacity);
  ~EventLoop();

  // Handle the evnetLoop once;
  void LoopOnce();
  void Loop();
  void ShutDown();
  void SubmitTask();
  void WakeUp();

 private:
  
};

class EventLoopGroup : NoCopy {};

class Channel : NoCopy {};
}