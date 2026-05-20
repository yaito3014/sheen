#include <sheen/archives/yaml.hpp>
#include <sheen/core/concepts.hpp>
#include <sheen/core/policy.hpp>

#include <slot/slot.hpp>

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <string_view>

TEST_CASE("yaml_output_archive satisfies output_archive_for")
{
  using ya = sheen::yaml_output_archive;
  STATIC_CHECK(sheen::output_archive_for<ya, int>);
  STATIC_CHECK(sheen::output_archive_for<ya, bool>);
  STATIC_CHECK(sheen::output_archive_for<ya, std::string_view>);
  STATIC_CHECK(sheen::output_archive_for<ya, slot::binding<sheen::nvp_policy, int>>);
  STATIC_CHECK(sheen::output_archive_for<ya, slot::binding<sheen::comment_policy, int>>);
}

TEST_CASE("yaml_output_archive writes a flat mapping via nvp")
{
  std::string out;
  sheen::yaml_output_archive ar{out};
  ar(sheen::nvp("age", 30));
  ar(sheen::nvp("active", true));
  ar(sheen::nvp("name", std::string_view{"Alice"}));
  CHECK(out == "age: 30\nactive: true\nname: \"Alice\"\n");
}

TEST_CASE("yaml_output_archive combines comment_policy and nvp_policy via wrap")
{
  std::string out;
  sheen::yaml_output_archive ar{out};
  ar(sheen::wrap(30, sheen::nvp_policy{"age"}, sheen::comment_policy{"in years"}));
  CHECK(out == "# in years\nage: 30\n");
}

TEST_CASE("yaml_output_archive is constexpr-clean")
{
  STATIC_CHECK([] {
    std::string out;
    sheen::yaml_output_archive ar{out};
    ar(sheen::nvp("x", 42));
    ar(sheen::wrap(true, sheen::nvp_policy{"ok"}, sheen::comment_policy{"flag"}));
    return out == "x: 42\n# flag\nok: true\n";
  }());
}
