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
      connectionSocket_(std::forward<ConnectionSocketFD>(connectionSocket)) {}

TCPChannel::~TCPChannel() {}

void TCPChannel::Send(std::string msg) {}

void TCPChannel::Close() {}

// Register channel to eventloop
// including selector, events, and event-callback
void TCPChannel::Register() {
  channel_ = std::move(eventloop_->RegisterChannel(connectionSocket_));
  // TODO: should register kWriteEventFlag ?
  channel_->SetReadEnable();
  state_ = State::CONNECTED;
  channel_->OnRead(std::bind(&OnReadable, this));
}

void TCPChannel::OnReadable() {
  if (state_ != State::CONNECTED) {
    WARN("connection fd(%d) not in connected state", channel_->fd());
  }
  connectionSocket_.read()
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
  listenChannel_ = std::move((eventloop_->RegisterChannel(listenFD)));
  listenChannel_->SetReadEnable().OnRead([this]() { OnAcceptable(); });
}

void TCPServer::Bind(SocketFD& listenFD) {
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
      ERROR("accept connection failed on fd(%d)", listenChannel_->fd());
      break;
    }
    Ref<TCPChannel> connection = newInstance<TCPChannel>(eventloop_, std::move(connectionSocket));
    connection->Register();
    // connected !
    OnConnect();
    connection->OnReadable();
  }
}

void TCPServer::OnConnect() {
  
}

// end happytrain
}