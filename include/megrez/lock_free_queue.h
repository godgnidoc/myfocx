#ifndef __MEGREZ_LOCK_FREE_QUEUE_H__
#define __MEGREZ_LOCK_FREE_QUEUE_H__

#include <atomic>

namespace megrez {

/**
 * 多生产者单消费者无锁队列
 */
template <typename T>
class MSLockFreeQueue {
 private:
  struct Node {
    T data;
    std::atomic<Node*> next;
  };

 public:
  MSLockFreeQueue() {
    head_ = new Node{{}, nullptr};
    tail_ = head_;
  }

  ~MSLockFreeQueue() {
    while (head_ != nullptr) {
      auto next = head_->next.load();
      delete head_;
      head_ = next;
    }
  }

  void push_back(T const& data) {
    auto node = new Node{data, nullptr};
    auto tail = tail_.load(std::memory_order_acquire);
    Node* expected = nullptr;
    while (!tail->next.compare_exchange_weak(expected, node)) {
      expected = nullptr;
      tail = tail_.load();
    }
    tail_.store(node, std::memory_order_release);
  }

  bool pop_front() {
    auto next = head_->next.load();
    if (next == nullptr) return false;

    delete head_;
    head_ = next;
    return true;
  }

  bool pop_front(T& data) {
    auto next = head_->next.load();
    if (next == nullptr) return false;

    data = std::move(next->data);
    delete head_;
    head_ = next;
    return true;
  }

  T* peek_front() {
    auto next = head_->next.load();
    if (next == nullptr) return nullptr;

    return &next->data;
  }

 private:
  Node* head_;
  std::atomic<Node*> tail_;
};

}  // namespace megrez

#endif  // __MEGREZ_LOCK_FREE_QUEUE_H__