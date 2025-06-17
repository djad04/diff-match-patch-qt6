@echo off
echo Building and running diff-match-patch-qt6 benchmark...
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Running Performance Benchmark
echo ========================================
echo.

REM Run benchmark
Release\benchmark.exe
if %ERRORLEVEL% neq 0 (
    echo Benchmark execution failed!
    pause
    exit /b 1
)

echo.
echo Benchmark completed successfully!
pause
