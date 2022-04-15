#ifndef ANET_INCLUDE_ANET_BASE_NONCOPYABLE_HPP_
#define ANET_INCLUDE_ANET_BASE_NONCOPYABLE_HPP_

namespace anet {

class noncopyable {
 public:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
 protected:
  noncopyable() = default;
  ~noncopyable() = default;
};

} // namespace anet

#endif //ANET_INCLUDE_ANET_BASE_NONCOPYABLE_HPP_
