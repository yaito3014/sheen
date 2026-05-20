#include <sheen/yaml/parser.hpp>

#include <catch2/catch_test_macros.hpp>

#if defined(SHEEN_YAML_BACKEND_FKYAML)
#include <sheen/yaml/parsers/fkyaml.hpp>

#include <string_view>

TEST_CASE("fkyaml_parser models yaml_parser")
{
  STATIC_CHECK(sheen::yaml::yaml_parser<sheen::yaml::fkyaml_parser>);
  STATIC_CHECK(sheen::yaml::yaml_node<sheen::yaml::fkyaml_node>);
}

TEST_CASE("fkyaml_parser parses a flat mapping")
{
  sheen::yaml::fkyaml_parser p;
  auto root = p.parse("age: 30\nactive: true\nname: Alice").value();

  REQUIRE(root.is_mapping());
  CHECK(root.size() == 3);

  REQUIRE(root.contains("age"));
  CHECK(root["age"].is_integer());
  CHECK(root["age"].as_integer() == 30);

  REQUIRE(root.contains("active"));
  CHECK(root["active"].is_boolean());
  CHECK(root["active"].as_boolean());

  REQUIRE(root.contains("name"));
  CHECK(root["name"].is_string());
  CHECK(root["name"].as_string() == "Alice");
}

TEST_CASE("fkyaml_parser parses a sequence")
{
  sheen::yaml::fkyaml_parser p;
  auto root = p.parse("[1, 2, 3]").value();

  REQUIRE(root.is_sequence());
  CHECK(root.size() == 3);
  CHECK(root[0].as_integer() == 1);
  CHECK(root[2].as_integer() == 3);
}

TEST_CASE("fkyaml_parser returns an error on malformed input")
{
  sheen::yaml::fkyaml_parser p;
  auto r = p.parse("a: 1\na: 2");
  REQUIRE_FALSE(r.has_value());
  CHECK(r.error().code == sheen::error_code::parse_error);
}

#endif  // SHEEN_YAML_BACKEND_FKYAML
