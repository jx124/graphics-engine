#!/bin/sh

case $1 in
  --debug|-d)
    mkdir -p ./build/debug
    cmake -H. -B build/debug -DCMAKE_BUILD_TYPE=debug
    cmake --build ./build/debug -j 24 && cp -u ./build/debug/compile_commands.json . && ./build/debug/graphics-engine
    ;;
  --release|-r)
    mkdir -p ./build/release
    cmake -H. -B build/release -DCMAKE_BUILD_TYPE=release
    cmake --build ./build/release -j 24 && cp -u ./build/release/compile_commands.json . && ./build/release/graphics-engine
    ;;
  *)
    echo "Usage: ./build.sh [--release|-r|--debug|-d]"
    ;;
esac
