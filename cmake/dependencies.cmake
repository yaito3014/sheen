include_guard(GLOBAL)

include(FetchContent)

FetchContent_Declare(
    slot
    GIT_REPOSITORY https://github.com/yaito3014/slot
    GIT_TAG main
    GIT_SHALLOW ON
)

FetchContent_Declare(
    json20
    GIT_REPOSITORY https://github.com/yaito3014/json20
    GIT_TAG main
    GIT_SHALLOW ON
)

FetchContent_Declare(
    fkYAML
    GIT_REPOSITORY https://github.com/fktn-k/fkYAML
    GIT_TAG v0.4.2
    GIT_SHALLOW ON
)

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp
    GIT_TAG 0.8.0
    GIT_SHALLOW ON
)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.15.0
    GIT_SHALLOW ON
)

# Invoked by CMake for every find_package() call. We satisfy the ones we know
# about via FetchContent and leave everything else to the normal search.
# Implemented as a macro so that set(<pkg>_FOUND ...) lands in find_package()'s
# own scope.
macro(sheen_provide_dependency method package_name)
    if("${package_name}" STREQUAL "slot")
        if(NOT DEFINED SLOT_INSTALL)
            set(SLOT_INSTALL ${SHEEN_INSTALL})
        endif()
        FetchContent_MakeAvailable(slot)
        set(slot_FOUND TRUE)
    elseif("${package_name}" STREQUAL "json20")
        if(NOT DEFINED JSON20_INSTALL)
            set(JSON20_INSTALL ${SHEEN_INSTALL})
        endif()
        FetchContent_MakeAvailable(json20)
        set(json20_FOUND TRUE)
    elseif("${package_name}" STREQUAL "fkYAML")
        if(NOT DEFINED FK_YAML_INSTALL)
            set(FK_YAML_INSTALL ${SHEEN_INSTALL})
        endif()
        FetchContent_MakeAvailable(fkYAML)
        set(fkYAML_FOUND TRUE)
    elseif("${package_name}" STREQUAL "yaml-cpp")
        set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
        set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
        set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "" FORCE)
        set(YAML_CPP_INSTALL ${SHEEN_INSTALL} CACHE BOOL "" FORCE)
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        FetchContent_MakeAvailable(yaml-cpp)
        set(yaml-cpp_FOUND TRUE)
    elseif("${package_name}" STREQUAL "Catch2")
        FetchContent_MakeAvailable(Catch2)
        set(Catch2_FOUND TRUE)
    endif()
endmacro()

cmake_language(
    SET_DEPENDENCY_PROVIDER sheen_provide_dependency
    SUPPORTED_METHODS FIND_PACKAGE
)
