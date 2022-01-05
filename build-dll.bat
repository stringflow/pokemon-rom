@echo off
if not exist build mkdir build
pushd build
clang++ -o2 -fdiagnostics-absolute-paths -Wall -Wno-switch -shared ..\src\rom.cpp -o rom.dll
popd