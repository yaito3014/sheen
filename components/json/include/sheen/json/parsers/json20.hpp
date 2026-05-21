#ifndef SHEEN_JSON_PARSERS_JSON20_HPP
#define SHEEN_JSON_PARSERS_JSON20_HPP

#include <sheen/core/dom.hpp>
#include <sheen/core/error.hpp>

#include <yk/json20.hpp>

#include <cstddef>
#include <cstdint>
#include <expected>
#include <memory>
#include <string_view>
#include <utility>

namespace sheen::json {

class json20_node {
public:
  constexpr explicit json20_node(yk::json20::json&& doc) : owned_(std::make_unique<yk::json20::json>(std::move(doc))), node_(owned_.get()) {}

  constexpr explicit json20_node(yk::json20::json const& view) noexcept : node_(&view) {}

  json20_node(json20_node const&) = delete;
  json20_node& operator=(json20_node const&) = delete;

  constexpr json20_node(json20_node&& other) noexcept : owned_(std::move(other.owned_)), node_(owned_ ? owned_.get() : other.node_) { other.node_ = nullptr; }

  constexpr json20_node& operator=(json20_node&& other) noexcept
  {
    owned_ = std::move(other.owned_);
    node_ = owned_ ? owned_.get() : other.node_;
    other.node_ = nullptr;
    return *this;
  }

  constexpr bool is_null() const noexcept { return node_->get_kind() == yk::json20::json_value_kind::null; }
  constexpr bool is_boolean() const noexcept { return node_->get_kind() == yk::json20::json_value_kind::boolean; }

  constexpr bool is_integer() const noexcept
  {
    auto const k = node_->get_kind();
    return k == yk::json20::json_value_kind::number_signed_integer || k == yk::json20::json_value_kind::number_unsigned_integer;
  }

  constexpr bool is_float() const noexcept { return node_->get_kind() == yk::json20::json_value_kind::number_floating_point; }
  constexpr bool is_string() const noexcept { return node_->get_kind() == yk::json20::json_value_kind::string; }
  constexpr bool is_list() const noexcept { return node_->get_kind() == yk::json20::json_value_kind::array; }
  constexpr bool is_record() const noexcept { return node_->get_kind() == yk::json20::json_value_kind::object; }

  constexpr bool as_boolean() const { return node_->as_boolean(); }

  constexpr std::int64_t as_integer() const
  {
    if (node_->get_kind() == yk::json20::json_value_kind::number_signed_integer) return node_->as_signed_integer<std::int64_t>();
    return static_cast<std::int64_t>(node_->as_unsigned_integer<std::uint64_t>());
  }

  constexpr double as_float() const { return node_->as_floating_point<double>(); }
  constexpr std::string_view as_string() const { return node_->as_string(); }

  constexpr std::size_t size() const
  {
    auto const k = node_->get_kind();
    if (k == yk::json20::json_value_kind::array) return node_->as_array().size();
    if (k == yk::json20::json_value_kind::object) return node_->as_object().size();
    return 0;
  }

  constexpr bool contains(std::string_view key) const { return node_->try_at(key).has_value(); }

  constexpr json20_node operator[](std::size_t i) const { return json20_node{node_->at(i)}; }
  constexpr json20_node operator[](std::string_view key) const { return json20_node{node_->at(key)}; }

private:
  std::unique_ptr<yk::json20::json> owned_;
  yk::json20::json const* node_ = nullptr;
};

class json20_parser {
public:
  using node_type = json20_node;

  constexpr std::expected<json20_node, error> parse(std::string_view src) const
  {
    auto opt = yk::json20::json_parser::try_parse(src);
    if (!opt) return std::unexpected(error{error_code::parse_error, "invalid JSON"});
    return json20_node{std::move(*opt)};
  }
};

static_assert(dom::tree_parser<json20_parser>);

}  // namespace sheen::json

#endif  // SHEEN_JSON_PARSERS_JSON20_HPP
