#include <sheen/core/error.hpp>
#include <sheen/core/exception.hpp>

#include <catch2/catch_test_macros.hpp>

#include <stdexcept>
#include <string_view>
#include <type_traits>

TEST_CASE("exception shape")
{
  STATIC_CHECK(std::is_base_of_v<std::runtime_error, sheen::exception>);
  STATIC_CHECK(std::is_constructible_v<sheen::exception, sheen::error>);
  STATIC_CHECK(!std::is_trivially_destructible_v<sheen::exception>);
}

TEST_CASE("exception wraps an error and exposes it via info()")
{
  sheen::exception exc{sheen::error{sheen::error_code::parse_error, "bad token"}};
  CHECK(exc.info().code == sheen::error_code::parse_error);
  CHECK(exc.info().detail == "bad token");
}

TEST_CASE("exception::what() surfaces the error detail")
{
  sheen::exception exc{sheen::error{sheen::error_code::io_failure, "disk full"}};
  CHECK(std::string_view{exc.what()} == "disk full");
}

TEST_CASE("exception with empty detail still has a non-empty what()")
{
  sheen::exception exc{sheen::error{sheen::error_code::missing_key}};
  CHECK_FALSE(std::string_view{exc.what()}.empty());
}

TEST_CASE("exception is throwable and catchable as std::runtime_error")
{
  try {
    throw sheen::exception{sheen::error{sheen::error_code::out_of_range, "n=99"}};
  } catch (std::runtime_error const& re) {
    CHECK(std::string_view{re.what()} == "n=99");
    auto* exc = dynamic_cast<sheen::exception const*>(&re);
    REQUIRE(exc != nullptr);
    CHECK(exc->info().code == sheen::error_code::out_of_range);
  }
}
