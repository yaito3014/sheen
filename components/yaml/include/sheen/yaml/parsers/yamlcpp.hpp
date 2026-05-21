#ifndef SHEEN_YAML_PARSERS_YAMLCPP_HPP
#define SHEEN_YAML_PARSERS_YAMLCPP_HPP

#include <sheen/core/dom.hpp>
#include <sheen/core/error.hpp>

#include <yaml-cpp/yaml.h>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <expected>
#include <string>
#include <string_view>
#include <utility>

namespace sheen::yaml {

class yamlcpp_node {
public:
  explicit yamlcpp_node(YAML::Node node) noexcept : node_(std::move(node)) {}

  bool is_null() const noexcept { return node_.IsNull(); }

  bool is_boolean() const noexcept
  {
    if (!node_.IsScalar()) return false;
    bool tmp{};
    return YAML::convert<bool>::decode(node_, tmp);
  }

  bool is_integer() const noexcept
  {
    if (!node_.IsScalar()) return false;
    std::int64_t tmp{};
    return YAML::convert<std::int64_t>::decode(node_, tmp);
  }

  bool is_float() const noexcept
  {
    if (!node_.IsScalar()) return false;
    double tmp{};
    return YAML::convert<double>::decode(node_, tmp);
  }

  bool is_string() const noexcept { return node_.IsScalar(); }
  bool is_list() const noexcept { return node_.IsSequence(); }
  bool is_record() const noexcept { return node_.IsMap(); }

  bool as_boolean() const { return node_.as<bool>(); }
  std::int64_t as_integer() const { return node_.as<std::int64_t>(); }
  double as_float() const { return node_.as<double>(); }
  std::string_view as_string() const { return node_.Scalar(); }

  std::size_t size() const noexcept { return node_.size(); }

  bool contains(std::string_view key) const { return node_[std::string{key}].IsDefined(); }

  yamlcpp_node operator[](std::size_t i) const { return yamlcpp_node{node_[i]}; }
  yamlcpp_node operator[](std::string_view key) const { return yamlcpp_node{node_[std::string{key}]}; }

private:
  YAML::Node node_;
};

class yamlcpp_parser {
public:
  using node_type = yamlcpp_node;

  std::expected<yamlcpp_node, error> parse(std::string_view src) const
  {
    try {
      return yamlcpp_node{YAML::Load(std::string{src})};
    } catch (YAML::Exception const& e) {
      return std::unexpected(error{error_code::parse_error, e.what()});
    } catch (std::exception const& e) {
      return std::unexpected(error{error_code::parse_error, e.what()});
    }
  }
};

static_assert(dom::tree_parser<yamlcpp_parser>);

}  // namespace sheen::yaml

#endif  // SHEEN_YAML_PARSERS_YAMLCPP_HPP
