#include <sheen/archives/json.hpp>
#include <sheen/core/exception.hpp>
#include <sheen/core/policy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("json_input_archive reads scalars at the root")
{
  SECTION("int")
  {
    sheen::json_input_archive ar{"42"};
    int v{};
    ar(v);
    CHECK(v == 42);
  }
  SECTION("negative int")
  {
    sheen::json_input_archive ar{"-7"};
    int v{};
    ar(v);
    CHECK(v == -7);
  }
  SECTION("bool")
  {
    sheen::json_input_archive ar{"true"};
    bool v{};
    ar(v);
    CHECK(v == true);
  }
  SECTION("string")
  {
    sheen::json_input_archive ar{R"("hello")"};
    std::string v;
    ar(v);
    CHECK(v == "hello");
  }
}

TEST_CASE("json_input_archive reads a flat object via nvp")
{
  sheen::json_input_archive ar{R"({"age":30,"active":true,"name":"Alice"})"};
  int age{};
  bool active{};
  std::string name;
  ar(sheen::nvp("age", age));
  ar(sheen::nvp("active", active));
  ar(sheen::nvp("name", name));
  CHECK(age == 30);
  CHECK(active);
  CHECK(name == "Alice");
}

namespace ex {

struct point {
  int x;
  int y;

  template<class A>
  constexpr void serialize(A& ar)
  {
    ar(sheen::nvp("x", x));
    ar(sheen::nvp("y", y));
  }
};

}  // namespace ex

TEST_CASE("json_input_archive drives a user type's intrusive serialize")
{
  sheen::json_input_archive ar{R"({"x":3,"y":4})"};
  ex::point p{};
  p.serialize(ar);
  CHECK(p.x == 3);
  CHECK(p.y == 4);
}

TEST_CASE("json_input_archive throws on missing key")
{
  sheen::json_input_archive ar{R"({"age":30})"};
  std::string name;
  CHECK_THROWS_AS(ar(sheen::nvp("name", name)), sheen::exception);
}

TEST_CASE("json_input_archive throws on type mismatch")
{
  sheen::json_input_archive ar{R"({"value":"not_an_int"})"};
  int v{};
  CHECK_THROWS_AS(ar(sheen::nvp("value", v)), sheen::exception);
}

TEST_CASE("json_input_archive is constexpr-clean with json20")
{
  STATIC_CHECK([] {
    sheen::json_input_archive ar{R"({"x":3,"y":4})"};
    ex::point p{};
    p.serialize(ar);
    return p.x == 3 && p.y == 4;
  }());
}
