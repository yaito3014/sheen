#ifndef SHEEN_ARCHIVES_JSON_HPP
#define SHEEN_ARCHIVES_JSON_HPP

#include <sheen/core/dispatch.hpp>
#include <sheen/core/dom.hpp>
#include <sheen/core/error.hpp>
#include <sheen/core/exception.hpp>
#include <sheen/core/policy.hpp>
#include <sheen/json/default_parser.hpp>

#include <slot/slot.hpp>

#include <charconv>
#include <string>
#include <string_view>
#include <utility>

namespace sheen {

class json_output_archive {
public:
  constexpr explicit json_output_archive(std::string& sink) noexcept : sink_{sink} {}

  json_output_archive(json_output_archive const&) = delete;
  json_output_archive(json_output_archive&&) = delete;
  json_output_archive& operator=(json_output_archive const&) = delete;
  json_output_archive& operator=(json_output_archive&&) = delete;

  constexpr ~json_output_archive()
  {
    if (object_open_) sink_.push_back('}');
  }

  template<class T>
  constexpr void operator()(slot::binding<nvp_policy, T> const& kv)
  {
    open_or_separate();
    sink_.push_back('"');
    sink_.append(kv.policy().name);
    sink_.append("\":");
    (*this)(kv.get());
  }

  constexpr void operator()(int v)
  {
    char buf[12];
    auto const r = std::to_chars(buf, buf + sizeof(buf), v);
    sink_.append(buf, r.ptr);
  }

  constexpr void operator()(bool v) { sink_.append(v ? "true" : "false"); }

  constexpr void operator()(std::string_view v)
  {
    sink_.push_back('"');
    for (char c : v) {
      switch (c) {
      case '"':  sink_.append("\\\""); break;
      case '\\': sink_.append("\\\\"); break;
      case '\b': sink_.append("\\b"); break;
      case '\f': sink_.append("\\f"); break;
      case '\n': sink_.append("\\n"); break;
      case '\r': sink_.append("\\r"); break;
      case '\t': sink_.append("\\t"); break;
      default:   sink_.push_back(c); break;
      }
    }
    sink_.push_back('"');
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
  constexpr void open_or_separate()
  {
    if (object_open_) {
      sink_.push_back(',');
    } else {
      sink_.push_back('{');
      object_open_ = true;
    }
  }

  std::string& sink_;
  bool object_open_ = false;
};

#if defined(SHEEN_JSON_HAS_DEFAULT_PARSER)
template<dom::tree_parser P = json::default_parser>
#else
template<dom::tree_parser P>
#endif
class json_input_archive {
public:
  using parser_type = P;
  using node_type = typename P::node_type;

  constexpr explicit json_input_archive(std::string_view source, P parser = P{}) : parser_(std::move(parser)), root_(parse_or_throw(parser_, source)), current_(&root_) {}

  json_input_archive(json_input_archive const&) = delete;
  json_input_archive(json_input_archive&&) = delete;
  json_input_archive& operator=(json_input_archive const&) = delete;
  json_input_archive& operator=(json_input_archive&&) = delete;

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

#endif  // SHEEN_ARCHIVES_JSON_HPP
