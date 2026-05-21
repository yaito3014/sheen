add_library(sheen_json INTERFACE)

file(GLOB_RECURSE SHEEN_JSON_HEADERS CONFIGURE_DEPENDS include/*.hpp)

target_sources(
    sheen_json
    INTERFACE
        FILE_SET sheen_json_headers_set
            TYPE HEADERS
            BASE_DIRS include
            FILES ${SHEEN_JSON_HEADERS}
)

target_link_libraries(sheen_json INTERFACE sheen::sheen)

option(SHEEN_JSON_ENABLE_JSON20 "build with json20 backend available" ON)

set(SHEEN_JSON_DEFAULT_BACKEND
    "JSON20"
    CACHE STRING
    "which enabled backend is exposed as sheen::json::default_parser (JSON20, NONE)"
)
set_property(
    CACHE SHEEN_JSON_DEFAULT_BACKEND
    PROPERTY STRINGS JSON20 NONE
)

if(SHEEN_JSON_ENABLE_JSON20)
    include(FetchContent)
    FetchContent_Declare(
        json20
        GIT_REPOSITORY https://github.com/yaito3014/json20
        GIT_TAG main
        GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(json20)
    target_link_libraries(sheen_json INTERFACE yk::json20)
    target_compile_definitions(sheen_json INTERFACE SHEEN_JSON_HAS_JSON20)
endif()

if(SHEEN_JSON_DEFAULT_BACKEND STREQUAL "JSON20")
    if(NOT SHEEN_JSON_ENABLE_JSON20)
        message(
            FATAL_ERROR
            "SHEEN_JSON_DEFAULT_BACKEND=JSON20 requires SHEEN_JSON_ENABLE_JSON20=ON"
        )
    endif()
    target_compile_definitions(
        sheen_json
        INTERFACE SHEEN_JSON_DEFAULT_BACKEND_JSON20
    )
elseif(SHEEN_JSON_DEFAULT_BACKEND STREQUAL "NONE")
    # no default; consumer picks per-archive via template arg
else()
    message(
        FATAL_ERROR
        "unsupported SHEEN_JSON_DEFAULT_BACKEND: ${SHEEN_JSON_DEFAULT_BACKEND}"
    )
endif()

add_library(sheen::json ALIAS sheen_json)

if(SHEEN_INSTALL)
    install(
        TARGETS sheen_json
        EXPORT sheen-json-targets
        FILE_SET sheen_json_headers_set
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT json
    )

    install(
        EXPORT sheen-json-targets
        FILE sheen-json-targets.cmake
        NAMESPACE sheen::
        DESTINATION ${SHEEN_INSTALL_CMAKEDIR}
        COMPONENT json
    )
endif()
