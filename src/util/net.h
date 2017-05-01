#pragma once

#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

namespace happyntrain {

namespace network {

int setNonBlock(int fd, bool value = true) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return errno;
  }
  flags = value ? flags | O_NONBLOCK : flags & ~O_NONBLOCK;
  return fcntl(fd, F_SETFL, flags);
}

// end network
}
// end happyntrain
}