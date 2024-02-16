cmake_minimum_required(VERSION 3.0.0)
project(BIOAI-2 VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()


add_executable(BIOAI-2 main.cpp)

include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)




