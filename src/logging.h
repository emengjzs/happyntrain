#pragma once

#include <cstdio>

namespace happyntrain {

#define LOG(message, level, ...) \
  printf("[" level "] %s-%d " message "\n", __FILE__, __LINE__, ##__VA_ARGS__);
#define INFO(message, ...) LOG(message, "INFO", ##__VA_ARGS__);
#define DEBUG(message, ...) LOG(message, "DEBUG", ##__VA_ARGS__);
#define ERROR(message, ...) LOG(message, "ERROR", ##__VA_ARGS__);

// end happyntrain
}