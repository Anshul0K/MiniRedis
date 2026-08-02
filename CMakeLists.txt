cmake_minimum_required(VERSION 3.16)

project(MiniRedis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

add_executable(MiniRedis ${SOURCES})

if(UNIX)
    target_link_libraries(MiniRedis pthread)
endif()