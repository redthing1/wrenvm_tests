# wrenvm_tests

how to build

get submodules:
```sh
git submodule update --init --recursive
```

build a demo, such as `./wrenscript`:
```sh
cd ./wrenscript
meson setup build
ninja -C build
./build/wrenscript_test
```
