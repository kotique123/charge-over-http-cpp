cmake_minimum_required(VERSION 3.16) 

project(charge-over-http-cpp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build test executable" OFF)
option(BUILD_INSPECT "Build inspect executable" OFF) # Output binaries to bin/
option(BUILD_SHARED_LIBS "Build libraries as shared (off=static)" OFF)
option(ENABLE_LTO "Enable Link-Time Optimization for Release" ON)
option(ENABLE_NATIVE_OPTIMIZATION "Enable -march/-mtune=native in Release" ON)
option(ENABLE_SANITIZERS "Enable Address/UB sanitizers in Debug" OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# Visibility and PIC for better shared/static builds
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Global compile and link options
add_compile_options(
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Release>:-DNDEBUG>
    $<$<CONFIG:Release>:-ffunction-sections>
    $<$<CONFIG:Release>:-fdata-sections>
    $<$<CONFIG:Release>:-fvisibility=hidden>
    $<$<CONFIG:Release>:-fvisibility-inlines-hidden>

    $<$<CONFIG:Debug>:-O0>
    $<$<CONFIG:Debug>:-g3>
    $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
)

if(ENABLE_NATIVE_OPTIMIZATION)
    if(APPLE)
        add_compile_options($<$<CONFIG:Release>:-mcpu=native>)
    else()
        add_compile_options(
            $<$<CONFIG:Release>:-march=native>
            $<$<CONFIG:Release>:-mtune=native>
        )
    endif()
endif()

if(APPLE)
    add_link_options($<$<CONFIG:Release>:-Wl,-dead_strip>)
else()
    add_link_options($<$<CONFIG:Release>:-Wl,--gc-sections>)
endif()

if(ENABLE_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

if(ENABLE_SANITIZERS)
    add_compile_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
    add_link_options($<$<CONFIG:Debug>:-fsanitize=address,undefined>)
endif()

# Dependencies
find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
if(APPLE)
    find_library(COREFOUNDATION_LIBRARY CoreFoundation REQUIRED)
    find_library(CORESERVICES_LIBRARY CoreServices REQUIRED)
endif()

# Battery library
add_library(battery_lib lib/battery/battery.cpp)
target_include_directories(battery_lib PUBLIC lib/battery/include)

# macOS-specific frameworks (only for battery_lib)
if(APPLE)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    target_link_libraries(battery_lib PRIVATE
        ${COREFOUNDATION_LIBRARY}
        ${IOKIT_LIBRARY}
    )
endif()

# JSON wrapper library
add_library(json_lib lib/json-object/json-object.cpp)
target_include_directories(json_lib PUBLIC lib/json-object/include)
target_link_libraries(json_lib PUBLIC battery_lib nlohmann_json)

# Executables
# Server
add_executable(battery-server src/server.cpp)
target_link_libraries(battery-server
    PRIVATE
    battery_lib
    json_lib
    httplib::httplib
)
if(APPLE)
    target_link_libraries(battery-server
        PRIVATE
        ${COREFOUNDATION_LIBRARY}
        ${CORESERVICES_LIBRARY}
    )
endif()

# Client
add_executable(battery-client src/client.cpp)
target_link_libraries(battery-client
    PRIVATE
    httplib::httplib
    nlohmann_json
)
if(APPLE)
    target_link_libraries(battery-client
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

if(BUILD_INSPECT AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_executable(inspect tests/insepct.cpp)
    target_link_libraries(inspect
        PRIVATE
        battery_lib
        json_lib
    )
elseif(BUILD_INSPECT)
    message(STATUS "Skipping 'inspect' target for ${CMAKE_BUILD_TYPE}. It is only built for Debug.")
endif()