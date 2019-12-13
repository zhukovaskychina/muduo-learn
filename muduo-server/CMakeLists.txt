cmake_minimum_required(VERSION 3.15)
project(muduo-server)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release-install-cpp11")
endif()
message(${CMAKE_BUILD_TYPE})
if (NOT MUDUO_PATH)
    set(MUDUO_PATH "/home/$ENV{USER}/wk/build/release-install-cpp11")
endif ()

message(${MUDUO_PATH})
set(CXX_FLAGS
        -g
        # -DVALGRIND
        # -DMUDUO_STD_STRING
        -Wall
        -Wextra
        # -m32
        -Werror
        -Wconversion
        -Wno-unused-parameter
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -march=native
        # -MMD
        # -std=c++0x
        -rdynamic
        )
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -finline-limit=1000 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_path(Muduo_INCLUDE_DIR muduo "${MUDUO_PATH}/include")
find_path(Muduo_LIBRARY_DIR libmuduo_net.a "${MUDUO_PATH}/lib")
set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${Muduo_LIBRARY_DIR})
message(STATUS ${Muduo_INCLUDE_DIR})
message(STATUS ${Muduo_LIBRARY_DIR})

include_directories(${Muduo_INCLUDE_DIR})
find_library(muduo_base muduo_base)
find_library(muduo_net muduo_net)
message(STATUS ${muduo_base})
message(STATUS ${muduo_net})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS ${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR})
add_executable(muduodemo main.cpp)
target_link_libraries(muduodemo ${muduo_net})
target_link_libraries(muduodemo ${muduo_base})
target_link_libraries(muduodemo pthread rt)