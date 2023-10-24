cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

project(core LANGUAGES CXX)

set(INCLUDE_DIR "./include")

file(GLOB SOURCE_FILES 
./src/*.cpp)

file(GLOB HEADER_FILES
./include/*.hpp)

add_library (${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})