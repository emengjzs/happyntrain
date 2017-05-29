#include <iostream>
#include <string>

#include "src/eventloop.h"
#include "src/tcpserver.h"
#include "src/util/core.h"

using namespace std;
using namespace happyntrain;

int main() {
  auto eventloop = newInstance<EventLoop>(0);
  TCPServer server(eventloop);

  server.OnConnect([](auto& socket) {
    socket->OnRead([](auto& self, auto& buffer) {
      buffer.retrieve();
      self->Send("Hello World!");
    });
  });

  const int port = 8888; 
  server.Listen(port, [=]{
    INFO("Server is now listening on %d", port);
  });
  eventloop->Run();
  return 0;
}
