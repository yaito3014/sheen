#include <sheen/archives/json.hpp>
#include <sheen/core/policy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

TEST_CASE("json_output_archive emits root-level scalars")
{
  SECTION("int")
  {
    std::string out;
    {
      sheen::json_output_archive ar{out};
      ar(42);
    }
    CHECK(out == "42");
  }
  SECTION("negative int")
  {
    std::string out;
    {
      sheen::json_output_archive ar{out};
      ar(-7);
    }
    CHECK(out == "-7");
  }
  SECTION("bool true")
  {
    std::string out;
    {
      sheen::json_output_archive ar{out};
      ar(true);
    }
    CHECK(out == "true");
  }
  SECTION("bool false")
  {
    std::string out;
    {
      sheen::json_output_archive ar{out};
      ar(false);
    }
    CHECK(out == "false");
  }
  SECTION("string")
  {
    std::string out;
    {
      sheen::json_output_archive ar{out};
      ar(std::string_view{"hello"});
    }
    CHECK(out == "\"hello\"");
  }
}

TEST_CASE("json_output_archive wraps an nvp sequence in a root object")
{
  std::string out;
  {
    sheen::json_output_archive ar{out};
    int x = 1;
    int y = 2;
    ar(sheen::nvp("x", x));
    ar(sheen::nvp("y", y));
  }
  CHECK(out == R"({"x":1,"y":2})");
}

TEST_CASE("json_output_archive escapes special characters inside strings")
{
  std::string out;
  {
    sheen::json_output_archive ar{out};
    ar(std::string_view{"a\"b\\c\n\t"});
  }
  CHECK(out == R"("a\"b\\c\n\t")");
}

namespace ex {

struct point {
  int x;
  int y;

  template<class A>
  constexpr void serialize(A& ar) const
  {
    ar(sheen::nvp("x", x));
    ar(sheen::nvp("y", y));
  }
};

struct rectangle {
  int width;
  int height;
};

template<class A>
constexpr void serialize(A& ar, rectangle const& r)
{
  ar(sheen::nvp("width", r.width));
  ar(sheen::nvp("height", r.height));
}

}  // namespace ex

TEST_CASE("json_output_archive drives intrusive serialize")
{
  std::string out;
  {
    sheen::json_output_archive ar{out};
    ex::point p{1, 2};
    p.serialize(ar);
  }
  CHECK(out == R"({"x":1,"y":2})");
}

TEST_CASE("json_output_archive drives free serialize via ADL")
{
  std::string out;
  {
    sheen::json_output_archive ar{out};
    ex::rectangle r{4, 5};
    serialize(ar, r);
  }
  CHECK(out == R"({"width":4,"height":5})");
}

TEST_CASE("json_output_archive is constexpr-clean")
{
  STATIC_CHECK([] {
    std::string out;
    {
      sheen::json_output_archive ar{out};
      ex::point p{3, 4};
      p.serialize(ar);
    }
    return out == R"({"x":3,"y":4})";
  }());
}
