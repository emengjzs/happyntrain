#pragma once

#include <string>

// A simple buffer for socket read/write
// it may be not efficient but we hope that we can do things corrent first
// it may be replaced with more useful implementation futher, may be...
class Buffer {
  std::string content_;
  size_t predictSize_;

 public:
  Buffer(): content_(), predictSize_(0) {}
  Buffer(Buffer&& buffer): content_(std::move(buffer.content_)), predictSize_(0) { }
  Buffer(std::string&& str): content_(std::move(str)), predictSize_(0) { }
  Buffer(const std::string& str): content_(str), predictSize_(0) { }
  Buffer(const char* bytes, size_t size): content_(bytes, size), predictSize_(0) { }
  
  inline void append(const char* bytes) { content_.append(bytes); }
  inline void append(const char* bytes, size_t size) { content_.append(bytes, size); }
  inline void append(const std::string& str) { content_.append(str); }

  // get the contents out and set contents empty
  inline std::string retrieve() { 
    std::string temp(std::move(content_));
    predictSize_ = (
      (predictSize_ - (predictSize_ >> 2)) + 
      (temp.size() >> 2) + 
      (predictSize_ >> 4)
      );
    content_.reserve(predictSize_);
    return temp;
  }

  inline size_t size() const { return content_.size(); }
  inline bool empty() const { return content_.empty(); }
};