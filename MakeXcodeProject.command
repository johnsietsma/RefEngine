cd `dirname $0`
mkdir -p build/xcode
pushd build/xcode
cmake -G "Xcode" ../../
popd
