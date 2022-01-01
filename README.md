# pokemon-rom

pokemon-rom is a work-in-progress C DLL that wraps parsing of commonly used data from generation 1 pokemon roms. Any function marked with `DLLEXPORT` can be called from any other language that calling C libraries.

# Code Layout

All DLL code is located in the `src` directory, with all code being `#inlcude` into the main translation unit, resulting in no header files. Testing code lives in the `test` directory, it follows a similar pattern as well.

# Build Instructions

Currently only build scripts for Windows exist since that is the platform that I am using to develop this project.  

Install clang and run `build-dll.bat` to build the DLL, or `build-test.bat` to build the testing executable. In order to run that executable, you will have to put ROM as well as .sym files into the respective directories within `test`.

# Future plans/goals

* Collision logic
* Inbuilt stat and damage calculators
* Near full parsing of the entire pokered data folder
* Gen 2 support