cmake_minimum_required(VERSION 3.15)
project(graphics-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
    add_compile_definitions(DEBUG=1)
else(CMAKE_BUILD_TYPE STREQUAL "release")
    add_compile_options(-O2 -march=native -flto -pipe)
endif()

file(GLOB_RECURSE source CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/glad/glad.c"
)

add_subdirectory("include/glfw")

add_executable(graphics-engine ${source})

find_package(OpenMP REQUIRED)

target_include_directories(graphics-engine PUBLIC "include/")
target_link_libraries(graphics-engine PUBLIC dl glfw OpenMP::OpenMP_CXX)
