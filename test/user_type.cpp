#include <sheen/archives/yaml.hpp>
#include <sheen/core/concepts.hpp>
#include <sheen/core/dispatch.hpp>
#include <sheen/core/policy.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>

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

TEST_CASE("customization-point concepts detect the user opt-ins")
{
  using ya = sheen::yaml_output_archive;
  using namespace sheen::detail;

  STATIC_CHECK(has_output_member_serialize<ya, ex::point>);
  STATIC_CHECK(!has_output_member_save<ya, ex::point>);
  STATIC_CHECK(!has_output_free_serialize<ya, ex::point>);

  STATIC_CHECK(has_output_free_serialize<ya, ex::rectangle>);
  STATIC_CHECK(!has_output_member_serialize<ya, ex::rectangle>);
  STATIC_CHECK(!has_output_free_save<ya, ex::rectangle>);
}

TEST_CASE("serializable / deserializable detect archive-agnostic opt-ins")
{
  STATIC_CHECK(sheen::serializable<ex::point>);
  STATIC_CHECK(sheen::serializable<ex::rectangle>);
  STATIC_CHECK(!sheen::serializable<int>);

  STATIC_CHECK(sheen::deserializable<ex::point&>);
  STATIC_CHECK(!sheen::deserializable<ex::point>);
  STATIC_CHECK(!sheen::deserializable<ex::point const&>);
  STATIC_CHECK(!sheen::deserializable<int&>);
}

TEST_CASE("intrusive serialize: point → YAML")
{
  std::string out;
  sheen::yaml_output_archive ar{out};
  ex::point p{1, 2};
  p.serialize(ar);
  CHECK(out == "x: 1\ny: 2\n");
}

TEST_CASE("free serialize (ADL): rectangle → YAML")
{
  std::string out;
  sheen::yaml_output_archive ar{out};
  ex::rectangle r{4, 5};
  serialize(ar, r);
  CHECK(out == "width: 4\nheight: 5\n");
}

TEST_CASE("yaml_output_archive dispatches user types via ar(value)")
{
  std::string out;
  sheen::yaml_output_archive ar{out};
  ex::point const p{1, 2};
  ar(p);
  CHECK(out == "x: 1\ny: 2\n");
}

TEST_CASE("yaml_output_archive variadic call composes single-arg invocations")
{
  std::string out;
  sheen::yaml_output_archive ar{out};
  int x = 1, y = 2;
  ar(sheen::nvp("x", x), sheen::nvp("y", y));
  CHECK(out == "x: 1\ny: 2\n");
}

TEST_CASE("user-type serialization is constexpr-clean")
{
  STATIC_CHECK([] {
    std::string out;
    sheen::yaml_output_archive ar{out};
    ex::point p{3, 4};
    p.serialize(ar);
    return out == "x: 3\ny: 4\n";
  }());
}
