@echo off
if not exist build mkdir build
pushd build
clang++ -fdiagnostics-absolute-paths -Wno-switch ..\test\main.cpp -o rom-test.exe && rom-test.exe
popd