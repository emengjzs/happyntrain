#include "tcpserver.h"
#include "util/fd.h"

using namespace std;

namespace happyntrain {

TCPChannel::TCPChannel(EventLoop* eventloop)
    : eventloop_(eventloop), state_(State::INVALID) {}

TCPChannel::~TCPChannel() {}

void TCPChannel::Connect() {
  if (state_ == State::ESTABLISH || state_ == CONNECTED) {
    WARN("TCP socket is in state(%d)", state_);
  }
}

void TCPChannel::Send(std::string msg) {}

void TCPChannel::Close() {}

// end happytrain
}