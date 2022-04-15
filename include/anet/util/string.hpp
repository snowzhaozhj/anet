#ifndef ANET_INCLUDE_ANET_UTIL_STRING_HPP_
#define ANET_INCLUDE_ANET_UTIL_STRING_HPP_

#include <cstring>
#include <string>

namespace anet::util {

struct CaseInsensitiveLess {
  bool operator()(const std::string &lhs, const std::string &rhs) const {
//    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end(),
                                        [](char a, char b) {
                                          return std::tolower(a) < std::tolower(b);
                                        });
  }
};

inline bool StringToInt(int &value, const std::string &s, int base = 10) {
  try {
    value = std::stoi(s, nullptr, base);
    return true;
  } catch (...) {
    return false;
  }
}

inline std::pair<bool, int> StringToInt(const std::string &s, int base = 10) {
  int val;
  bool success = StringToInt(val, s, base);
  return {success, val};
}

inline bool HasPrefix(std::string_view s, std::string_view prefix) {
  if (prefix.size() > s.size()) return false;
  return std::equal(prefix.begin(), prefix.end(),
                    s.begin());
}

/// @brief 如果s包含前缀prefix，则将去除前缀后的s写入result\n
/// @brief 如果s不包含前缀prefix，则不对result进行修改
/// @return 返回true表示s包含前缀prefix，返回false表示s不包含前缀prefix
inline bool RemovePrefix(std::string &result, std::string_view s, std::string_view prefix) {
  if (!HasPrefix(s, prefix)) return false;
  result = s.substr(prefix.size());
  return true;
}

}  // namespace anet::util

#endif //ANET_INCLUDE_ANET_UTIL_STRING_HPP_
