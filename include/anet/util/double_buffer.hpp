#ifndef ANET_INCLUDE_ANET_UTIL_DOUBLE_BUFFER_HPP_
#define ANET_INCLUDE_ANET_UTIL_DOUBLE_BUFFER_HPP_

#include <array>
#include <atomic>

namespace anet::util {

/// 一个简单的DoubleBuffer
/// @tparam BufferType 底层的Buffer类型
/// @tparam IndexType 用来存储当前活跃的Buffer的下标，默认为bool，也可以使用std::atomic<bool>类型来确保操作是原子的
template<typename BufferType, typename IndexType = bool>
class DoubleBuffer {
 public:
  DoubleBuffer() : index(0) {};

  BufferType &GetActiveBuffer() {
    return buffers_[index];
  }

  BufferType &GetInactiveBuffer() {
    return buffers_[index ^ 1];
  }

  void SwitchBuffer() {
    index ^= 1;
  }

 private:
  std::array<BufferType, 2> buffers_;
  IndexType index;
};

template<typename BufferType>
using AtomicDoubleBuffer = DoubleBuffer<BufferType, std::atomic<bool>>;

}

#endif //ANET_INCLUDE_ANET_UTIL_DOUBLE_BUFFER_HPP_
