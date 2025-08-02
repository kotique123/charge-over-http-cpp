cmake_minimum_required(VERSION 3.16)

project(charge-over-http-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(httplib CONFIG REQUIRED)
find_package(nlohmann-json CONFIG REQUIRED)

add_library(battery_lib STATIC lib/battery/battery.cpp)
target_include_directories(battery_lib PUBLIC lib/battery/include)

add_executable(charge-over-http-cpp src/main.cpp)

target_link_libraries(charge-over-http-cpp PRIVATE battery_lib httplib::httplib nlohmann-json)

