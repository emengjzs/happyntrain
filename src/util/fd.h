#pragma once

#include <fcntl.h>
#include <unistd.h>

namespace happyntrain {
namespace fd {

enum Flag { NON_BLOCK = O_NONBLOCK, CLOSE_ON_EXEC = O_CLOEXEC };

using FD = int;

template <Flag f>
inline int set_fd_flag(FD fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    return errno;
  }
  DEBUG("set fd(%d) FL(%08x | %08x)", fd, flags, f);
  flags = flags | f;
  return fcntl(fd, F_SETFL, flags);
}

template <>
inline int set_fd_flag<Flag::CLOSE_ON_EXEC>(FD fd) {
  int ret = fcntl(fd, F_GETFD);
  DEBUG("set fd(%d) FD(%08x | %08x)", fd, ret, Flag::CLOSE_ON_EXEC);
  return fcntl(fd, F_SETFD, ret | FD_CLOEXEC);
}

inline int set_fd_nonblocking(FD fd) {
  return set_fd_flag<Flag::NON_BLOCK>(fd);
}

inline int set_fd_closexec(FD fd) {
  return set_fd_flag<Flag::CLOSE_ON_EXEC>(fd);
}

template <typename T>
class FileDiscriptor {
 public:
  FileDiscriptor(int fd) : fd_(fd) {}
  ~FileDiscriptor() {}

  const int fd() const { return fd_; }
  bool valid() const { return fd_ > 0; }
  void close() {
    if (fd_ <= 0) {
      WARN("Closse invalid fd(%d)", fd_);
    } else {
      ::close(fd_);
      fd_ = -1;
    }
  }
  operator const int() { return fd_; }
 protected:
  int fd_;
};

template <typename T>
class AutoClosable: NoCopy {
public:
  template <typename... Args>
  AutoClosable(Args... args): closable_(args...) { }
  ~AutoClosable() {
    DEBUG("del fd(%d)", closable_.fd());
    closable_.close();
  }
  operator const int() { return closable_; }
  T* operator->() { return &closable_;  }
private:
  T closable_;
};

// end fd
}
// end happyntrain
}