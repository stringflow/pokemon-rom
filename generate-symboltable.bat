@echo off
if not exist build mkdir build
if not exist sym mkdir sym

set SOURCE_FILES=..\test\generate-symboltable.cpp
set COMPILER_FLAGS=-Wall -fdiagnostics-absolute-paths -O2
set IGNORED_WARNINGS=
set DEPENDENCIES=
set SYMBOLS=red blue yellow gold silver crystal

pushd sym
echo on
clang++ -o ..\build\generate-symboltable.exe %COMPILER_FLAGS% %IGNORED_WARNINGS% %SOURCE_FILES% %DEPENDENCIES% && ..\build\generate-symboltable.exe %SYMBOLS% && clang++ -c %COMPILER_FLAGS% %IGNORED_WARNINGS% symbol-table.cpp
@echo off
popd