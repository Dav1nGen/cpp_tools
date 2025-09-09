/**
 * @file thread_communicate.hpp
 * @author Dav1nGen (davicheng1114@gmail.com)
 * @brief 
 * @version 1.0
 * @date 2025-07-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#pragma once

// System
#include <unistd.h>

// C++ standard library header file
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <queue>
#include <thread>

// Third-party library headers
#include <spdlog/spdlog.h>
#include <boost/lockfree/spsc_queue.hpp>

template <typename DATA_TYPE>
class ThreadCommunicate {
 public:
  ThreadCommunicate() = default;
  ~ThreadCommunicate() = default;
  void Consumer();
  void Producer(const DATA_TYPE& data);
  DATA_TYPE GetLatestData();
  void Stop();

  std::atomic<bool> is_running = true;  // wait fix

 private:
  std::atomic<bool> stop_flag_ = false;
  std::mutex mutex_;
  std::condition_variable not_empty_;
  std::condition_variable not_full_;
  std::queue<DATA_TYPE> queue_;
  size_t capacity_ = 256;
};

/**
 * @brief producer function: push data into queue
 * 
 * @tparam DATA_TYPE 
 * @param data 
 */
template <typename DATA_TYPE>
void ThreadCommunicate<DATA_TYPE>::Producer(const DATA_TYPE& data) {
  if (!stop_flag_.load()) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_full_.wait(lock, [this] {
        return queue_.size() < capacity_ && !stop_flag_.load();
      });
      queue_.push(data);
      not_empty_.notify_one();
    }
  }
}

/**
 * @brief Get latest data function (thread-safe) (consumer function)
 * 
 * @tparam DATA_TYPE 
 * @return DATA_TYPE 
 */
template <typename DATA_TYPE>
DATA_TYPE ThreadCommunicate<DATA_TYPE>::GetLatestData() {
  DATA_TYPE data;
  if (!stop_flag_.load()) {
    {
      std::unique_lock<std::mutex> lock(mutex_);
      not_empty_.wait(lock,
                      [this] { return !queue_.empty() && !stop_flag_.load(); });

      data = queue_.front();
      queue_.pop();
      not_full_.notify_one();
      return data;
    }
  }
}

/**
 * @brief stop product
 * 
 * @tparam DATA_TYPE 
 */
template <typename DATA_TYPE>
void ThreadCommunicate<DATA_TYPE>::Stop() {
  stop_flag_.store(true);
  not_empty_.notify_all();  // Notify all waiting consumers
  not_full_.notify_all();   // Notify all waiting producers
  std::cout << "stop!"
            << "\n";
}
