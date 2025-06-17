@echo off
echo ========================================
echo  diff-match-patch-qt6 TEST SELECTOR
echo ========================================
echo.
echo Choose which test to run:
echo.
echo 1. Benchmark Test (Performance)
echo 2. Correctness Test (Algorithm validation)
echo 3. Memory Test (Memory usage analysis)
echo 4. All Tests
echo 5. Exit
echo.
set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto benchmark
if "%choice%"=="2" goto correctness
if "%choice%"=="3" goto memory
if "%choice%"=="4" goto all
if "%choice%"=="5" goto exit
echo Invalid choice. Please try again.
pause
goto start

:benchmark
echo.
echo Running Benchmark Test...
cd benchmark
call run.bat
cd ..
goto end

:correctness
echo.
echo Running Correctness Test...
cd correctness
call run.bat
cd ..
goto end

:memory
echo.
echo Running Memory Test...
cd memory
call run.bat
cd ..
goto end

:all
echo.
echo Running All Tests...
echo.
echo 1/3 - Correctness Test
cd correctness
call run.bat
cd ..
echo.
echo 2/3 - Memory Test
cd memory
call run.bat
cd ..
echo.
echo 3/3 - Benchmark Test
cd benchmark
call run.bat
cd ..
goto end

:end
echo.
echo ========================================
echo All selected tests completed!
echo ========================================
pause
goto exit

:exit
