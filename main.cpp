#include <iostream>
#include <set>
#include <string>
#include <thread>

#include "src/eventloop.h"
#include "src/selector.h"
#include "src/util/concurrent.h"
#include "src/util/core.h"
#include "src/util/fd.h"
#include "src/util/net.h"

using namespace std;
using namespace happyntrain;
using namespace happyntrain::concurrent;

struct A {
  int a;
  std::string b;
  A(int a_, std::string b_) : a(a_), b(b_) {}
  A() : A(0, "") {}
};

int main() {
  EventLoop eventloop(0);
  set<int> fuckset;
  fuckset.insert(1);
  LinkedBlockingQueue<A> queue;
  queue.Push(A(1, "fuck"));
  DEBUG("%d", queue.Pop().a);
  int fd = network::new_tcp_socket();
  fd::set_fd_nonblocking(fd);
  fd::set_fd_closexec(fd);
  eventloop.SubmitTask(10000, []() { INFO("Hello, world!"); });
  eventloop.SubmitTask(20000, []() { INFO("Fuck you!"); });
  std::thread maint([&]() { eventloop.Run(); });

  for (int i = 0; i < 100; i++) {
    std::thread t([i, &eventloop]() {
      eventloop.SubmitTask([i]() { INFO("Wake up %d!", 2 * i); });
      eventloop.SubmitTask([i]() { INFO("Wake up %d!", 2 * i + 1); });
    });
    t.detach();
  }
  std::thread t1(
      [&]() { eventloop.SubmitTask(30000, [&]() { eventloop.ShutDown(); }); });
  maint.join();
  t1.join();
  return 0;
}
