#include <iostream>
#include <set>
#include <string>

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
  Ref<Selector> integer = newInstance<Selector>(2);
  { Ptr<Selector> a2(new Selector(2)); }
  set<int> fuckset;
  fuckset.insert(1);
  Selector s(1);
  LinkedBlockingQueue<A> queue;
  queue.Push(A(1, "fuck"));
  DEBUG("%d", queue.Pop().a);
  return 0;
}
