#include <sheen/yaml/default_parser.hpp>
#include <sheen/yaml/parser.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("default_parser models yaml_parser")
{
  STATIC_CHECK(sheen::yaml::yaml_parser<sheen::yaml::default_parser>);
  STATIC_CHECK(sheen::yaml::yaml_node<sheen::yaml::default_parser::node_type>);
}

TEST_CASE("default_parser parses a flat mapping")
{
  sheen::yaml::default_parser p;
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

TEST_CASE("default_parser parses a sequence")
{
  sheen::yaml::default_parser p;
  auto root = p.parse("[1, 2, 3]").value();

  REQUIRE(root.is_sequence());
  CHECK(root.size() == 3);
  CHECK(root[0].as_integer() == 1);
  CHECK(root[2].as_integer() == 3);
}
