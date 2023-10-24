project(tests)

file(GLOB TEST_FILES ./*.cpp ./*.hpp)

set(INCLUDE_DIR "./")
add_executable(${PROJECT_NAME} ${TEST_FILES})

find_package(Catch2)

target_link_libraries(${PROJECT_NAME} Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME} core)
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

include(CTest)
include(Catch)