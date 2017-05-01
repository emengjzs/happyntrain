#pragma once

#include <cstdio>

namespace happyntrain {

#define LOG(message, level, ...)                                     \
  printf("[%5s] %s-%d " message "\n", level, __FILENAME__, __LINE__, \
         ##__VA_ARGS__);

#define INFO(message, ...) LOG(message, "INFO", ##__VA_ARGS__);
#define DEBUG(message, ...) LOG(message, "DEBUG", ##__VA_ARGS__);
#define ERROR(message, ...) \
  LOG("\x1b[31m" message "\x1b[0m", "ERROR", ##__VA_ARGS__);

// end happyntrain
}