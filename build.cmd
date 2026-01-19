@echo off
echo Kompilacja RetroSLM dla Windows...
echo.

REM Sprawdz czy g++ jest dostepny
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo BLAD: g++ nie jest dostepny w PATH!
    echo Zainstaluj MinGW-w64 lub MSYS2 z g++
    pause
    exit /b 1
)

REM Utworz katalogi BazaDanych jesli nie istnieja
if not exist "BazaDanych\dane" mkdir BazaDanych\dane
if not exist "BazaDanych\faktury" mkdir BazaDanych\faktury

REM Czyszczenie output/
if exist output rmdir /s /q output
mkdir output

echo.
echo Kompilacja z g++ i FTXUI...
echo.

g++ -std=c++17 ^
    main.cpp ^
    src\Magazyn.cpp ^
    src\Klienci.cpp ^
    src\Faktury.cpp ^
    src\Historia.cpp ^
    -I.\include ^
    -lftxui-component -lftxui-dom -lftxui-screen ^
    -o output\RetroSLM.exe

if %errorlevel% equ 0 (
    echo.
    echo Kompilacja zakonczona pomyslnie!
    echo Uruchamianie programu...
    echo.
    output\RetroSLM.exe
) else (
    echo.
    echo BLAD kompilacji!
    pause
    exit /b 1
)
