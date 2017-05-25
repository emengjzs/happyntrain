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

  void OnRead(DataListener&& dataListener) { dataListener_ = dataListener; } 

  State state() const { return state_; }

 private:
  void OnReadable(const Ref<TCPChannel>& self);
  void FinishNonBlockingRead(const Ref<TCPChannel>& self);
  void OrderCleanUpTask();
  void CleanUp(const Ref<TCPChannel>& self);
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
  using ConnectionListener = std::function<void(const Ref<TCPChannel>&)>;

  //TODO: 
  TCPServer(): TCPServer(nullptr) { }
  explicit TCPServer(const Ref<EventLoop>& eventloop);
  ~TCPServer() {}

  void Listen() { Listen(Runnable{}); }
  void Listen(int port) { Listen(port, Runnable{}); };
  void Listen(Runnable&& onListening);
  void Listen(int port, Runnable&& onListening);

  void Close();
  void OnListening();
  void OnConnect(ConnectionListener&& cb);
  void OnError();
  void OnClose();

  network::IP4Address address() const { return address_; }

 private:
  Ptr<Channel> listenChannel_;
  Ref<EventLoop> eventloop_;
  network::IP4Address address_;

  ConnectionListener connectionCallback_;
  
  void Bind(network::ServerSocketFD& listenFD);
  void OnAcceptable();
};

// end happyntrain
}