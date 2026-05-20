#ifndef SHEEN_CORE_EXCEPTION_HPP
#define SHEEN_CORE_EXCEPTION_HPP

#include <sheen/core/error.hpp>

#include <stdexcept>
#include <string>
#include <utility>

namespace sheen {

class exception : public std::runtime_error {
public:
  explicit exception(error e) : std::runtime_error{detail_or_default(e)}, error_{std::move(e)} {}

  error const& info() const noexcept { return error_; }

private:
  static std::string detail_or_default(error const& e) { return e.detail.empty() ? std::string{"sheen::exception"} : e.detail; }

  error error_;
};

}  // namespace sheen

#endif  // SHEEN_CORE_EXCEPTION_HPP
