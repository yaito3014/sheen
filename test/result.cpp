#include <sheen/core/error.hpp>
#include <sheen/core/result.hpp>

#include <catch2/catch_test_macros.hpp>

#include <expected>
#include <string_view>
#include <type_traits>
#include <utility>

TEST_CASE("result shape")
{
  STATIC_CHECK(std::is_same_v<sheen::result<int>, std::expected<int, sheen::error>>);
  STATIC_CHECK(std::is_same_v<sheen::result<void>, std::expected<void, sheen::error>>);
}

TEST_CASE("result is constexpr-usable")
{
  STATIC_CHECK([] {
    sheen::result<int> r = 42;
    return r.has_value() && *r == 42;
  }());

  STATIC_CHECK([] {
    sheen::result<int> r = std::unexpected{sheen::error{sheen::error_code::parse_error, "bad"}};
    return !r.has_value() && r.error().code == sheen::error_code::parse_error && std::string_view{r.error().detail} == "bad";
  }());

  STATIC_CHECK([] {
    sheen::result<void> r{};
    return r.has_value();
  }());
}

TEST_CASE("result holds either a value or an error")
{
  sheen::result<int> ok = 7;
  REQUIRE(ok.has_value());
  CHECK(*ok == 7);

  sheen::result<int> bad = std::unexpected{sheen::error{sheen::error_code::missing_key, "id"}};
  REQUIRE_FALSE(bad.has_value());
  CHECK(bad.error().code == sheen::error_code::missing_key);
  CHECK(bad.error().detail == "id");
}
