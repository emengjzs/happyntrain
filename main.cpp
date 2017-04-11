#include "src/util.h"

using namespace happyntrain;

int main() {
  Ref<int> integer = std::make_shared<int>(2);
  Ptr<int> a2(new int(2));
  return 0;
}