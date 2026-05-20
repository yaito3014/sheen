#ifndef SHEEN_YAML_DEFAULT_PARSER_HPP
#define SHEEN_YAML_DEFAULT_PARSER_HPP

#if defined(SHEEN_YAML_BACKEND_FKYAML)
#include <sheen/yaml/parsers/fkyaml.hpp>
namespace sheen::yaml {
using default_parser = fkyaml_parser;
}  // namespace sheen::yaml
#elif defined(SHEEN_YAML_BACKEND_YAMLCPP)
#include <sheen/yaml/parsers/yamlcpp.hpp>
namespace sheen::yaml {
using default_parser = yamlcpp_parser;
}  // namespace sheen::yaml
#else
namespace sheen::yaml {
class default_parser;
}  // namespace sheen::yaml
#endif

#endif  // SHEEN_YAML_DEFAULT_PARSER_HPP
