#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include <string>

#include "core.h"

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

struct IP4Address {
  std::string host;
  short port;

  IP4Address(short port_) : IP4Address("", port_) {}
  IP4Address(std::string host_, short port_) : host(host_), port(port_) {}
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
