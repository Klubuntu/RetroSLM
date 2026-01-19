#!/usr/bin/env bash

set -e

echo ">>> Czyszczenie build/"
rm -rf build

echo ">>> Czyszczenie output/"
rm -rf output

echo ">>> Tworzenie build/ i output/"
mkdir build
mkdir output
cd build

echo ">>> Konfiguracja CMake (MinGW Makefiles)"
unset CXXFLAGS
unset CFLAGS
unset CPPFLAGS

cmake -G "MinGW Makefiles" .. \
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=../output \
    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=../output \
    -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=../output

echo ">>> Budowanie"
mingw32-make -j4

echo ">>> Gotowe!"
