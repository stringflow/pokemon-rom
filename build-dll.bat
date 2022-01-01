@echo off
if not exist build mkdir build
pushd build
clang++ -fdiagnostics-absolute-paths -shared ..\src\rom.cpp -o rom.dll
popd