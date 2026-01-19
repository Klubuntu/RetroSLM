# RetroSLM - System Zarządzania w Stylu MS-DOS

System zarządzania magazynem, klientami, fakturami i historią operacji z interfejsem retro inspirowanym MS-DOS Editor.

## Sterowanie

- **F1** - Pokaż/ukryj pomoc
- **F2** - Zmiana motywu kolorystycznego
- **F3** - Przełączenie focus na górne menu
- **ESC** - Powrót / Wyjście z programu
- **Strzałki ←→** - Nawigacja w menu górnym
- **Strzałki ↑↓** - Nawigacja w menu modułów
- **ENTER** - Potwierdzenie wyboru

## Motywy Kolorystyczne

Program oferuje 5 motywów:
1. **Klasyczny** (granatowo-czarny) - domyślny motyw w stylu MS-DOS
2. **Zielony** (zielono-czarny) - styl terminala matrix
3. **Czerwony** (czerwono-czarny) - taki alarmowy
4. **Niebieski** (niebiesko-czarny) - spokojny
5. **Purpurowy** (fioletowo-czarny) - nowoczesny

Motyw jest automatycznie zapisywany w `BazaDanych/program.ini` i ładowany przy starcie.

## Technologie

- **C++17** - język programowania
- **FTXUI v6.1.9** - biblioteka do interfejsu terminalowego
- **Nix** - zarządzanie środowiskiem i zależnościami

## Wymagania
- Kompilator C++ obsługujący standard C++17 lub nowszy
- Biblioteka FTXUI (dostępna w Nix shell)
- CMake 3.10+ (opcjonalnie)

## Funkcje
- Menu interaktywne z obsługą klawiaturą
- Trwałe przechowywanie danych w plikach tekstowych
- Automatyczny zapis przy wyjściu z programu
- Automatyczne ładowanie danych przy starcie
