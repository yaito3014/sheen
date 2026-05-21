#ifndef SHEEN_JSON_DEFAULT_PARSER_HPP
#define SHEEN_JSON_DEFAULT_PARSER_HPP

#if defined(SHEEN_JSON_DEFAULT_BACKEND_JSON20)
#include <sheen/json/parsers/json20.hpp>
namespace sheen::json {
using default_parser = json20_parser;
}  // namespace sheen::json
#define SHEEN_JSON_HAS_DEFAULT_PARSER 1
#endif

#endif  // SHEEN_JSON_DEFAULT_PARSER_HPP
