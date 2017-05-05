#pragma once

#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <string>

namespace happyntrain {

namespace network {

inline int setNonBlock(int fd, bool value = true) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return errno;
  }
  flags = value ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

inline int new_tcp_socket() { return socket(AF_INET, SOCK_STREAM, 0); }

struct IP4Address {
  std::string host;
  short port;

  IP4Address(std::string host_, short port_) : host(host_), port(port_) {}
};

// end network
}
// end happyntrain
}