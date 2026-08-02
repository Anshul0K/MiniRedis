cmake_minimum_required(VERSION 3.16)

project(MiniRedis VERSION 1.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Collect all source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

# Create executable
add_executable(MiniRedis ${SOURCES})

# Header search path
target_include_directories(MiniRedis PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

# Thread library (needed later)
find_package(Threads REQUIRED)
target_link_libraries(MiniRedis PRIVATE Threads::Threads)