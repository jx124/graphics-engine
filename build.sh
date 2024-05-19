#!/bin/sh

case $1 in
  --debug|-d)
    mkdir -p ./build/debug
    cmake -H. -B build/debug -DCMAKE_BUILD_TYPE=debug
    cmake --build ./build/debug && cp -u ./build/debug/compile_commands.json . && ./build/debug/voxelengine
    ;;
  --release|-r)
    mkdir -p ./build/release
    cmake -H. -B build/release -DCMAKE_BUILD_TYPE=release
    cmake --build ./build/release && cp -u ./build/release/compile_commands.json . && ./build/release/voxelengine
    ;;
  *)
    echo "Usage: ./build.sh [--release|-r|--debug|-d]"
    ;;
esac
