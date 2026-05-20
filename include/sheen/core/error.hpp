#ifndef SHEEN_CORE_ERROR_HPP
#define SHEEN_CORE_ERROR_HPP

#include <source_location>
#include <string>
#include <string_view>
#include <utility>

namespace sheen {

enum class error_code {
  io_failure,
  type_mismatch,
  parse_error,
  missing_key,
  out_of_range,
  user_rejected,
};

struct error {
  error_code code;
  std::string detail;
  std::source_location where;

  constexpr error(error_code c = {}, std::string_view d = {}, std::source_location loc = std::source_location::current()) : code{c}, detail{d}, where{loc} {}
};

}  // namespace sheen

#endif  // SHEEN_CORE_ERROR_HPP
