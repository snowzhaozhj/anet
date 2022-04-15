#ifndef ANET_INCLUDE_ANET_BASE_SINGLETON_HPP_
#define ANET_INCLUDE_ANET_BASE_SINGLETON_HPP_

#include "anet/base/noncopyable.hpp"

namespace anet {

template<typename T>
class Singleton : noncopyable {
 public:
  Singleton() = default;
  ~Singleton() = default;

  T &GetInstance() {
    static T instance;
    return instance;
  }
};

}

#endif //ANET_INCLUDE_ANET_BASE_SINGLETON_HPP_
