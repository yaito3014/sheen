add_library(sheen_yaml INTERFACE)

file(GLOB_RECURSE SHEEN_YAML_HEADERS CONFIGURE_DEPENDS include/*.hpp)

target_sources(
    sheen_yaml
    INTERFACE
        FILE_SET sheen_yaml_headers_set
            TYPE HEADERS
            BASE_DIRS include
            FILES ${SHEEN_YAML_HEADERS}
)

target_link_libraries(sheen_yaml INTERFACE sheen::sheen)

option(SHEEN_YAML_ENABLE_FKYAML "build with fkYAML backend available" ON)
option(SHEEN_YAML_ENABLE_YAMLCPP "build with yaml-cpp backend available" OFF)

set(SHEEN_YAML_DEFAULT_BACKEND
    "FKYAML"
    CACHE STRING
    "which enabled backend is exposed as sheen::yaml::default_parser (FKYAML, YAMLCPP, NONE)"
)
set_property(
    CACHE SHEEN_YAML_DEFAULT_BACKEND
    PROPERTY STRINGS FKYAML YAMLCPP NONE
)

if(SHEEN_YAML_ENABLE_FKYAML)
    find_package(fkYAML REQUIRED)
    target_link_libraries(sheen_yaml INTERFACE fkYAML::fkYAML)
    target_compile_definitions(sheen_yaml INTERFACE SHEEN_YAML_HAS_FKYAML)
endif()

if(SHEEN_YAML_ENABLE_YAMLCPP)
    find_package(yaml-cpp REQUIRED)
    target_link_libraries(sheen_yaml INTERFACE yaml-cpp::yaml-cpp)
    target_compile_definitions(sheen_yaml INTERFACE SHEEN_YAML_HAS_YAMLCPP)
endif()

if(SHEEN_YAML_DEFAULT_BACKEND STREQUAL "FKYAML")
    if(NOT SHEEN_YAML_ENABLE_FKYAML)
        message(
            FATAL_ERROR
            "SHEEN_YAML_DEFAULT_BACKEND=FKYAML requires SHEEN_YAML_ENABLE_FKYAML=ON"
        )
    endif()
    target_compile_definitions(
        sheen_yaml
        INTERFACE SHEEN_YAML_DEFAULT_BACKEND_FKYAML
    )
elseif(SHEEN_YAML_DEFAULT_BACKEND STREQUAL "YAMLCPP")
    if(NOT SHEEN_YAML_ENABLE_YAMLCPP)
        message(
            FATAL_ERROR
            "SHEEN_YAML_DEFAULT_BACKEND=YAMLCPP requires SHEEN_YAML_ENABLE_YAMLCPP=ON"
        )
    endif()
    target_compile_definitions(
        sheen_yaml
        INTERFACE SHEEN_YAML_DEFAULT_BACKEND_YAMLCPP
    )
elseif(SHEEN_YAML_DEFAULT_BACKEND STREQUAL "NONE")
    # no default; consumer picks per-archive via template arg
else()
    message(
        FATAL_ERROR
        "unsupported SHEEN_YAML_DEFAULT_BACKEND: ${SHEEN_YAML_DEFAULT_BACKEND}"
    )
endif()

add_library(sheen::yaml ALIAS sheen_yaml)

if(SHEEN_INSTALL)
    install(
        TARGETS sheen_yaml
        EXPORT sheen-yaml-targets
        FILE_SET sheen_yaml_headers_set
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT yaml
    )

    install(
        EXPORT sheen-yaml-targets
        FILE sheen-yaml-targets.cmake
        NAMESPACE sheen::
        DESTINATION ${SHEEN_INSTALL_CMAKEDIR}
        COMPONENT yaml
    )
endif()
