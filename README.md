# Happy Network Train

Just to study how to implement a Reactor-Model network library for Linux, just for fun... Not suitable for production environment.

## Environment

- Ubuntu 16.04 LTS x86-64
- Clang++ 3.8
- cmake 3.5.1

## Build

```bash
mkdir build && cd build && cmake .. && make
```

## Example

The example is written in C++14.

```C++
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
      self->Send(buffer);
    });
  });

  const int port = 8888; 
  server.Listen(port, [=]{
    INFO("Server is now listening on %d", port);
  });
  eventloop->Run();
  return 0;
}
```
  ​

