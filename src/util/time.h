#pragma once

#include <chrono>
#include <ctime>

#include "logging.h"

namespace happyntrain {

namespace time {

using microseconds = std::chrono::microseconds;
using seconds = std::chrono::seconds;
using minutes = std::chrono::minutes;
using hours = std::chrono::hours;

template <class T>
int64_t current_time() {
  std::chrono::time_point<std::chrono::steady_clock> p =
      std::chrono::steady_clock::now();
  return std::chrono::duration_cast<T>(p.time_since_epoch()).count();
}

inline clock_t time_begin() { return std::clock(); }

template <typename... Vars>
inline void time_end(const clock_t begin, const char* message, Vars&&... args) {
  printf("Task: ");
  printf(message, args...);
  printf(" -- %.2fs\n", double(std::clock() - begin) / CLOCKS_PER_SEC);
}
// end time
}
// end happyntrain
}