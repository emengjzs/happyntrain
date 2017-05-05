#pragma once

#include <chrono>
#include <ctime>

#include "logging.h"

namespace happyntrain {

namespace time {

using microseconds = std::chrono::microseconds;
using us = std::chrono::microseconds;
using milliseconds = std::chrono::milliseconds;
using ms = std::chrono::milliseconds;
using seconds = std::chrono::seconds;
using minutes = std::chrono::minutes;
using hours = std::chrono::hours;

template <class T>
int64_t current_time() {
  using namespace std::chrono;
  auto p = time_point_cast<ms>(steady_clock::now());
  return duration_cast<T>(p.time_since_epoch()).count();
}

template <class T>
inline int64_t now() {
  return current_time<T>();
}

inline clock_t time_begin() { return std::clock(); }

template <typename... Vars>
inline void time_end(const clock_t begin, const char* message, Vars&&... args) {
  printf("Task: ");
  printf(message, args...);
  printf(" -- %.2fs\n", (double)(std::clock() - begin) / 1000);
}

// Use RAII for timing
class timing {
  clock_t begin_;
  const std::string msg_;

 public:
  timing(std::string&& msg) : begin_(std::clock()), msg_(msg) {}
  ~timing() { time_end(begin_, "%s", msg_.c_str()); }
};

// end time
}
// end happyntrain
}