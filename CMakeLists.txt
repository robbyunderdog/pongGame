cmake_minimum_required(VERSION 3.16)
project(PongGame CXX C)

set(CMAKE_CXX_STANDARD 17)

# Download raylib automatically the first time we build.
include(FetchContent)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)   # lets raylib's older CMake files work with new CMake
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.5.tar.gz
)
FetchContent_MakeAvailable(raylib)

# Our game: one executable built from main.cpp, linked against raylib.
add_executable(pong main.cpp)
target_link_libraries(pong raylib)
