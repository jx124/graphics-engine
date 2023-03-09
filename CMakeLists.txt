cmake_minimum_required(VERSION 3.10)
project(pathtracer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/Wall)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wall)
endif()

add_executable(pathtracer src/main.cpp src/glad.c)

target_include_directories(pathtracer PUBLIC include)
target_link_libraries(pathtracer PUBLIC dl glfw)
