#ifndef SHEEN_YAML_PARSERS_FKYAML_HPP
#define SHEEN_YAML_PARSERS_FKYAML_HPP

#include <sheen/core/dom.hpp>
#include <sheen/core/error.hpp>

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
  explicit fkyaml_node(fkyaml::node&& doc) : owned_(std::make_unique<fkyaml::node>(std::move(doc))), node_(owned_.get()) {}

  explicit fkyaml_node(fkyaml::node const& view) noexcept : node_(&view) {}

  fkyaml_node(fkyaml_node const&) = delete;
  fkyaml_node& operator=(fkyaml_node const&) = delete;

  fkyaml_node(fkyaml_node&& other) noexcept : owned_(std::move(other.owned_)), node_(owned_ ? owned_.get() : other.node_) { other.node_ = nullptr; }

  fkyaml_node& operator=(fkyaml_node&& other) noexcept
  {
    owned_ = std::move(other.owned_);
    node_ = owned_ ? owned_.get() : other.node_;
    other.node_ = nullptr;
    return *this;
  }

  bool is_null() const noexcept { return node_->is_null(); }
  bool is_boolean() const noexcept { return node_->is_boolean(); }
  bool is_integer() const noexcept { return node_->is_integer(); }
  bool is_float() const noexcept { return node_->is_float_number(); }
  bool is_string() const noexcept { return node_->is_string(); }
  bool is_list() const noexcept { return node_->is_sequence(); }
  bool is_record() const noexcept { return node_->is_mapping(); }

  bool as_boolean() const { return node_->get_value<bool>(); }
  std::int64_t as_integer() const { return node_->get_value<std::int64_t>(); }
  double as_float() const { return node_->get_value<double>(); }
  std::string_view as_string() const { return node_->get_value_ref<std::string const&>(); }

  std::size_t size() const { return node_->size(); }

  bool contains(std::string_view key) const { return node_->contains(std::string{key}); }

  fkyaml_node operator[](std::size_t i) const { return fkyaml_node{(*node_)[static_cast<int>(i)]}; }

  fkyaml_node operator[](std::string_view key) const { return fkyaml_node{(*node_)[std::string{key}]}; }

private:
  std::unique_ptr<fkyaml::node> owned_;
  fkyaml::node const* node_ = nullptr;
};

class fkyaml_parser {
public:
  using node_type = fkyaml_node;

  std::expected<fkyaml_node, error> parse(std::string_view src) const
  {
    try {
      return fkyaml_node{fkyaml::node::deserialize(std::string{src})};
    } catch (fkyaml::exception const& e) {
      return std::unexpected(error{error_code::parse_error, e.what()});
    } catch (std::exception const& e) {
      return std::unexpected(error{error_code::parse_error, e.what()});
    }
  }
};

static_assert(dom::tree_parser<fkyaml_parser>);

}  // namespace sheen::yaml

#endif  // SHEEN_YAML_PARSERS_FKYAML_HPP
