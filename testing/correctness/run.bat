@echo off
echo ========================================
echo  diff-match-patch-qt6 CORRECTNESS TEST
echo ========================================
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
echo Configuring test suite...
cmake .. -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
echo Building test suite...
cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

REM Run
echo.
echo Running correctness tests...
echo ========================================
if exist Debug\test_suite.exe (
    Debug\test_suite.exe
) else if exist test_suite.exe (
    test_suite.exe
) else (
    echo Error: test_suite.exe not found!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Tests complete!
pause
