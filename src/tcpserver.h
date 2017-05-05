#pragma once

#include "eventloop.h"
#include "util/core.h"

namespace happyntrain {

class TCPChannel : NoCopy {
 public:
  enum State {
    INVALID,
    ESTABLISH,
    CONNECTED,
    CLOSED,
    FAILED,
  };

 private:
  State state_;
  Ptr<Channel> channel_;
  EventLoop* eventloop_;

 public:
  TCPChannel(EventLoop* eventloop);
  ~TCPChannel();

  void Connect();
  void Send(std::string msg);
  void Close();

  State state() const { return state_; }
};

// end happyntrain
}