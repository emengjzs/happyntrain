#include "tcpserver.h"

#include "util/net.h"

using namespace std;
using namespace happyntrain::network;
using namespace happyntrain::fd;

namespace happyntrain {

class TCPChannel;

TCPChannel::TCPChannel(EventLoop* eventloop, ConnectionSocketFD&& connectionSocket)
    : state_(State::INVALID),
      channel_(nullptr),
      eventloop_(eventloop),
      address_(0),
      connectionSocket_(std::forward<ConnectionSocketFD>(connectionSocket)),
      inBuffer_(),
      predictInBufferSize_(1 << 7),
      outBuffer_(),
      dataListener_(nullptr)
      {}

TCPChannel::~TCPChannel() {}

void TCPChannel::Send(std::string msg) {}

void TCPChannel::Close() {}

// Register channel to eventloop
// including selector, events, and event-callback
void TCPChannel::Register(Ref<TCPChannel> self) {
  channel_ = eventloop_->RegisterChannel(connectionSocket_);
  // TODO: should register kWriteEventFlag ?
  channel_->SetReadEnable();
  state_ = State::CONNECTED;
  channel_->OnRead([self] { self->OnReadable(); });
}


void TCPChannel::OnReadable() {
  if (state_ != State::CONNECTED) {
    WARN("connection fd(%d) not in connected state", channel_->fd());
    // TODO: handle exception 
    return;
  }

  const size_t maxBufferSize = 1 << 15;

  while (state_ == State::CONNECTED) {
    Ptr<char[]> buffer(new char[predictInBufferSize_]);
    ssize_t actualReadSize = connectionSocket_.read(buffer.get(), predictInBufferSize_);
    DEBUG("Channel id(%lu) fd(%d) read(%zu/%zd)", 
      channel_->id(), channel_->fd(), actualReadSize, predictInBufferSize_);

    if (actualReadSize == -1) {
      int err = connectionSocket_.err();
      if (err == EAGAIN || err == EWOULDBLOCK) {
        FinishNonBlockingRead();
        break;
      } else if (err == EINTR)  // interrupted
        continue;
      else {
        break;
      }
    } else if (actualReadSize == 0) {
      break;
    } else {
      inBuffer_.append(buffer.get(), predictInBufferSize_);
      if (actualReadSize == predictInBufferSize_ && predictInBufferSize_ < maxBufferSize) {
        predictInBufferSize_ <<= 1;
      }    
    } 
  }
}

void TCPChannel::FinishNonBlockingRead() {
  DEBUG("Buffer(%s)", inBuffer_.str().c_str());
  if (dataListener_) {
    dataListener_(shared_from_this(), inBuffer_);
  }
  else {
    inBuffer_.clear();
  }
}

// -------------------
class TCPServer;
//--------------------

TCPServer::TCPServer()
    : listenChannel_(nullptr),
      eventloop_(nullptr),
      address_("127.0.0.1", 12346) {}

TCPServer::TCPServer(EventLoop* eventloop)
    : listenChannel_(nullptr),
      eventloop_(eventloop),
      address_("127.0.0.1", 12346) {}

void TCPServer::Listen(int port) {
  address_ = IP4Address(port);
  Listen();
}

void TCPServer::Listen() {
  ServerSocketFD listenFD;
  Bind(listenFD);
  EXIT_IF(listenFD.listen(128) == false, "fd(%d) listen to port(%d) failed",
          listenFD.fd(), address_.port);
  INFO("fd(%d) is now listening on port(%d)", listenFD.fd(), address_.port);
  listenChannel_ = eventloop_->RegisterChannel(listenFD);
  listenChannel_->SetReadEnable().OnRead([this]() { OnAcceptable(); });
}

void TCPServer::Bind(ServerSocketFD& listenFD) {
  // Fail the program if these fail
  EXIT_IF(listenFD.invalid(), "create listen socket fd failed");
  EXIT_IF(listenFD.setNonBlock() == false, "fd(%d) set NONBLOCK fail",
          listenFD.fd());

  // Still go on even if these fail
  EXPECT(listenFD.setReusePort(), "fd(%d) set REUSEPORT fail", listenFD.fd());
  EXPECT(listenFD.setCloseOnExec(), "fd(%d) set CLOSEONEXEC fail",
         listenFD.fd());

  EXIT_IF(listenFD.bind(address_.host, address_.port) == false,
          "fd(%d) bind port(%d) failed", listenFD.fd(), address_.port);
}

void TCPServer::OnAcceptable() {
  const int listenFD = listenChannel_->fd();
  while (true) {
    ConnectionSocketFD connectionSocket(listenFD);
    if (connectionSocket.invalid()) {
      if (connectionSocket.err() != EAGAIN && connectionSocket.err() != EINTR)
        ERROR("accept connection failed on listen fd(%d)", listenFD);
      break;
    }
    DEBUG("accept connection fd(%d) succeed on listen fd(%d)", connectionSocket.fd(), listenFD);
    Ref<TCPChannel> connection = newInstance<TCPChannel>(eventloop_, std::move(connectionSocket));
    connection->Register(connection);
    // connected !
    OnConnect();
  }
}

void TCPServer::OnConnect() {
  
}

// end happytrain
}