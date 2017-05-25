#pragma once

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>

#include "core.h"

namespace happyntrain {
namespace concurrent {


template <class T>
class LinkedBlockingQueue : NoCopy {
  std::size_t capacity_;
  std::atomic<std::size_t> size_;
  std::mutex push_mutex_;
  std::mutex pop_mutex_;
  std::condition_variable not_full_;
  std::condition_variable not_empty_;

  struct Node {
    T value;
    Node* next;
    explicit Node(T&& val) : value(val), next(NULL) {}
  };

  Node* head_;
  Node* tail_;

 public:
  LinkedBlockingQueue()
      : LinkedBlockingQueue(std::numeric_limits<std::size_t>::max()) {}
  explicit LinkedBlockingQueue(std::size_t capacity);

  ~LinkedBlockingQueue();

  void Push(T&& t);
  bool Push(T&& t, int time_ms) = delete;
  bool TryPush(T&& t) = delete;

  T Pop();
  bool Pop(T& t, int time_ms) = delete;
  bool TryPop(T& t) = delete;

  inline std::size_t size() const { return size_.load(); }
  inline std::size_t capacity() const { return capacity_; }
  inline bool empty() const { return size_ == 0; }
};

template <class T>
LinkedBlockingQueue<T>::~LinkedBlockingQueue() {
  Node* current = head_;
  Node* temp;
  while (current) {
    temp = current->next;
    delete current;
    current = temp;
  }
}

template <class T>
LinkedBlockingQueue<T>::LinkedBlockingQueue(std::size_t capacity)
    : capacity_(capacity),
      size_(0),
      push_mutex_(),
      pop_mutex_(),
      not_full_(),
      not_empty_() {
  std::unique_lock<std::mutex> lock(push_mutex_);
  Node* node = new Node(T());
  head_ = node;
  tail_ = node;
}

template <class T>
void LinkedBlockingQueue<T>::Push(T&& t) {
  int c = -1;
  {
    std::unique_lock<std::mutex> lock(push_mutex_);
    while (size() == capacity_) {
      not_full_.wait(lock);
    }
    Node* node = new Node(std::move(t));
    tail_->next = node;
    tail_ = node;
    c = size_++;
    if (size_ + 1 < capacity_) {
      not_full_.notify_one();
    }
  }
  if (c == 0) {
    not_empty_.notify_one();
  }
}

template <class T>
T LinkedBlockingQueue<T>::Pop() {
  int c = -1;
  std::unique_lock<std::mutex> lock(pop_mutex_);
  while (size() == 0) {
    not_empty_.wait(lock);
  }
  Node* node = head_;
  Node* top = node->next;
  head_ = top;
  delete node;
  T t(std::move(top->value));
  c = size_--;
  if (size_ > 1) {
    not_empty_.notify_one();
  }
  lock.unlock();
  if (c == capacity_) {
    not_full_.notify_one();
  }
  return t;
}
// end concurrent
}
// end happyntrain
}
