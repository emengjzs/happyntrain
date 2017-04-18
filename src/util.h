#pragma once

#include <memory>

#include "logging.h"

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
#define DISABLE_COPY(Class)     \
 private:                       \
  Class(const Class&) = delete; \
  Class(Class&&) = delete;      \
  Class& operator=(const Class&) = delete;

// zero-cost class
class NoCopy {
 public:
  NoCopy(){};
  DISABLE_COPY(NoCopy);
};

// --------------------------
// Fail checking
// --------------------------
#define EXPECT(expression, message, ...)                                     \
  do {                                                                       \
    if (!(expression)) {                                                     \
      ERROR("Fail: " message, ##__VA_ARGS__);                                \
      ERROR("File: %s, Line: %d, Fail Expression: (%s)", __FILE__, __LINE__, \
            #expression);                                                    \
    }                                                                        \
  } while (0);

#define EXIT_IF(expression, message, ...)                                \
  do {                                                                   \
    if ((expression)) {                                                  \
      ERROR("Fail: " message, ##__VA_ARGS__);                            \
      ERROR("File: %s, Line: %d, Unexpected Expression: (%s)", __FILE__, \
            __LINE__, #expression);                                      \
      exit(1);                                                           \
    }                                                                    \
  } while (0);

// end happyntrain
}
