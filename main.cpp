#include <iostream>
#include <set>
#include <string>
#include <thread>

#include "src/eventloop.h"
#include "src/selector.h"
#include "src/tcpserver.h"
#include "src/util/concurrent.h"
#include "src/util/core.h"
#include "src/util/fd.h"
#include "src/util/net.h"

using namespace std;
using namespace happyntrain;
using namespace happyntrain::concurrent;
using namespace happyntrain::network;
using namespace happyntrain::fd;

struct A {
  int a;
  std::string b;
  A(int a_, std::string b_) : a(a_), b(b_) {}
  A() : A(0, "") {}
};

int main() {
  EventLoop eventloop(0);
  TCPServer server(&eventloop);
  server.Listen(8888);
  eventloop.Run();
  // auto server = TCPServer::Create([&](auto socket) {
  //                 socker.end("goodbye\n");
  //               })->OnError([&](auto error) {});
  // server.listen([&]() { DEBUG("opened server on ", server.Address()); });
  return 0;
}
