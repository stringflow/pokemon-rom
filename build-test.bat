@echo off
if not exist build mkdir build
pushd build

set SOURCE_FILES=..\test\main.cpp ..\sym\symbol-table.o
set COMPILER_FLAGS=-Wall -fdiagnostics-absolute-paths
set IGNORED_WARNINGS=-Wno-switch

echo on
clang++ -o rom-test.exe %COMPILER_FLAGS% %IGNORED_WARNINGS% %SOURCE_FILES% && rom-test.exe

@echo off
popd