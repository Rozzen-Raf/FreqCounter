cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(freq-counter LANGUAGES CXX)

option(TESTING "tests" ON)

add_subdirectory(core "${CMAKE_BINARY_DIR}/core")
if(TESTING)
    add_subdirectory(tests "${CMAKE_BINARY_DIR}/tests")
endif()
add_subdirectory(examples "${CMAKE_BINARY_DIR}/examples")