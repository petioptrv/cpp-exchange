cmake_minimum_required(VERSION 3.14...3.22)

# ---- Project ----
project(
        CPPExchange
        VERSION 1.0
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_VERBOSE_MAKEFILE ON)

# ---- In-source build guard ----
if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR
            "In-source builds are not allowed. Please create a new directory (e.g., 'build/') and run CMake from there: \n"
            "  mkdir build\n"
            "  cd build\n"
            "  cmake .."
    )
endif()

# ---- Add source files ----
file(GLOB headers CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/communication/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/cppexchange/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/utils/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)
file(GLOB sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/source/cppexchange/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/source/utils/*.cpp"
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# ---- Create library ----
add_library(${PROJECT_NAME} STATIC ${headers} ${sources})

# ---- Link libevent (Intel macOS) ----
set(LIBEVENT_INCLUDE_DIRS "/usr/local/include")
set(LIBEVENT_LIBRARIES "/usr/local/lib/libevent.dylib")

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBEVENT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBEVENT_LIBRARIES})

# ---- Option to build standalone ----
option(BUILD_STANDALONE "Build the standalone executable" OFF)

if (BUILD_STANDALONE)
    add_executable(investigation_alignment ${CMAKE_SOURCE_DIR}/standalone/investigation/alignment.cpp)

    add_executable(socket_example ${CMAKE_SOURCE_DIR}/standalone/socket_example.cpp)
    target_link_libraries(socket_example ${PROJECT_NAME})

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.env
            ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endif()

# ---- Option to build test ----
option(BUILD_TEST "Build the tests" OFF)

if (BUILD_TEST)
    option(ENABLE_TEST_COVERAGE "Enable test coverage" OFF)
    option(TEST_INSTALLED_VERSION "Test the version found by find_package" OFF)

    set(CPP_EXCHANGE_TESTS cpp_exchange_tests)

    file(GLOB test_sources CONFIGURE_DEPENDS
            ${CMAKE_CURRENT_SOURCE_DIR}/test/include/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/include/communication/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/include/cppexchange/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/include/utils/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/test/source/*.cpp
    )

    add_executable(${CPP_EXCHANGE_TESTS} ${test_sources})

    find_package(doctest REQUIRED)
    target_link_libraries(${CPP_EXCHANGE_TESTS} doctest::doctest)
    target_link_libraries(${CPP_EXCHANGE_TESTS} ${PROJECT_NAME})
    include(test/doctest.cmake)
    doctest_discover_tests(${CPP_EXCHANGE_TESTS})

    if (ENABLE_TEST_COVERAGE)
        target_compile_options(CPPExchange PUBLIC -O0 -g -fprofile-arcs -ftest-coverage)
        target_link_options(CPPExchange PUBLIC -fprofile-arcs -ftest-coverage)
    endif()
endif()
