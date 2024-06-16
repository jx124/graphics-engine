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

file(GLOB source CONFIGURE_DEPENDS 
    "src/*.cpp"
    "src/glad/glad.c"
)

file(GLOB imgui_source CONFIGURE_DEPENDS
    "imgui/*.cpp"
    "imgui/backends/imgui_impl_glfw.cpp"
    "imgui/backends/imgui_impl_opengl3.cpp"
)

add_subdirectory("include/glfw")

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_INSTALL_PDB OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
add_subdirectory("assimp")

add_executable(graphics-engine ${source} ${imgui_source})

find_package(OpenMP REQUIRED)

target_include_directories(graphics-engine PUBLIC "include/" "imgui/" "imgui/backends/")
target_link_libraries(graphics-engine PUBLIC dl glfw OpenMP::OpenMP_CXX assimp)
