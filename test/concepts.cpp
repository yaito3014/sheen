#include <sheen/core/concepts.hpp>

#include <slot/slot.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <type_traits>

namespace {

struct fake_bool_proxy {
  bool* p_ = nullptr;
  constexpr fake_bool_proxy const& operator=(bool b) const noexcept
  {
    if (p_) *p_ = b;
    return *this;
  }
  constexpr operator bool() const noexcept { return p_ ? *p_ : false; }
};

}  // namespace

template<>
struct sheen::input_value<fake_bool_proxy> : std::type_identity<bool> {};

namespace {

struct fake_output_archive {
  void operator()(int const&) {}
  void operator()(int&) = delete;
};

struct fake_input_archive {
  void operator()(int&) {}
};

struct fake_bidi_archive {
  void operator()(int const&) {}
  void operator()(int&) {}
};

struct fake_proxy_input_archive {
  void operator()(fake_bool_proxy&&) {}
};

struct fake_not_archive {};

struct fake_universal_output_archive {
  template<class T>
  void operator()(T const&)
  {
  }
};

struct fake_string_only_archive {
  void operator()(std::string const&) {}
};

}  // namespace

TEST_CASE("writable")
{
  STATIC_CHECK(sheen::writable<int&, int>);
  STATIC_CHECK(sheen::writable<int&, int const&>);
  STATIC_CHECK(!sheen::writable<int const&, int>);
  STATIC_CHECK(!sheen::writable<int, int>);
  STATIC_CHECK(sheen::writable<int&, int> == slot::writable<int&, int>);
}

TEST_CASE("input_value_t")
{
  STATIC_CHECK(std::is_same_v<sheen::input_value_t<int&>, int>);
  STATIC_CHECK(std::is_same_v<sheen::input_value_t<int const&>, int>);
  STATIC_CHECK(std::is_same_v<sheen::input_value_t<std::string&>, std::string>);
  STATIC_CHECK(std::is_same_v<sheen::input_value_t<fake_bool_proxy>, bool>);
  STATIC_CHECK(std::is_same_v<sheen::input_value_t<fake_bool_proxy&&>, bool>);
}

TEST_CASE("output_archive_for")
{
  STATIC_CHECK(sheen::output_archive_for<fake_output_archive, int>);
  STATIC_CHECK(sheen::output_archive_for<fake_bidi_archive, int>);
  STATIC_CHECK(!sheen::output_archive_for<fake_input_archive, int>);
  STATIC_CHECK(!sheen::output_archive_for<fake_not_archive, int>);
  STATIC_CHECK(!sheen::output_archive_for<int, int>);

  STATIC_CHECK(sheen::output_archive_for<fake_universal_output_archive, int>);
  STATIC_CHECK(sheen::output_archive_for<fake_universal_output_archive, double>);
  STATIC_CHECK(sheen::output_archive_for<fake_universal_output_archive, std::string>);

  STATIC_CHECK(!sheen::output_archive_for<fake_string_only_archive, int>);
  STATIC_CHECK(sheen::output_archive_for<fake_string_only_archive, std::string>);
}

TEST_CASE("input_archive_for")
{
  STATIC_CHECK(sheen::input_archive_for<fake_input_archive, int&>);
  STATIC_CHECK(!sheen::input_archive_for<fake_input_archive, int>);
  STATIC_CHECK(!sheen::input_archive_for<fake_input_archive, int const&>);
  STATIC_CHECK(!sheen::input_archive_for<fake_output_archive, int&>);
  STATIC_CHECK(sheen::input_archive_for<fake_bidi_archive, int&>);

  STATIC_CHECK(sheen::input_archive_for<fake_proxy_input_archive, fake_bool_proxy>);
  STATIC_CHECK(!sheen::input_archive_for<fake_input_archive, fake_bool_proxy>);
}
