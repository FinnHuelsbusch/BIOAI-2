cmake_minimum_required(VERSION 3.12.0)
project(BIOAI-2 VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()

include(FetchContent)
FetchContent_Declare(json DOWNLOAD_EXTRACT_TIMESTAMP true URL  https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(googletest  DOWNLOAD_EXTRACT_TIMESTAMP true URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

file(GLOB_RECURSE SRC_FILES src/*.cpp)
add_executable(BIOAI-2 ${SRC_FILES} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

file(GLOB_RECURSE TEST_FILES test/src/*.cpp)
add_executable(TestBIOAI-2 ${TEST_FILES} ${SRC_FILES})
target_link_libraries(TestBIOAI-2 PRIVATE gtest_main gmock_main nlohmann_json::nlohmann_json)
target_include_directories(TestBIOAI-2 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/test/include)
# set TESTING_MODE to 1 to enable testing mode
target_compile_definitions(TestBIOAI-2 PRIVATE TESTING_MODE=1)
gtest_discover_tests(TestBIOAI-2)
add_test(NAME TestBIOAI-2 COMMAND TestBIOAI-2)




set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)





