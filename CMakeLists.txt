cmake_minimum_required(VERSION 3.24)

project(sheen VERSION 0.1.0 LANGUAGES CXX)

option(
    SHEEN_BUILD_YAML
    "build the YAML archive component (sheen::yaml)"
    ${PROJECT_IS_TOP_LEVEL}
)
option(
    SHEEN_BUILD_JSON
    "build the JSON archive component (sheen::json)"
    ${PROJECT_IS_TOP_LEVEL}
)
option(SHEEN_BUILD_TESTING "build test" ${PROJECT_IS_TOP_LEVEL})
option(SHEEN_INSTALL "generate install rules for sheen" ${PROJECT_IS_TOP_LEVEL})

if(SHEEN_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
    set(SHEEN_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/sheen)
endif()

add_library(sheen INTERFACE)

target_compile_features(sheen INTERFACE cxx_std_23)

file(GLOB_RECURSE SHEEN_HEADERS CONFIGURE_DEPENDS include/*.hpp)

target_sources(
    sheen
    INTERFACE
        FILE_SET sheen_headers_set
            TYPE HEADERS
            BASE_DIRS include
            FILES ${SHEEN_HEADERS}
)

include(FetchContent)

if(NOT DEFINED SLOT_INSTALL)
    set(SLOT_INSTALL ${SHEEN_INSTALL})
endif()

FetchContent_Declare(
    slot
    GIT_REPOSITORY https://github.com/yaito3014/slot
    GIT_TAG main
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(slot)
target_link_libraries(sheen INTERFACE slot::slot)

add_library(sheen::sheen ALIAS sheen)

if(SHEEN_BUILD_YAML)
    add_subdirectory(components/yaml)
endif()

if(SHEEN_BUILD_JSON)
    add_subdirectory(components/json)
endif()

if(SHEEN_BUILD_TESTING)
    enable_testing()
    add_subdirectory(test)
endif()

if(SHEEN_INSTALL)
    install(
        TARGETS sheen
        EXPORT sheen-core-targets
        FILE_SET sheen_headers_set
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
            COMPONENT core
    )

    install(
        EXPORT sheen-core-targets
        FILE sheen-core-targets.cmake
        NAMESPACE sheen::
        DESTINATION ${SHEEN_INSTALL_CMAKEDIR}
        COMPONENT core
    )

    configure_package_config_file(
        cmake/sheen-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/sheen-config.cmake
        INSTALL_DESTINATION ${SHEEN_INSTALL_CMAKEDIR}
    )

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/sheen-config-version.cmake
        COMPATIBILITY SameMajorVersion
        ARCH_INDEPENDENT
    )

    install(
        FILES
            ${CMAKE_CURRENT_BINARY_DIR}/sheen-config.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/sheen-config-version.cmake
        DESTINATION ${SHEEN_INSTALL_CMAKEDIR}
        COMPONENT core
    )
endif()
