#ifndef SHEEN_CORE_DISPATCH_HPP
#define SHEEN_CORE_DISPATCH_HPP

#include <sheen/core/concepts.hpp>

namespace sheen {

namespace detail {

template<class A, class T>
concept exactly_one_output_cp =
    (has_output_member_save<A, T> + has_output_member_serialize<A, T> + has_output_free_save<A, T> + has_output_free_serialize<A, T>) == 1;

template<class A, class T>
concept exactly_one_input_cp =
    (has_input_member_load<A, T> + has_input_member_serialize<A, T> + has_input_free_load<A, T> + has_input_free_serialize<A, T>) == 1;

}  // namespace detail

namespace cpo {

struct save_to_fn {
  template<class A, class T>
    requires detail::exactly_one_output_cp<A, T>
  constexpr void operator()(A& ar, T const& v) const
  {
    if constexpr (detail::has_output_member_save<A, T>) {
      v.save(ar);
    } else if constexpr (detail::has_output_member_serialize<A, T>) {
      v.serialize(ar);
    } else if constexpr (detail::has_output_free_save<A, T>) {
      save(ar, v);
    } else {
      serialize(ar, v);
    }
  }
};

struct load_from_fn {
  template<class A, class T>
    requires detail::exactly_one_input_cp<A, T>
  constexpr void operator()(A& ar, T&& v) const
  {
    if constexpr (detail::has_input_member_load<A, T>) {
      static_cast<T&&>(v).load(ar);
    } else if constexpr (detail::has_input_member_serialize<A, T>) {
      static_cast<T&&>(v).serialize(ar);
    } else if constexpr (detail::has_input_free_load<A, T>) {
      load(ar, static_cast<T&&>(v));
    } else {
      serialize(ar, static_cast<T&&>(v));
    }
  }
};

}  // namespace cpo

inline constexpr cpo::save_to_fn save_to{};
inline constexpr cpo::load_from_fn load_from{};

template<class T>
concept serializable = requires(detail::dummy_output_archive& ar, T const& v) { save_to(ar, v); };

template<class T>
concept deserializable = requires(detail::dummy_input_archive& ar, T&& v) { load_from(ar, static_cast<T&&>(v)); };

}  // namespace sheen

#endif  // SHEEN_CORE_DISPATCH_HPP
