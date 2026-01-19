#!/usr/bin/env bash
set -e

echo "=== Czyszczę poprzedni folder budowy ==="
rm -rf build

echo "=== Czyszczę poprzedni folder output ==="
rm -rf output

echo "=== Tworzę foldery build i output ==="
mkdir build
mkdir output

echo "=== Uruchamiam instalacje: conan ==="
conan install

echo "=== Konfiguruje CMake ==="
cmake --preset conan-release \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=output \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=output \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=output

echo "=== Buduje projekt ==="
cd build
cmake --build --preset conan-release

echo "=== Zakończono budowę sukcesem ==="
