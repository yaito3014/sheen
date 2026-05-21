#ifndef SHEEN_ARCHIVES_YAML_HPP
#define SHEEN_ARCHIVES_YAML_HPP

#include <sheen/core/dispatch.hpp>
#include <sheen/core/dom.hpp>
#include <sheen/core/error.hpp>
#include <sheen/core/exception.hpp>
#include <sheen/core/policy.hpp>
#include <sheen/yaml/default_parser.hpp>

#include <slot/slot.hpp>

#include <charconv>
#include <string>
#include <string_view>
#include <utility>

namespace sheen {

class yaml_output_archive {
public:
  constexpr explicit yaml_output_archive(std::string& sink) noexcept : sink_{sink} {}

  template<class T>
  constexpr void operator()(slot::binding<nvp_policy, T> const& kv)
  {
    sink_.append(kv.policy().name);
    sink_.append(": ");
    (*this)(kv.get());
  }

  template<class T>
  constexpr void operator()(slot::binding<comment_policy, T> const& kv)
  {
    sink_.append("# ");
    sink_.append(kv.policy().text);
    sink_.push_back('\n');
    (*this)(kv.get());
  }

  constexpr void operator()(int v)
  {
    char buf[12];
    auto const r = std::to_chars(buf, buf + sizeof(buf), v);
    sink_.append(buf, r.ptr);
    sink_.push_back('\n');
  }

  constexpr void operator()(bool v)
  {
    sink_.append(v ? "true" : "false");
    sink_.push_back('\n');
  }

  constexpr void operator()(std::string_view v)
  {
    sink_.push_back('"');
    sink_.append(v);
    sink_.push_back('"');
    sink_.push_back('\n');
  }

  template<class T>
    requires serializable<T>
  constexpr void operator()(T const& v) { save_to(*this, v); }

  template<class... Ts>
    requires (sizeof...(Ts) > 1)
  constexpr void operator()(Ts&&... values)
  {
    (operator()(static_cast<Ts&&>(values)), ...);
  }

private:
  std::string& sink_;
};

#if defined(SHEEN_YAML_HAS_DEFAULT_PARSER)
template<dom::tree_parser P = yaml::default_parser>
#else
template<dom::tree_parser P>
#endif
class yaml_input_archive {
public:
  using parser_type = P;
  using node_type = typename P::node_type;

  constexpr explicit yaml_input_archive(std::string_view source, P parser = P{}) : parser_(std::move(parser)), root_(parse_or_throw(parser_, source)), current_(&root_) {}

  yaml_input_archive(yaml_input_archive const&) = delete;
  yaml_input_archive(yaml_input_archive&&) = delete;
  yaml_input_archive& operator=(yaml_input_archive const&) = delete;
  yaml_input_archive& operator=(yaml_input_archive&&) = delete;

  constexpr void operator()(int& dst)
  {
    require(current_->is_integer(), error_code::type_mismatch, "expected integer");
    dst = static_cast<int>(current_->as_integer());
  }

  constexpr void operator()(bool& dst)
  {
    require(current_->is_boolean(), error_code::type_mismatch, "expected boolean");
    dst = current_->as_boolean();
  }

  constexpr void operator()(std::string& dst)
  {
    require(current_->is_string(), error_code::type_mismatch, "expected string");
    dst = current_->as_string();
  }

  template<class T>
  constexpr void operator()(slot::binding<nvp_policy, T&> const& kv)
  {
    require(current_->is_record(), error_code::type_mismatch, "expected record");
    require(current_->contains(kv.policy().name), error_code::missing_key, kv.policy().name);

    node_type child = (*current_)[kv.policy().name];
    node_type* const saved = current_;
    current_ = &child;
    try {
      (*this)(kv.get());
      current_ = saved;
    } catch (...) {
      current_ = saved;
      throw;
    }
  }

  template<class T>
    requires deserializable<T&>
  constexpr void operator()(T& v) { load_from(*this, v); }

  template<class... Ts>
    requires (sizeof...(Ts) > 1)
  constexpr void operator()(Ts&&... values)
  {
    (operator()(static_cast<Ts&&>(values)), ...);
  }

private:
  static constexpr node_type parse_or_throw(P& parser, std::string_view source)
  {
    auto r = parser.parse(source);
    if (!r) throw exception(r.error());
    return std::move(*r);
  }

  static constexpr void require(bool cond, error_code code, std::string_view detail)
  {
    if (!cond) throw exception(error{code, detail});
  }

  P parser_;
  node_type root_;
  node_type* current_;
};

}  // namespace sheen

#endif  // SHEEN_ARCHIVES_YAML_HPP
