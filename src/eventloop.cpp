#include "eventloop.h"

using namesapce std;

class EventLoopImpt : Nocopy {

  public:
};

// --------------------------
// EventLoop
// --------------------------
EventLoop::EventLoop(int taskCapacity) {
  
}

EventLoop::~EventLoop(){

}

// Handle the evnetLoop once;
void EventLoop::LoopOnce() {

}

// Start the eventloop
void EventLoop::Loop() {

}

// Shut down the eventloop
void EventLoop::ShutDown() {

}

// Submit a Runnable task
void EventLoop::SubmitTask(const Runnable& task) {

}

// ??
void EventLoop::WakeUp() {

}