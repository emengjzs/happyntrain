#include <sys/eventfd.h>

#include <unistd.h>
#include "fd.h"

namespace happyntrain {
namespace fd {

class EventFD : public fd::FileDiscriptor<EventFD> {
 public:
  EventFD() : FileDiscriptor(eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK)) {}
  ~EventFD() {}

  uint64_t read() {
    uint64_t v = 0;
    int r = -1;
    r = ::read(fd_, &v, sizeof(v));
    return r == 0 ? v : r; 
  }

  bool write(uint64_t n) {
    int r = ::write(fd_, &n, sizeof(n));
    return r == sizeof(n);
  }

};

}
}