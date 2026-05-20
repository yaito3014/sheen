#include <sheen/core/error.hpp>

#include <catch2/catch_test_macros.hpp>

#include <source_location>
#include <string_view>
#include <type_traits>
#include <utility>

TEST_CASE("error shape")
{
  SECTION("error_code is an enum") { STATIC_CHECK(std::is_enum_v<sheen::error_code>); }
  SECTION("default-constructible") { STATIC_CHECK(std::is_default_constructible_v<sheen::error>); }
  SECTION("constructible from error_code alone") { STATIC_CHECK(std::is_constructible_v<sheen::error, sheen::error_code>); }
  SECTION("constructible from error_code + detail") { STATIC_CHECK(std::is_constructible_v<sheen::error, sheen::error_code, std::string>); }
  SECTION("nothrow-move-constructible (cheap to carry through std::expected)") { STATIC_CHECK(std::is_nothrow_move_constructible_v<sheen::error>); }
}

// std::string's storage can't escape constant evaluation; the IILE pattern is
// what try_serialize / try_deserialize use to stay constexpr-clean.
TEST_CASE("error is constexpr-usable")
{
  STATIC_CHECK([] {
    sheen::error e{sheen::error_code::parse_error, "bad token"};
    return e.code == sheen::error_code::parse_error && e.detail == "bad token";
  }());

  STATIC_CHECK([] {
    sheen::error e{};
    return e.detail.empty();
  }());
}

TEST_CASE("error captures the construction site by default")
{
  auto const here = std::source_location::current();
  sheen::error e{sheen::error_code::io_failure, "disk full"};
  CHECK(e.code == sheen::error_code::io_failure);
  CHECK(e.detail == "disk full");
  CHECK(e.where.line() == here.line() + 1);
  CHECK(std::string_view{e.where.file_name()} == here.file_name());
}

TEST_CASE("default-constructed error also captures its construction site")
{
  auto const here = std::source_location::current();
  sheen::error e{};
  CHECK(e.where.line() == here.line() + 1);
  CHECK(std::string_view{e.where.file_name()} == here.file_name());
}

TEST_CASE("move-construction transfers code and detail")
{
  sheen::error src{sheen::error_code::missing_key, "id"};
  sheen::error dst = std::move(src);
  CHECK(dst.code == sheen::error_code::missing_key);
  CHECK(dst.detail == "id");
}
