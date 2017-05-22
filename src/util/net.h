#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/epoll.h>

#include <string>

#include "core.h"
#include "fd.h"

namespace happyntrain {

// tcp with ipv4 only!!
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

class ServerSocketFD : public fd::FileDiscriptor<ServerSocketFD> {
 public:
  ServerSocketFD() : FileDiscriptor(new_tcp_socket()) {}
  ~ServerSocketFD() {}

  bool setReusePort() { return setSocketOpt(SO_REUSEPORT) == 0; }
  bool setNonBlock() { return network::setNonBlock(fd_, true) == 0; }
  bool setReuseAddress() { return setSocketOpt(SO_REUSEADDR) == 0; }

  bool bind(std::string host, uint16_t port) {
     C_STRUCT(sockaddr_in, addr);
     addr.sin_family = AF_INET;
     addr.sin_port = htons(port);
     addr.sin_addr.s_addr = INADDR_ANY;
     return ::bind(fd_, (struct sockaddr*) &addr, sizeof(struct sockaddr)) == 0;
  }

  bool listen(int toBeAcceptedQueueLength) {
    return ::listen(fd_, toBeAcceptedQueueLength) == 0;
  }

 private:
  int setSocketOpt(int opt) {
    int flag = 1;
    return setsockopt(fd_, SOL_SOCKET, opt, &flag, sizeof(flag));
  }
};

// Default set closexec
class EpollFD : public fd::FileDiscriptor<EpollFD> {
 public:
  EpollFD(): FileDiscriptor(epoll_create1(EPOLL_CLOEXEC)) {}
};

// Default set nonblock and closexec
class ConnectionSocketFD : public fd::FileDiscriptor<ConnectionSocketFD> {
 public:
  ConnectionSocketFD(int listenedFD): FileDiscriptor(initConnectionFD(listenedFD)) {}
  ConnectionSocketFD(ConnectionSocketFD&& socketFD): FileDiscriptor(socketFD.fd_) {
    socketFD.fd_ = -1; 
  }
 
  template<size_t len>
  int read(char (&buffer)[len]) {
    return ::read(fd_, buffer, len);
  }

 private:
  static int initConnectionFD(int listened_fd) {
    struct sockaddr_in remote_addr;
    socklen_t rsz = sizeof(remote_addr);
    return accept4(listened_fd, (struct sockaddr *) &remote_addr, &rsz, SOCK_NONBLOCK | SOCK_CLOEXEC);
  }
};

struct IP4Address {
  std::string host;
  uint16_t port;

  IP4Address(uint16_t port_) : IP4Address("", port_) {}
  IP4Address(std::string host_, uint16_t port_) : host(host_), port(port_) {}
};

class AddressInfoHolder : NoCopy {
  struct addrinfo* addrInfo;
  void free() {
    if (addrInfo) freeaddrinfo(addrInfo);
  }

 public:
  AddressInfoHolder() : AddressInfoHolder(NULL) {}
  AddressInfoHolder(struct addrinfo* addrinfo_) : addrInfo(addrinfo_) {}
  ~AddressInfoHolder() { free(); }

  void reset(struct addrinfo* newaddrInfo) {
    DEBUG("*addrInfo(%p => %p)", addrInfo, newaddrInfo);
    if (addrInfo != newaddrInfo) {
      free();
      addrInfo = newaddrInfo;
    }
  }

  addrinfo* operator->() { return addrInfo; }
  operator bool() { return addrInfo; }
  operator addrinfo*() { return addrInfo; }
};

class AddressInfoForClient {
  AddressInfoHolder addrinfo_;
  std::string host_;
  int port_;
  bool valid_;

  void GetAddressInfoForClient(const std::string& hoststr, short port) {
    // arguments for getaddrinfo
    const char* hostname = hoststr.empty() ? NULL : hoststr.c_str();
    const char* service = std::to_string(port).c_str();
    C_STRUCT(addrinfo, settings);
    settings.ai_family = AF_INET;
    settings.ai_socktype = SOCK_STREAM;
    settings.ai_protocol = IPPROTO_TCP;
    struct addrinfo* results;

    // return
    int r = getaddrinfo(hostname, service, &settings, &results);
    valid_ = (r == 0 && results);
    if (valid_) {
      addrinfo_.reset(results);

      // retrieve ip and host
      struct sockaddr_in* addr = (struct sockaddr_in*)(addrinfo_->ai_addr);
      // port
      char ip[INET_ADDRSTRLEN];
      port_ = ntohs(addr->sin_port);
      // ip
      inet_ntop(AF_INET, &addr->sin_addr, ip, INET_ADDRSTRLEN);
      host_ = ip;
    }
    DEBUG("Address ip(%s) port(%d)", host_.c_str(), port_);
  }

  void GetAddressInfoForServer() {}

 public:
  AddressInfoForClient(std::string&& hostname, short port)
      : addrinfo_(), host_(hostname), port_(port), valid_(false) {
    GetAddressInfoForClient(hostname, port);
  }
  ~AddressInfoForClient() {}

  bool valid() { return valid_; }
  int port() { return port_; }
  std::string ip() { return host_; }
};

// end network
}
// end happyntrain
}
