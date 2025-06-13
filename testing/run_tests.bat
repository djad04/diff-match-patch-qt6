@echo off
echo Building and running diff-match-patch-qt6 test suite...
echo.

REM Create build directory
if not exist build mkdir build
cd build

REM Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

REM Build
cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Running Test Suite
echo ========================================
echo.

REM Run test suite
Debug\test_suite.exe
if %ERRORLEVEL% neq 0 (
    echo Test suite execution failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo Running Memory Test
echo ========================================
echo.

REM Run memory test
Debug\memory_test.exe
if %ERRORLEVEL% neq 0 (
    echo Memory test execution failed!
    pause
    exit /b 1
)

echo.
echo All tests completed successfully!
pause
