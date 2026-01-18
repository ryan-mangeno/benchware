cmake_minimum_required(VERSION 3.12)
project(benchware VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT DEFINED TORCH_INSTALL_PREFIX)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/libtorch")
        set(TORCH_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/libtorch")
    elseif(DEFINED ENV{LIBTORCH_PATH})
        set(TORCH_INSTALL_PREFIX "$ENV{LIBTORCH_PATH}")
    else()
        message(FATAL_ERROR "libtorch not found set LIBTORCH_PATH or place it in the root")
    endif()
endif()

list(APPEND CMAKE_PREFIX_PATH "${TORCH_INSTALL_PREFIX}")
find_package(Torch REQUIRED)

file(GLOB_RECURSE BENCHWARE_SOURCES "benchware/Profiler/*.hpp" "benchware/Profiler/*.h" "benchware/Profiler/*.cpp")
add_library(benchware STATIC ${BENCHWARE_SOURCES})

target_link_libraries(benchware PUBLIC "${TORCH_LIBRARIES}")
target_include_directories(benchware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/benchware)
target_include_directories(benchware PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE CPU_SRC "benchware/tests/Hardware/CPU/*.cpp")

add_executable(cpu-benchmark ${CPU_SRC})

target_link_libraries(cpu-benchmark PRIVATE benchware)



if(APPLE)
    find_library(METAL_LIB Metal REQUIRED)
    find_library(FOUNDATION_LIB Foundation REQUIRED)
    target_link_libraries(benchware PRIVATE ${METAL_LIB} ${FOUNDATION_LIB})
endif()