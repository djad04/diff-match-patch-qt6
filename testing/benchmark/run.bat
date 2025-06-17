@echo off
echo ========================================
echo  diff-match-patch-qt6 BENCHMARK TEST
echo ========================================
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring benchmark...
cmake .. -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
echo Building benchmark...
cmake --build . --config Release
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

REM Run
echo.
echo Running benchmark...
echo ========================================
if exist Release\benchmark.exe (
    Release\benchmark.exe
) else if exist benchmark.exe (
    benchmark.exe
) else (
    echo Error: benchmark.exe not found!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Benchmark complete!
pause
