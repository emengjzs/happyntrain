#include <iostream>
#include <set>
#include <string>

#include "src/eventloop.h"
#include "src/selector.h"
#include "src/util/concurrent.h"
#include "src/util/core.h"

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
  return 0;
}
