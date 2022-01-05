@echo off
if not exist build mkdir build
pushd build
clang++ -o2 -fdiagnostics-absolute-paths -Wall -Wno-switch ..\test\main.cpp -o rom-test.exe && rom-test.exe
popd