cmake_minimum_required(VERSION 3.10)
project(graphics-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG=1)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/Wall /fsanitize=address /fsanitize=leak /fsanitize=undefined /GS)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wall -fsanitize=address -fsanitize=leak -fsanitize=undefined -fstack-protector-strong -pipe)
        add_link_options(-fsanitize=address -fsanitize=undefined)
    endif()
else()
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/O2 /LTCG)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-O3 -march=native -flto -pipe)
    endif()
endif()

file(GLOB source_files CONFIGURE_DEPENDS "src/*.cpp" "src/glad.c" "src/imgui/*.cpp")

add_executable(graphics-engine ${source_files})

target_include_directories(graphics-engine PUBLIC "include/" "include/imgui")
target_link_libraries(graphics-engine PUBLIC dl glfw)
