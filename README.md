# sheen

A header-only C++23 serialization library inspired by [cereal](https://github.com/USCiLab/cereal), built around named concepts, deducing-this, `std::expected`, and partially constexpr.

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
ctest --test-dir build
```

Requires CMake 3.24+ and C++23 (GCC 14+).

### Consuming from another project

When sheen is pulled in via `add_subdirectory()` or installed and located with
`find_package(sheen)`, supply its dependencies (`slot`, and per enabled backend
`json20`, `fkYAML`, `yaml-cpp`) the way you manage the rest of your
dependencies — a package manager, system packages, or your own dependency
provider.

## Status

Pre-1.0. APIs may change.

## License

MIT. See [LICENSE](LICENSE).
