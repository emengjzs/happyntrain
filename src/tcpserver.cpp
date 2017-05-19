#include "tcpserver.h"

#include "util/net.h"

using namespace std;
using namespace happyntrain::network;
using namespace happyntrain::fd;

namespace happyntrain {

TCPChannel::TCPChannel(EventLoop* eventloop)
    : eventloop_(eventloop), state_(State::INVALID), address_(0) {}

TCPChannel::~TCPChannel() {}

void TCPChannel::Setup(const IP4Address address) {
  if (state_ == State::ESTABLISH || state_ == CONNECTED) {
    WARN("TCP socket is in state(%d)", state_);
  }
  int fd = new_tcp_socket();
  EXPECT(fd > 0, "+ socket fd(%d) failed ", fd);
  set_fd_nonblocking(fd);
  set_fd_closexec(fd);
}

void TCPChannel::Send(std::string msg) {}

void TCPChannel::Close() {}

// -------------------
class TCPServer;
//--------------------

TCPServer::TCPServer()
    : listenChannel_(NULL), eventloop_(NULL), address_("127.0.0.1", 12346) {}

TCPServer::TCPServer(EventLoop* eventloop)
    : listenChannel_(NULL),
      eventloop_(eventloop),
      address_("127.0.0.1", 12346) {}

void TCPServer::Bind() {
  SocketFD listenFD;

  // Fail the program if these fail
  EXIT_IF(listenFD.invalid(), "create listen socket fd failed");
  EXIT_IF(listenFD.setNonBlock() == false, "fd(%d) set NONBLOCK fail", listenFD.fd());
  
  // Still go on even if these fail
  EXPECT(listenFD.setReusePort(), "fd(%d) set REUSEPORT fail", listenFD.fd());
  EXPECT(listenFD.setCloseOnExec(), "fd(%d) set CLOSEONEXEC fail", listenFD.fd());

  EXIT_IF(listenFD.bind(address_.host, address_.port) == false, 
    "fd(%d) bind port(%d) failed", listenFD.fd(), address_.port);
}


void TCPServer::Listen() {
  void Listen();
}

// end happytrain
}