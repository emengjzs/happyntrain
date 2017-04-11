#pragma once

#include <memory>

namespace happyntrain {

// -------------------------
// memory
// -------------------------
template <typename Object>
using Ref = std::shared_ptr<Object>;

template <typename Object>
using Ptr = std::unique_ptr<Object>;

// -------------------------
// Effective C++
// -------------------------
// macro
#define DISABLE_COPY(Clazz)     \
 private:                       \
  Clazz(const Clazz&) = delete; \
  Clazz(Clazz&&) = delete;      \
  Clazz& operator=(const Clazz&) = delete;

// zero-cost class
class NoCopy {
  NoCopy(){};
  DISABLE_COPY(NoCopy);
};

// --------------------------
}
