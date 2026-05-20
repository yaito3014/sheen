#ifndef SHEEN_CORE_RESULT_HPP
#define SHEEN_CORE_RESULT_HPP

#include <sheen/core/error.hpp>

#include <expected>

namespace sheen {

template<class T>
using result = std::expected<T, error>;

}  // namespace sheen

#endif  // SHEEN_CORE_RESULT_HPP
