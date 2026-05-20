include(CTest)
include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2
    GIT_TAG v3.15.0
    GIT_SHALLOW ON
)
FetchContent_MakeAvailable(Catch2)

function(sheen_add_test test_name)
    set(name sheen_${test_name}_test)
    add_executable(${name} ${test_name}.cpp)
    target_link_libraries(${name} PRIVATE sheen::sheen Catch2::Catch2WithMain ${ARGN})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

sheen_add_test(error)
sheen_add_test(exception)
sheen_add_test(concepts)
sheen_add_test(customization_points)
sheen_add_test(result)

if(SHEEN_BUILD_YAML)
    sheen_add_test(user_type sheen::yaml)
    sheen_add_test(yaml sheen::yaml)
    sheen_add_test(yaml_input sheen::yaml)
    if(SHEEN_YAML_BACKEND STREQUAL "FKYAML")
        sheen_add_test(yaml_parser sheen::yaml)
    endif()
endif()
