# wrenvm_tests

## how to build a project

install prerequisites:
+ c/c++ compiler
+ `meson`
+ `ninja`

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

## how to build repro for `foreigncall` issue:

install prerequisites:
+ c/c++ compiler
+ `meson`
+ `ninja`

get submodules (wren is a submodule):
```sh
git submodule update --init --recursive
```

build a demo, such as `./foreigncall`:
```sh
cd ./foreigncall
meson setup build
ninja -C build
./build/foreigncall_test
```
