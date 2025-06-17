@echo off
echo Running diff-match-patch-qt6 Memory Test...
echo ==========================================

cd build\Desktop_Qt_6_8_2_MSVC2022_64bit-Release
if exist memory_test.exe (
    memory_test.exe
) else (
    echo Error: memory_test.exe not found!
    echo Please build the project first.
    pause
)

pause
