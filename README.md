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
ctest --test-dir build
```

The library targets consume their dependencies via `find_package()` only; they
do not fetch anything themselves. For a standalone build, the root
`CMakeLists.txt` automatically resolves them through a FetchContent-backed
dependency provider (`cmake/dependencies.cmake`), so the commands above work
with no extra flags. A `dev` preset that does the same is also available:

```sh
cmake --preset dev && cmake --build --preset dev && ctest --preset dev
```

Requires CMake 3.24+ and C++23 (GCC 14+).

### Consuming from another project

When sheen is pulled in via `add_subdirectory()` or installed and located with
`find_package(sheen)`, supply its dependencies (`slot`, and per enabled backend
`json20`, `fkYAML`, `yaml-cpp`) the way you manage the rest of your
dependencies — a package manager, system packages, or your own dependency
provider. You can reuse the provider in `cmake/dependencies.cmake` by passing it
via `CMAKE_PROJECT_TOP_LEVEL_INCLUDES`.

## Status

Pre-1.0. APIs may change.

## License

MIT. See [LICENSE](LICENSE).
