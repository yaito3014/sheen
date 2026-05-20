#include <sheen/archives/yaml.hpp>
#include <sheen/core/exception.hpp>
#include <sheen/core/policy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

TEST_CASE("yaml_input_archive reads scalars at the root")
{
  SECTION("int")
  {
    sheen::yaml_input_archive ar{"42"};
    int v{};
    ar(v);
    CHECK(v == 42);
  }
  SECTION("bool")
  {
    sheen::yaml_input_archive ar{"true"};
    bool v{};
    ar(v);
    CHECK(v == true);
  }
  SECTION("string")
  {
    sheen::yaml_input_archive ar{"hello"};
    std::string v;
    ar(v);
    CHECK(v == "hello");
  }
}

TEST_CASE("yaml_input_archive reads a flat mapping via nvp")
{
  sheen::yaml_input_archive ar{"age: 30\nactive: true\nname: Alice"};
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
  void serialize(A& ar)
  {
    ar(sheen::nvp("x", x));
    ar(sheen::nvp("y", y));
  }
};

}  // namespace ex

TEST_CASE("yaml_input_archive drives a user type's intrusive serialize")
{
  sheen::yaml_input_archive ar{"x: 3\ny: 4"};
  ex::point p{};
  p.serialize(ar);
  CHECK(p.x == 3);
  CHECK(p.y == 4);
}

TEST_CASE("yaml_input_archive throws on missing key")
{
  sheen::yaml_input_archive ar{"age: 30"};
  std::string name;
  CHECK_THROWS_AS(ar(sheen::nvp("name", name)), sheen::exception);
}

TEST_CASE("yaml_input_archive throws on type mismatch")
{
  sheen::yaml_input_archive ar{"value: not_an_int"};
  int v{};
  CHECK_THROWS_AS(ar(sheen::nvp("value", v)), sheen::exception);
}
