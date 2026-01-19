#!/bin/bash

echo "Kompilacja RetroSLM..."

# Czyszczenie output/
rm -rf output
mkdir output

# Kompilacja bezpośrednia za pomocą g++ z FTXUI
g++ -std=c++17 \
    main.cpp \
    src/Magazyn.cpp \
    src/Klienci.cpp \
    src/Faktury.cpp \
    src/Historia.cpp \
    -I./include \
    -lftxui-component -lftxui-dom -lftxui-screen \
    -o output/RetroSLM

if [ $? -eq 0 ]; then
    echo "Kompilacja zakończona pomyślnie!"
    echo "Uruchamianie programu..."
    echo ""
    ./output/RetroSLM
else
    echo "Błąd kompilacji!"
    exit 1
fi
