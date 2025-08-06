cmake_minimum_required(VERSION 3.16)

project(charge-over-http-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test executable" OFF)

# Output binaries to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Dependencies
find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
# macOS system frameworks for httplib
if(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(CORESERVICES_LIBRARY CoreServices REQUIRED)
endif()

# Battery library
add_library(battery_lib STATIC lib/battery/battery.cpp)
target_include_directories(battery_lib PUBLIC lib/battery/include)

# macOS-specific frameworks (only for battery_lib)
if(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    target_link_libraries(battery_lib PRIVATE
        ${COREFOUNDATION_LIBRARY}
        ${IOKIT_LIBRARY}
    )
endif()

# JSON wrapper library
add_library(json_lib STATIC lib/json-object/json-object.cpp)
target_include_directories(json_lib PUBLIC lib/json-object/include)
target_link_libraries(json_lib PUBLIC nlohmann_json)

# Main executable
add_executable(charge-over-http-cpp src/main.cpp)
target_link_libraries(charge-over-http-cpp
    PRIVATE
    battery_lib
    json_lib
    httplib::httplib
)
if(APPLE)
    target_link_libraries(charge-over-http-cpp
        PRIVATE
        ${COREFOUNDATION_LIBRARY}
        ${CORESERVICES_LIBRARY}
    )
endif()
# Tests
if(BUILD_TESTS)
    enable_testing()

    add_executable(test-json-creation tests/test-json-creation.cpp)
    target_link_libraries(test-json-creation
        PRIVATE
        battery_lib
        json_lib
        nlohmann_json
    )

    add_test(NAME TestJsonCreation COMMAND test-json-creation)
endif()
