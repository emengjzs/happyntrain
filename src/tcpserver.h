#pragma once

#include <sys/socket.h>

#include "eventloop.h"
#include "util/core.h"
#include "util/net.h"

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
  network::IP4Address address_;

 public:
  TCPChannel(EventLoop* eventloop);
  ~TCPChannel();

  virtual void Setup(const network::IP4Address address) = 0;
  void Send(std::string msg);
  void Close();

  State state() const { return state_; }
};

class ServerTCPChannel : public TCPChannel {
 public:
  void Bind(const network::IP4Address& address) {}
};

class ClientTCPChannel : public TCPChannel {
 public:
  void Connect(const network::IP4Address& address) {}
};

class TCPServer : NoCopy {
 public:
  static Ref<TCPServer> Create();
  TCPServer();
  TCPServer(EventLoop* eventloop);
  ~TCPServer();
  void Listen();
  void Listen(std::string host, int port);
  void Close();
  void OnListening();
  void OnError();
  void OnClose();
  void Address();

 private:
  Channel* listenChannel_;
  EventLoop* eventloop_;
  network::IP4Address address_;
  void Bind();
};

// end happyntrain
}