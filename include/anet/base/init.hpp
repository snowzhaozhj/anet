#ifndef ANET_INCLUDE_ANET_BASE_INIT_HPP_
#define ANET_INCLUDE_ANET_BASE_INIT_HPP_

#include "anet/base/concat.hpp"

#define __ANET_INIT_FUNCNAME(name) NET_CONCAT(__NET_INIT_, name)
#define ANET_INIT(name) inline void __attribute__((constructor)) __ANET_INIT_FUNCNAME(name)()

#endif //ANET_INCLUDE_ANET_BASE_INIT_HPP_
