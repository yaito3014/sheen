#include <sheen/core/dom.hpp>
#include <sheen/json/default_parser.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("json default_parser models tree_parser")
{
  STATIC_CHECK(sheen::dom::tree_parser<sheen::json::default_parser>);
  STATIC_CHECK(sheen::dom::tree_node<sheen::json::default_parser::node_type>);
}

TEST_CASE("json default_parser parses a flat object")
{
  sheen::json::default_parser p;
  auto root = p.parse(R"({"age":30,"active":true,"name":"Alice"})").value();

  REQUIRE(root.is_record());
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

TEST_CASE("json default_parser parses an array")
{
  sheen::json::default_parser p;
  auto root = p.parse("[1, 2, 3]").value();

  REQUIRE(root.is_list());
  CHECK(root.size() == 3);
  CHECK(root[0].as_integer() == 1);
  CHECK(root[2].as_integer() == 3);
}
