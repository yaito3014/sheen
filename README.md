# sheen

A header-only C++23 serialization library inspired by [cereal](https://github.com/USCiLab/cereal), built around named concepts, deducing-this, `std::expected`, and constexpr-end-to-end.

## Quick taste

```cpp
#include <sheen/archives/json.hpp>

struct point { int x; int y; };

template <class A>
constexpr void serialize(A& ar, point& p) {
    ar(sheen::nvp("x", p.x), sheen::nvp("y", p.y));
}

// Deserialize at compile time:
constexpr auto p = [] {
    sheen::json_input_archive ar{R"({"x":1,"y":2})"};
    point p{};
    ar(p);  // dispatches through your serialize via ADL
    return p;
}();
static_assert(p.x == 1 && p.y == 2);
```

## Components

| Target          | CMake option                  | Notes                                                  |
|-----------------|-------------------------------|--------------------------------------------------------|
| `sheen::sheen`  | always                        | Core. Depends only on `slot::slot`.                    |
| `sheen::yaml`   | `SHEEN_BUILD_YAML=ON`         | Pluggable backend: fkYAML, yaml-cpp.                   |
| `sheen::json`   | `SHEEN_BUILD_JSON=ON`         | Pluggable backend: json20 (constexpr).                 |

Backend per component is enabled via `SHEEN_<FORMAT>_ENABLE_<BACKEND>` and the default-exposed parser via `SHEEN_<FORMAT>_DEFAULT_BACKEND`.

## Building

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build/test
```

Requires C++23 (GCC 14+).

## Status

Pre-1.0. APIs may change.

## License

MIT. See [LICENSE](LICENSE).
