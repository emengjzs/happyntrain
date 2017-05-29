#pragma once

#include <sys/uio.h>
#include <assert.h>

#include <string>
#include <deque>

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
      (predictSize_ > temp.size() ? temp.size() >> 4 : predictSize_ >> 4)
      );
    content_.reserve(predictSize_);
    return temp;
  }
  inline const std::string& str() { return content_; } 
  inline size_t size() const { return content_.size(); }
  inline bool empty() const { return content_.empty(); }
  inline void clear() { content_.clear(); }
};

class OutputBuffer {
  std::deque<std::string> content_;
  using size_t =  std::deque<std::string>::size_type;
  size_t size_;
  size_t offset_;

public:
  OutputBuffer(): content_(), size_(0), offset_(0) { }

  void append(std::string&& str) {
    size_ += str.size();
    content_.emplace_back(std::forward<std::string>(str));
  }

  int setupIovec(struct iovec* vec, const size_t size) {
    auto itr = content_.cbegin();
    if (itr == content_.cend() || size <= 0) 
      return 0;
    vec[0].iov_base = static_cast<void*>(const_cast<char*>(itr->data() + offset_));
    vec[0].iov_len = itr->size() - offset_;
    itr ++;

    int i = 1;
    while (i < size && itr != content_.cend()) {
      vec[i].iov_base =  static_cast<void*>(const_cast<char*>(itr->data()));
      vec[i].iov_len = itr->size();
      ++ itr;
      ++ i;
    }
    return i;
  }

  void pop(const size_t size) {
    assert(size <= size_);
    size_t rest = size + offset_;
    while (! content_.empty()) {
      if (content_[0].size() <= rest) {
        rest -= content_[0].size();
        content_.pop_front();
      }
      else {
        break;
      }
    }
    DEBUG("offset_(%zu), size(%zu)", offset_, size_);
    offset_ = rest;
    size_ -= size;
  }

  inline size_t size() const { return size_; }
  inline bool empty() const { return content_.empty(); }
  inline void clear() { content_.clear(); }

};