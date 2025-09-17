cmake_minimum_required(VERSION 3.14)
project(my_list_project LANGUAGES CXX)

option(ENABLE_ASAN     "Enable Address/Leak sanitizers" ON)
option(ENABLE_UBSAN    "Enable UndefinedBehavior sanitizer" OFF)
option(ENABLE_COVERAGE "Enable coverage (gcov/lcov)" OFF)
# Если нужен <=>, подними на 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

if(ENABLE_COVERAGE)
    set(ENABLE_ASAN  OFF CACHE BOOL "" FORCE)
    set(ENABLE_UBSAN OFF CACHE BOOL "" FORCE)
endif()

# Debug по умолчанию можно раскомментировать
# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
# endif()

# Coverage
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(STATUS "Coverage enabled")
    add_compile_options(--coverage -O0 -g -fno-omit-frame-pointer)
    add_link_options(--coverage)
elseif(ENABLE_COVERAGE)
    message(FATAL_ERROR "Coverage configured only for GCC/Clang")
endif()

# Sanitizers
if((ENABLE_ASAN OR ENABLE_UBSAN) AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-O0 -g -fno-omit-frame-pointer)
    if(ENABLE_ASAN)
        add_compile_options(-fsanitize=address -fsanitize=leak)
        add_link_options(-fsanitize=address -fsanitize=leak)
    endif()
    if(ENABLE_UBSAN)
        add_compile_options(-fsanitize=undefined)
        add_link_options(-fsanitize=undefined)
    endif()
elseif(ENABLE_ASAN OR ENABLE_UBSAN)
    message(FATAL_ERROR "Sanitizers configured only for GCC/Clang")
endif()

# --- приложение ---
add_executable(list_app
    src/main.cpp
)
target_include_directories(list_app PRIVATE ${CMAKE_SOURCE_DIR}/include)

# --- GoogleTest ---
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# --- тесты ---
enable_testing()

add_executable(list_tests
    tests/test_deque.cpp   # <-- здесь исправление
)
target_include_directories(list_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(list_tests PRIVATE GTest::gtest_main)

add_test(NAME list_tests COMMAND list_tests)

# --- coverage ---
find_program(LCOV lcov)
find_program(GENHTML genhtml)
if(ENABLE_COVERAGE AND LCOV AND GENHTML)
    add_custom_target(coverage
        COMMAND ${LCOV} --directory . --zerocounters
        COMMAND ctest --output-on-failure
        COMMAND ${LCOV} --capture --directory . --output-file coverage.info
        COMMAND ${LCOV} --remove coverage.info '/usr/*' '${CMAKE_BINARY_DIR}/*' '*/tests/*' --output-file coverage.info
        COMMAND ${GENHTML} coverage.info --output-directory coverage
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        DEPENDS list_tests
        COMMENT "Generating coverage report in build/coverage/"
        VERBATIM
    )
endif()

# --- cppcheck ---
find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(run-cppcheck
        COMMAND ${CPPCHECK}
            --enable=all --inconclusive --std=c++14
            --suppress=missingIncludeSystem
            --inline-suppr
            --quiet
            --language=c++
            --error-exitcode=1
            -I ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running cppcheck on include/, src/, tests/"
        VERBATIM
    )
endif()
