#ifndef SHEEN_YAML_PARSER_HPP
#define SHEEN_YAML_PARSER_HPP

#include <sheen/core/error.hpp>

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <string_view>

namespace sheen::yaml {

template<class N>
concept yaml_node = requires(N const& n, std::size_t i, std::string_view key) {
  { n.is_null() } -> std::convertible_to<bool>;
  { n.is_boolean() } -> std::convertible_to<bool>;
  { n.is_integer() } -> std::convertible_to<bool>;
  { n.is_float() } -> std::convertible_to<bool>;
  { n.is_string() } -> std::convertible_to<bool>;
  { n.is_sequence() } -> std::convertible_to<bool>;
  { n.is_mapping() } -> std::convertible_to<bool>;

  { n.as_boolean() } -> std::convertible_to<bool>;
  { n.as_integer() } -> std::convertible_to<std::int64_t>;
  { n.as_float() } -> std::convertible_to<double>;
  { n.as_string() } -> std::convertible_to<std::string_view>;

  { n.size() } -> std::convertible_to<std::size_t>;
  { n.contains(key) } -> std::convertible_to<bool>;
  { n[i] };
  { n[key] };
};

template<class P>
concept yaml_parser = requires(P& p, std::string_view src) {
  typename P::node_type;
  requires yaml_node<typename P::node_type>;
  { p.parse(src) } -> std::same_as<std::expected<typename P::node_type, error>>;
};

}  // namespace sheen::yaml

#endif  // SHEEN_YAML_PARSER_HPP
