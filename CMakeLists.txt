cmake_minimum_required(VERSION 3.16)

project(MiniRedis VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find all common source files
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# --------------------------------------------------
# MiniRedis Node
# --------------------------------------------------

set(MINIREDDIS_SOURCES ${ALL_SOURCES})

list(FILTER MINIREDDIS_SOURCES EXCLUDE REGEX ".*/router/.*")
list(FILTER MINIREDDIS_SOURCES EXCLUDE REGEX ".*/main_router\\.cpp$")

add_executable(MiniRedis ${MINIREDDIS_SOURCES})

target_include_directories(MiniRedis PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# --------------------------------------------------
# MiniRedis Router
# --------------------------------------------------

set(ROUTER_SOURCES
    ${PROJECT_SOURCE_DIR}/src/main_router.cpp
    ${PROJECT_SOURCE_DIR}/src/router/RouterServer.cpp
    ${PROJECT_SOURCE_DIR}/src/sharding/ShardRouter.cpp
    ${PROJECT_SOURCE_DIR}/src/protocol/RESPParser.cpp
)

add_executable(MiniRedisRouter ${ROUTER_SOURCES})

target_include_directories(MiniRedisRouter PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# --------------------------------------------------
# Threads
# --------------------------------------------------

find_package(Threads REQUIRED)

target_link_libraries(MiniRedis PRIVATE
    Threads::Threads
)

target_link_libraries(MiniRedisRouter PRIVATE
    Threads::Threads
)