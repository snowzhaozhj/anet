#ifndef ANET_INCLUDE_ANET_UTIL_ASIO_IO_SERVICE_POOL_HPP_
#define ANET_INCLUDE_ANET_UTIL_ASIO_IO_SERVICE_POOL_HPP_

#include "anet/base/noncopyable.hpp"

#include "asio/io_context.hpp"
#include <thread>

namespace anet::util {

struct GuardedIOContext {
  GuardedIOContext()
      : io_context_ptr(std::make_unique<asio::io_context>()),
        guard(asio::make_work_guard(*io_context_ptr)) {}

  std::unique_ptr<asio::io_context> io_context_ptr;
  asio::executor_work_guard<asio::io_context::executor_type> guard;
};

class IOContextPool : noncopyable {
 public:
  /// @note 请确保pool_size > 0
  explicit IOContextPool(std::size_t pool_size) : index_(0) {
    guarded_io_context_vec_.resize(pool_size);
  }

  /// @brief 启动IOContextPool并阻塞到所有IOContext执行结束
  void Run() {
    std::vector<std::thread> thread_vec;
    for (auto &guarded_io_context : guarded_io_context_vec_) {
      thread_vec.emplace_back([&] { guarded_io_context.io_context_ptr->run(); });
    }

    for (auto &thread : thread_vec) {
      thread.join();
    }
  }

  /// @brief 停止IOContextPool
  void Stop() {
    for (auto &guarded_io_context : guarded_io_context_vec_) {
      guarded_io_context.io_context_ptr->stop();
    }
  }

  /// @brief 使用RoundRobin获取Pool中的一个IOContext
  /// @note 非线程安全
  asio::io_context &GetIOContext() {
    asio::io_context &io_context = *guarded_io_context_vec_[index_].io_context_ptr;
    ++index_;
    if (index_ == guarded_io_context_vec_.size()) {
      index_ = 0;
    }
    return io_context;
  }

 private:
  std::vector<GuardedIOContext> guarded_io_context_vec_;
  std::size_t index_;
};

} // namespace anet::util

#endif //ANET_INCLUDE_ANET_UTIL_ASIO_IO_SERVICE_POOL_HPP_
