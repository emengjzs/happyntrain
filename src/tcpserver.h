#pragma once

#include "util/core.h"
#include "util/net.h"
#include "eventloop.h"
#include "buffer.h"

namespace happyntrain {

class TCPChannel : public Sharable<TCPChannel>, private NoCopy {
 public:
  enum State {
    INVALID,
    CONNECTED,
    CLOSED,
    FAILED,
  };

  using DataListener = std::function<void(const Ref<TCPChannel>&, Buffer&)>;

 private:
  State state_;

  Ptr<Channel> channel_;
  EventLoop* eventloop_;
  network::IP4Address address_;
  network::ConnectionSocketFD connectionSocket_;

  Buffer inBuffer_;
  size_t predictInBufferSize_;
  Buffer outBuffer_;

  DataListener dataListener_;

 public:
  TCPChannel(EventLoop* eventloop, network::ConnectionSocketFD&& connectionSocket);
  ~TCPChannel();

  void Send(std::string msg);
  void Close();
  void Register();

  void OnRead(DataListener dataListener) { dataListener_ = dataListener; } 

  State state() const { return state_; }

 private:
  
  void OnReadable();
  void FinishNonBlockingRead();
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
  ~TCPServer() {}
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