cmake_minimum_required(VERSION 3.14)
project(MyProject LANGUAGES CXX)

include(CTest)
enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX ".*/main\\.cpp$")
list(LENGTH SRC_FILES NUM_SRC_FILES)

if(NUM_SRC_FILES GREATER 0)
    add_library(my_lib ${SRC_FILES})
    target_include_directories(my_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/core
    )
    target_compile_options(my_lib PRIVATE
        $<$<CONFIG:Debug>:--coverage -fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=leak>
    )
    target_link_options(my_lib PRIVATE
        $<$<CONFIG:Debug>:--coverage -fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=leak>
    )
endif()

file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(tests ${TEST_FILES})

if(NUM_SRC_FILES GREATER 0)
    target_link_libraries(tests PRIVATE my_lib GTest::gtest_main)
else()
    target_include_directories(tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/core
    )
    target_link_libraries(tests PRIVATE GTest::gtest_main)
endif()

target_compile_options(tests PRIVATE
    $<$<CONFIG:Debug>:--coverage -fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=leak>
)
target_link_options(tests PRIVATE
    $<$<CONFIG:Debug>:--coverage -fprofile-arcs -ftest-coverage -fsanitize=address -fsanitize=leak>
)

add_test(NAME MyTests COMMAND tests)

find_program(LCOV lcov)
find_program(GENHTML genhtml)

if(LCOV AND GENHTML)
  add_custom_target(coverage
    COMMAND ${LCOV} --rc branch_coverage=1 --zerocounters --directory ${CMAKE_BINARY_DIR}
    COMMAND ctest --output-on-failure
    COMMAND ${LCOV} --rc branch_coverage=1 --capture
            --directory ${CMAKE_BINARY_DIR}
            --base-directory ${CMAKE_SOURCE_DIR}
            --output-file coverage.info
    COMMAND ${LCOV} --rc branch_coverage=1 --remove coverage.info
            '/usr/*'
            '*/_deps/*'
            '*/googletest/*'
            '*/tests/*'
            '*/src/bigint.cpp'
            '*/src/main.cpp'
            --output-file coverage_filtered.info
    COMMAND ${GENHTML} --branch-coverage --highlight --legend coverage_filtered.info --output-directory coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS tests
    VERBATIM
  )

  add_custom_target(coverage_modexp
    COMMAND ${LCOV} --rc branch_coverage=1 --zerocounters --directory ${CMAKE_BINARY_DIR}
    COMMAND ctest --output-on-failure
    COMMAND ${LCOV} --rc branch_coverage=1 --capture
            --directory ${CMAKE_BINARY_DIR}
            --base-directory ${CMAKE_SOURCE_DIR}
            --output-file coverage.info
    COMMAND ${LCOV} --rc branch_coverage=1 --extract coverage.info
            "*/src/mod_exp.cpp"
            "${CMAKE_SOURCE_DIR}/src/mod_exp.cpp"
            --output-file coverage_filtered.info
    COMMAND ${GENHTML} --branch-coverage --highlight --legend coverage_filtered.info --output-directory coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS tests
    VERBATIM
  )
else()
  message(WARNING "lcov/genhtml не найдены — цели 'coverage' и 'coverage_modexp' недоступны.")
endif()

find_program(CPPCHECK cppcheck)
if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND find ./ -type f -name *.gcda -delete
        COMMAND ${CPPCHECK} --quiet --enable=all --inconclusive --std=c++20
                --suppress=missingIncludeSystem
                -I include -I include/core
                src tests
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        VERBATIM
    )
else()
    message(WARNING "cppcheck не найден, цель 'cppcheck' недоступна.")
endif()

add_custom_target(clean-build
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/bin
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/*.a
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/test*
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage_filtered.info
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage.info
)

add_custom_target(purge
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}
)
