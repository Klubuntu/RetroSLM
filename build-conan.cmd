@echo off
setlocal enabledelayedexpansion

echo === Czyszczę poprzedni folder budowy ===
if exist build rmdir /s /q build

echo === Czyszczę poprzedni folder output ===
if exist output rmdir /s /q output

echo === Tworzę foldery build i output ===
mkdir build
mkdir output

echo === Uruchamiam instalacje: conan ===
conan install

if %errorlevel% neq 0 (
    echo Conan install failed
    exit /b 1
)

echo === Konfiguruje CMake ===
cmake --preset conan-default ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../output ^
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../output ^
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=../output

if %errorlevel% neq 0 (
    echo CMake configuration failed
    exit /b 1
)

echo === Buduje projekt ===
cd build
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo Build failed
    exit /b 1
)

echo === Zakończono budowę sukcesem ===
endlocal
