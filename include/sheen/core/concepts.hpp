#ifndef SHEEN_CORE_CONCEPTS_HPP
#define SHEEN_CORE_CONCEPTS_HPP

#include <slot/slot.hpp>

#include <type_traits>

namespace sheen {

template<class Destination, class Source>
concept writable = slot::writable<Destination, Source>;

// std::string::value_type is char, so nested-value_type auto-detection is
// wrong for containers. Proxies opt in by specialising this.
template<class T>
struct input_value : std::type_identity<std::remove_cvref_t<T>> {};

template<class T>
using input_value_t = typename input_value<std::remove_cvref_t<T>>::type;

template<class A, class T>
concept output_archive_for = requires(A& a, T const& v) { a(v); };

template<class A, class T>
concept input_archive_for = writable<T, input_value_t<T>> && requires(A& a, T&& v) { a(static_cast<T&&>(v)); };

namespace detail {

struct dummy_output_archive {
  template<class T>
  constexpr void operator()(T const&) const noexcept
  {
  }
};

struct dummy_input_archive {
  template<class T>
  constexpr void operator()(T&&) const noexcept
  {
  }
};

template<class A, class T>
concept has_output_member_save = requires(A& a, T const& v) { v.save(a); };

template<class A, class T>
concept has_output_free_save = requires(A& a, T const& v) { save(a, v); };

template<class A, class T>
concept has_output_member_serialize = requires(A& a, T const& v) { v.serialize(a); };

template<class A, class T>
concept has_output_free_serialize = requires(A& a, T const& v) { serialize(a, v); };

template<class A, class T>
concept has_input_member_load = writable<T, input_value_t<T>> && requires(A& a, T&& v) { static_cast<T&&>(v).load(a); };

template<class A, class T>
concept has_input_free_load = writable<T, input_value_t<T>> && requires(A& a, T&& v) { load(a, static_cast<T&&>(v)); };

template<class A, class T>
concept has_input_member_serialize = writable<T, input_value_t<T>> && requires(A& a, T&& v) { static_cast<T&&>(v).serialize(a); };

template<class A, class T>
concept has_input_free_serialize = writable<T, input_value_t<T>> && requires(A& a, T&& v) { serialize(a, static_cast<T&&>(v)); };

}  // namespace detail

}  // namespace sheen

#endif  // SHEEN_CORE_CONCEPTS_HPP
