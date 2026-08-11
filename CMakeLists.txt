cmake_minimum_required(VERSION 3.16)

project(MiniRedis VERSION 1.0 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Threads
find_package(Threads REQUIRED)


# ==================================================
# Find all source files
# ==================================================

file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)


# ==================================================
# MiniRedis Node
# ==================================================

set(MINIREDDIS_SOURCES ${ALL_SOURCES})

# Router is a separate executable
list(FILTER MINIREDDIS_SOURCES EXCLUDE REGEX ".*/router/.*")
list(FILTER MINIREDDIS_SOURCES EXCLUDE REGEX ".*/main_router\\.cpp$")

add_executable(MiniRedis
    ${MINIREDDIS_SOURCES}
)

target_include_directories(MiniRedis PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(MiniRedis PRIVATE
    Threads::Threads
)


# ==================================================
# MiniRedis Router
# ==================================================

set(ROUTER_SOURCES

    # Router entry point
    ${PROJECT_SOURCE_DIR}/src/main_router.cpp

    # Router
    ${PROJECT_SOURCE_DIR}/src/router/RouterServer.cpp

    # Sharding
    ${PROJECT_SOURCE_DIR}/src/sharding/ShardRouter.cpp

    # Configuration
    ${PROJECT_SOURCE_DIR}/src/config/Config.cpp

    # RESP parser
    ${PROJECT_SOURCE_DIR}/src/protocol/RESPParser.cpp
)

add_executable(MiniRedisRouter
    ${ROUTER_SOURCES}
)

target_include_directories(MiniRedisRouter PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(MiniRedisRouter PRIVATE
    Threads::Threads
)