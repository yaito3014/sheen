#ifndef SHEEN_CORE_POLICY_HPP
#define SHEEN_CORE_POLICY_HPP

#include <slot/slot.hpp>

#include <string_view>
#include <utility>

namespace sheen {

struct nvp_policy {
  std::string_view name;
};

struct comment_policy {
  std::string_view text;
};

namespace detail {

template<class T>
constexpr T&& wrap_impl(T&& target) noexcept
{
  return std::forward<T>(target);
}

template<class T, class P0, class... Rest>
constexpr auto wrap_impl(T&& target, P0 p0, Rest... rest)
{
  return wrap_impl(slot::into(std::forward<T>(target), std::move(p0)), std::move(rest)...);
}

}  // namespace detail

// First policy is innermost; last is outermost (what the archive sees first).
template<class T, class... Policies>
[[nodiscard]] constexpr auto wrap(T&& target, Policies... policies)
{
  return detail::wrap_impl(std::forward<T>(target), std::move(policies)...);
}

template<class T>
[[nodiscard]] constexpr auto nvp(std::string_view name, T&& value)
{
  return slot::into(std::forward<T>(value), nvp_policy{name});
}

}  // namespace sheen

#endif  // SHEEN_CORE_POLICY_HPP
