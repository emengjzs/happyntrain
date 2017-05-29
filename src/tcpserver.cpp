#include "tcpserver.h"

#include "util/net.h"
#include "util/core.h"
#include <assert.h>

#include <sys/uio.h>

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

TCPChannel::~TCPChannel() { DEBUG("Destroy TCPChannel port(%u)", address_.port); }

void TCPChannel::Send(std::string&& msg) {
  outBuffer_.append(std::forward<string>(msg));
  if (! channel_->IsWriteEnabled()) {
    channel_->SetWriteEnable();
  }
}


void TCPChannel::Close() {}


// Register channel to eventloop
// including selector, events, and event-callback
void TCPChannel::Register() {
  channel_ = eventloop_->RegisterChannel(connectionSocket_);
  // TODO: should register kWriteEventFlag ?
  channel_->SetReadEnable();
  state_ = State::CONNECTED;
  auto self = shared_from_this();
  channel_->OnRead([self] { self->OnReadable(self); });
  channel_->OnWrite([self] { self->OnWritable(self); });
}


void TCPChannel::OnReadable(const Ref<TCPChannel>& self) {
  assert(self.get() == this);

  if (state_ != State::CONNECTED) {
    WARN("Connection fd(%d) not in connected state when readable.", channel_->fd());
    // TODO: handle exception 
    return;
  }

  const size_t maxBufferSize = 1 << 15;
  while (state_ == State::CONNECTED) {
    Ptr<char[]> buffer(new char[predictInBufferSize_]);
    ssize_t actualReadSize = connectionSocket_.read(buffer.get(), predictInBufferSize_);
    DEBUG("Channel id(%lu) fd(%d) read(%zd/%zu)", 
      channel_->id(), channel_->fd(), actualReadSize, predictInBufferSize_);

    if (actualReadSize == -1) {
      int err = connectionSocket_.err();
      if (err == EAGAIN || err == EWOULDBLOCK) {
        FinishNonBlockingRead(self);
        break;
      } else if (err == EINTR)  // interrupted
        continue;
    } 
    if (actualReadSize <= 0) {
      OrderCleanUpTask();
      break;
    } else {
      inBuffer_.append(buffer.get(), actualReadSize);
      if (actualReadSize == predictInBufferSize_ && predictInBufferSize_ < maxBufferSize) {
        predictInBufferSize_ <<= 1;
      }    
    } 
  }
}

void TCPChannel::OnWritable(const Ref<TCPChannel>& self) {
  assert(self.get() == this);

  if (state_ != State::CONNECTED) {
    WARN("Connection fd(%d) not in connected state when writable.", channel_->fd());
    return;
  }
  
  struct iovec iov[32];
  while (! outBuffer_.empty()) {
    int invcnt = outBuffer_.setupIovec(iov, 32);
    ssize_t nwritten = writev(channel_->fd(), iov, invcnt);
    DEBUG("Channel id(%lu) fd(%d) write(%zd/%zu)", 
      channel_->id(), channel_->fd(), nwritten, outBuffer_.size());

    if (nwritten > 0) {
      outBuffer_.pop(nwritten);
      continue;
    }
    if (nwritten == -1) {
      int err = connectionSocket_.err();
      if (err == EAGAIN || err == EWOULDBLOCK) {
        break;
      } else if (err == EINTR)  // interrupted
        continue;
    }
    if (nwritten <= 0) {
      WARN("Channel id(%lu) fd(%d) write error(%d)", channel_->id(), channel_->fd(), connectionSocket_.err());
      break;
    } 
  }

  if (outBuffer_.empty() && channel_->IsWriteEnabled()) {
     channel_->SetWriteEnable(false);
  }

}

void TCPChannel::FinishNonBlockingRead(const Ref<TCPChannel>& self) {
  DEBUG("Buffer(%s)", inBuffer_.str().c_str());
  if (dataListener_) {
    dataListener_(self, inBuffer_);
  }
  else {
    inBuffer_.clear();
  }
}

void TCPChannel::OrderCleanUpTask() {
  auto self = shared_from_this();
  eventloop_->SubmitCleanUpTask([self] { self->CleanUp(self); });
}

void TCPChannel::CleanUp(const Ref<TCPChannel>& self) {
  if (! inBuffer_.empty()) {
    FinishNonBlockingRead(self);
  }
  WARN("Connection fd(%d) clean up", channel_->fd());
  state_ = State::CLOSED;
  dataListener_ = nullptr;
  channel_->DisableHandler();
}

// -------------------
class TCPServer;
//--------------------

TCPServer::TCPServer(const Ref<EventLoop>& eventloop)
    : listenChannel_(nullptr),
      eventloop_(eventloop),
      address_("127.0.0.1", 12346),
      connectionCallback_() {}

void TCPServer::Listen(int port, Runnable&& onListening) {
  address_ = IP4Address(port);
  Listen(std::forward<Runnable>(onListening));
}

void TCPServer::Listen(Runnable&& onListening) {
  ServerSocketFD listenFD;
  EXIT_IF(listenFD.invalid(), "Create Server Socket failed");

  Bind(listenFD);
  EXIT_IF(listenFD.listen(128) == false, "fd(%d) listen to port(%d) failed",
          listenFD.fd(), address_.port);
          
  INFO("Server Socket FD(%d) is now listening on port(%d)", listenFD.fd(), address_.port);
  if (onListening) {
    onListening();
  }
  listenChannel_ = eventloop_->RegisterChannel(listenFD);
  listenChannel_->SetReadEnable().OnRead([this] { this->OnAcceptable(); });
}

void TCPServer::Bind(ServerSocketFD& listenFD) {
  // Fail the program if these fail
  EXIT_IF(listenFD.invalid(), "Create Server Socket fd failed");
  EXIT_IF(listenFD.setNonBlock() == false, "Server Socket fd(%d) set NONBLOCK fail",
          listenFD.fd());

  // Still go on even if these fail
  EXPECT(listenFD.setReusePort(), "Server Socket fd(%d) set REUSEPORT fail", listenFD.fd());
  EXPECT(listenFD.setCloseOnExec(), "Server Socket fd(%d) set CLOSEONEXEC fail",
         listenFD.fd());

  EXIT_IF(listenFD.bind(address_.host, address_.port) == false,
          "Server Socket fd(%d) bind port(%d) failed", listenFD.fd(), address_.port);
}

void TCPServer::OnAcceptable() {
  const int listenFD = listenChannel_->fd();
  while (true) {
    ConnectionSocketFD connectionSocket(listenFD);
    if (connectionSocket.invalid()) {
      if (connectionSocket.err() != EAGAIN && connectionSocket.err() != EINTR)
        ERROR("Accept connection failed on Server Socket fd(%d)", listenFD);
      break;
    }
    DEBUG("Accept connection fd(%d) succeed on Server Socket fd(%d)", connectionSocket.fd(), listenFD);
    Ref<TCPChannel> connection = newInstance<TCPChannel>(eventloop_.get(), std::move(connectionSocket));
    connection->Register();
    // connected !
    connectionCallback_(connection);
  }
}

void TCPServer::OnConnect(ConnectionListener&& cb) {
  connectionCallback_ = cb;
}

// end happytrain
}