#ifndef SHEEN_YAML_PARSERS_FKYAML_HPP
#define SHEEN_YAML_PARSERS_FKYAML_HPP

#include <sheen/core/error.hpp>
#include <sheen/yaml/parser.hpp>

#include <fkYAML/exception.hpp>
#include <fkYAML/node.hpp>

#include <cstddef>
#include <cstdint>
#include <exception>
#include <expected>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

namespace sheen::yaml {

class fkyaml_node {
public:
  fkyaml_node(std::shared_ptr<fkyaml::node const> doc, fkyaml::node const* node) noexcept : doc_(std::move(doc)), node_(node) {}

  bool is_null() const noexcept { return node_->is_null(); }
  bool is_boolean() const noexcept { return node_->is_boolean(); }
  bool is_integer() const noexcept { return node_->is_integer(); }
  bool is_float() const noexcept { return node_->is_float_number(); }
  bool is_string() const noexcept { return node_->is_string(); }
  bool is_sequence() const noexcept { return node_->is_sequence(); }
  bool is_mapping() const noexcept { return node_->is_mapping(); }

  bool as_boolean() const { return node_->get_value<bool>(); }
  std::int64_t as_integer() const { return node_->get_value<std::int64_t>(); }
  double as_float() const { return node_->get_value<double>(); }
  std::string_view as_string() const { return node_->get_value_ref<std::string const&>(); }

  std::size_t size() const { return node_->size(); }

  bool contains(std::string_view key) const { return node_->contains(std::string{key}); }

  fkyaml_node operator[](std::size_t i) const { return fkyaml_node{doc_, &(*node_)[static_cast<int>(i)]}; }

  fkyaml_node operator[](std::string_view key) const { return fkyaml_node{doc_, &(*node_)[std::string{key}]}; }

private:
  std::shared_ptr<fkyaml::node const> doc_;
  fkyaml::node const* node_;
};

class fkyaml_parser {
public:
  using node_type = fkyaml_node;

  std::expected<fkyaml_node, error> parse(std::string_view src) const
  {
    try {
      auto doc = std::make_shared<fkyaml::node>(fkyaml::node::deserialize(std::string{src}));
      auto* root = doc.get();
      return fkyaml_node{std::shared_ptr<fkyaml::node const>{std::move(doc)}, root};
    } catch (fkyaml::exception const& e) {
      return std::unexpected(error{error_code::parse_error, e.what()});
    } catch (std::exception const& e) {
      return std::unexpected(error{error_code::parse_error, e.what()});
    }
  }
};

static_assert(yaml_parser<fkyaml_parser>);

}  // namespace sheen::yaml

#endif  // SHEEN_YAML_PARSERS_FKYAML_HPP
