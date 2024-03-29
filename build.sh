clear
case $1 in
  --debug|-d)
    cmake -H. -Bbuild/debug -DCMAKE_BUILD_TYPE=Debug
    cmake --build ./build/debug && ./build/debug/graphics-engine
    ;;
  --release|-r)
    cmake -H. -Bbuild/release -DCMAKE_BUILD_TYPE=Release
    cmake --build ./build/release && ./build/release/graphics-engine
    ;;
  *)
    echo "Usage: ./build.sh [--release|-r|--debug|-d]"
    ;;
esac
