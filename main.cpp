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
#include "src/buffer.h"

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
  auto eventloop = newInstance<EventLoop>(0);
  TCPServer server(eventloop);

  server.OnConnect([](auto& socket) {
    socket->OnRead([](auto& self, auto& buffer) {
      INFO("%s", buffer.retrieve().c_str());
    });
  });

  const int port = 8888; 
  server.Listen(port, [=]{
    INFO("Server is now listening on %d", port);
  });
  eventloop->Run();
  // auto server = TCPServer::Create([&](auto socket) {
  //                 socker.end("goodbye\n");
  //               })->OnError([&](auto error) {});
  // server.listen([&]() { DEBUG("opened server on ", server.Address()); });
  return 0;
}
