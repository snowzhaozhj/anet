#ifndef ANET_INCLUDE_ANET_SERVICE_SERVICE_CONTEXT_HPP_
#define ANET_INCLUDE_ANET_SERVICE_SERVICE_CONTEXT_HPP_

#include <any>
#include <memory>
#include <string>
#include <unordered_map>

namespace anet {

class Context {
 public:
  Context() = default;

  template<typename T>
  void Set(const std::string &key, T &&value) {
    data_[key] = std::forward<T>(value);
  }

  template<typename T>
  T *Get(const std::string &key) {
    auto it = data_.find(key);
    if (it == data_.end()) {
      return nullptr;
    }
    auto val = std::any_cast<T>(&it->second);
    return val;
  }

 private:
  std::unordered_map<std::string, std::any> data_;
};

using ContextPtr = std::shared_ptr<Context>;

} // namespace anet

#endif // ANET_INCLUDE_ANET_SERVICE_SERVICE_CONTEXT_HPP_
