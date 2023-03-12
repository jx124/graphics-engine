cmake_minimum_required(VERSION 3.10)
project(graphics-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/Wall)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall)
endif()

add_executable(graphics-engine src/main.cpp src/glad.c)

target_include_directories(graphics-engine PUBLIC include)
target_link_libraries(graphics-engine PUBLIC dl glfw)
