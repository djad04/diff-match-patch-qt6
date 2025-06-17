@echo off
echo ========================================
echo  diff-match-patch-qt6 MEMORY TEST
echo ========================================
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring memory test...
cmake .. -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
echo Building memory test...
cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

REM Run
echo.
echo Running memory analysis...
echo ========================================
if exist Debug\memory_test.exe (
    Debug\memory_test.exe
) else if exist memory_test.exe (
    memory_test.exe
) else (
    echo Error: memory_test.exe not found!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Memory test complete!
pause
