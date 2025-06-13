@echo off
echo Running complete diff-match-patch-qt6 test and benchmark suite...
echo.

echo ========================================
echo STEP 1: Running Test Suite
echo ========================================
call run_tests.bat

echo.
echo ========================================
echo STEP 2: Running Performance Benchmark
echo ========================================
call run_benchmark.bat

echo.
echo ========================================
echo ALL TESTS AND BENCHMARKS COMPLETED
echo ========================================
echo.
echo Results summary:
echo - Test Suite: Check above for pass/fail status
echo - Memory Test: Check above for memory usage
echo - Performance Benchmark: Check above for timing results
echo.
echo You can now update the README.md with the actual performance results!
pause
