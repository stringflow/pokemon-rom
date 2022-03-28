@echo off
if not exist build mkdir build
pushd build

set SOURCE_FILES=..\src\rom.cpp ..\sym\symbol-table.o
set COMPILER_FLAGS=-Wall -Wextra -O2 -shared -fdiagnostics-absolute-paths
set IGNORED_WARNINGS=-Wno-switch

echo on
clang++ -o pokemon-rom.dll %COMPILER_FLAGS% %IGNORED_WARNINGS% %SOURCE_FILES%

@echo off
popd