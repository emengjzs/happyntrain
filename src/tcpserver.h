#pragma once

#include "util/core.h"
#include "util/net.h"
#include "eventloop.h"

namespace happyntrain {

class TCPChannel : public Sharable<TCPChannel>, private NoCopy {
 public:
  enum State {
    INVALID,
    CONNECTED,
    CLOSED,
    FAILED,
  };

 private:
  State state_;
  Ptr<Channel> channel_;
  EventLoop* eventloop_;
  network::IP4Address address_;
  ConnectionSocketFD connectionSocket_;

 public:
  TCPChannel(EventLoop* eventloop, ConnectionSocketFD&& connectionSocket);
  ~TCPChannel();

  void Send(std::string msg);
  void Close();
  void Register(); 

  State state() const { return state_; }

 private:
  void OnReadable();
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
  void Listen(int port);
  void Close();
  void OnListening();
  void OnConnect();
  void OnError();
  void OnClose();
  void Address();

 private:
  Ptr<Channel> listenChannel_;
  EventLoop* eventloop_;
  network::IP4Address address_;
  
  void Bind(network::ServerSocketFD& listenFD);
  void OnAcceptable();
  
};

// end happyntrain
}