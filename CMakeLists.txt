cmake_minimum_required(VERSION 3.16)

project(charge-over-http-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test executable" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_library(battery_lib STATIC lib/battery/battery.cpp)
target_include_directories(battery_lib PUBLIC lib/battery/include)

add_library(json_lib lib/json-object/json-object.cpp)
target_include_directories(json_lib PUBLIC lib/json-object/include)
target_link_libraries(json_lib PUBLIC nlohmann_json)


add_executable(charge-over-http-cpp src/main.cpp)

target_link_libraries(charge-over-http-cpp PRIVATE battery_lib httplib::httplib nlohmann_json)

if(BUILD_TESTS)
    add_executable(test-json-creation tests/test-json-creation.cpp)
    target_link_libraries(test-json-creation PRIVATE battery_lib nlohmann_json json_lib)
endif()