@echo off

SET CMAKE="cmake.exe"
SET SOURCE_PATH=%CD%

IF [%1]==[] SET BUILD_TYPE=debug
SET BUILD_DIR=%CD%\out\build


IF EXIST %BUILD_DIR% (
	GOTO :argcheck
) ELSE (
	mkdir %BUILD_DIR%
)

:argcheck

cd %BUILD_DIR%

%CMAKE% -S %SOURCE_PATH% -B %BUILD_DIR% -G "MinGW Makefiles" --toolchain "c:/Program Files/Cmake/bin/arm-linux-gnueabihf.cmake" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
%CMAKE% --build .

