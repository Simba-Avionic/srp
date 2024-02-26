/**
 * @file wait_queue.h
 * @author Bartosz Snieg (snieg45@gmail.com)
 * @brief
 * @version 0.1
 * @date 2024-02-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef CORE_COMMON_WAIT_QUEUE_H_
#define CORE_COMMON_WAIT_QUEUE_H_
#include <condition_variable>  // NOLINT
#include <mutex>               // NOLINT
#include <queue>
namespace simba {
namespace core {
template <typename T, uint16_t maxsize = 0>
class WaitQueue final {
 private:
  std::mutex m;
  std::mutex add_mutex;
  std::queue<T> q{};
  std::condition_variable cv;

 public:
  WaitQueue(/* args */) = default;
  WaitQueue(const WaitQueue&) = delete;
  WaitQueue(const WaitQueue&&) = delete;
  ~WaitQueue() = default;
  bool push(T obj) {
    bool flag = true;
    std::lock_guard<std::mutex> guard(add_mutex);
    if (static_cast<uint16_t>(q.size()) > maxsize && maxsize != 0) {
      q.pop();
      flag = false;
    }
    q.push(obj);
    cv.notify_one();
    return flag;
  }
  T Get() noexcept {
    if (q.empty()) {
      std::unique_lock lk(m);
      cv.wait(lk, [this] { return !q.empty(); });
    }
    std::lock_guard<std::mutex> guard(add_mutex);
    auto o = q.front();
    q.pop();
    return o;
  }
  T GetWithoutRemove() noexcept {
    if (q.empty()) {
      std::unique_lock lk(m);
      cv.wait(lk, [this] { return !q.empty(); });
    }
    std::lock_guard<std::mutex> guard(add_mutex);
    return q.front();
  }
  void Remove() {
    std::lock_guard<std::mutex> guard(add_mutex);
    q.pop();
  }
};
}  // namespace core
}  // namespace simba
#endif  // CORE_COMMON_WAIT_QUEUE_H_"
