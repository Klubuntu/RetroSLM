#include "../include/Historia.h"
#include <iostream>
#include <fstream>
#include <ctime>

using namespace std;

Historia::Historia() {
    sciezkaBazy = "BazaDanych/dane/historia.txt";
    wczytajZPliku();
}

Historia::~Historia() {
    zapiszDoPliku();
}

void Historia::menu() {
    int wybor;
    bool powrot = false;
    
    while (!powrot) {
        cout << "\n=== HISTORIA ===" << endl;
        cout << "1. Wyswietl historie" << endl;
        cout << "2. Filtruj po module" << endl;
        cout << "3. Wyczysc historie" << endl;
        cout << "0. Powrot" << endl;
        cout << "Wybierz opcje: ";
        cin >> wybor;
        
        switch (wybor) {
            case 1:
                wyswietlHistorie();
                break;
            case 2:
                filtrujPoModule();
                break;
            case 3:
                wyczyscHistorie();
                break;
            case 0:
                powrot = true;
                break;
            default:
                cout << "Nieprawidlowa opcja!" << endl;
        }
    }
}

void Historia::dodajWpis(const string& modul, const string& akcja, const string& szczegoly) {
    Wpis nowy;
    
    time_t teraz = time(0);
    tm* czasLokalny = localtime(&teraz);
    
    char buforData[11];
    char buforCzas[9];
    strftime(buforData, 11, "%d.%m.%Y", czasLokalny);
    strftime(buforCzas, 9, "%H:%M:%S", czasLokalny);
    
    nowy.data = buforData;
    nowy.czas = buforCzas;
    nowy.modul = modul;
    nowy.akcja = akcja;
    nowy.szczegoly = szczegoly;
    
    wpisy.push_back(nowy);
    zapiszDoPliku();
}

void Historia::wyswietlHistorie() {
    if (wpisy.empty()) {
        cout << "Historia jest pusta." << endl;
        return;
    }
    
    cout << "\n--- Historia operacji ---" << endl;
    for (const auto& w : wpisy) {
        cout << "[" << w.data << " " << w.czas << "] " 
             << w.modul << " - " << w.akcja;
        if (!w.szczegoly.empty()) {
            cout << " (" << w.szczegoly << ")";
        }
        cout << endl;
    }
}

void Historia::filtrujPoModule() {
    string modul;
    cin.ignore();
    cout << "Podaj nazwe modulu (Magazyn/Klienci/Faktury): ";
    getline(cin, modul);
    
    bool znaleziono = false;
    cout << "\n--- Historia dla modulu: " << modul << " ---" << endl;
    for (const auto& w : wpisy) {
        if (w.modul == modul) {
            cout << "[" << w.data << " " << w.czas << "] " 
                 << w.akcja;
            if (!w.szczegoly.empty()) {
                cout << " (" << w.szczegoly << ")";
            }
            cout << endl;
            znaleziono = true;
        }
    }
    
    if (!znaleziono) {
        cout << "Brak wpisow dla tego modulu." << endl;
    }
}

void Historia::wyczyscHistorie() {
    char potwierdzenie;
    cout << "Czy na pewno chcesz wyczyscic cala historie? (t/n): ";
    cin >> potwierdzenie;
    
    if (potwierdzenie == 't' || potwierdzenie == 'T') {
        wpisy.clear();
        zapiszDoPliku();
        cout << "Historia wyczyszczona." << endl;
    }
}

void Historia::wczytajZPliku() {
    ifstream plik(sciezkaBazy);
    if (!plik.is_open()) {
        return;
    }
    
    Wpis w;
    while (getline(plik, w.data)) {
        getline(plik, w.czas);
        getline(plik, w.modul);
        getline(plik, w.akcja);
        getline(plik, w.szczegoly);
        wpisy.push_back(w);
    }
    plik.close();
}

void Historia::zapiszDoPliku() {
    ofstream plik(sciezkaBazy);
    if (!plik.is_open()) {
        cerr << "Nie mozna otworzyc pliku do zapisu!" << endl;
        return;
    }
    
    for (const auto& w : wpisy) {
        plik << w.data << endl;
        plik << w.czas << endl;
        plik << w.modul << endl;
        plik << w.akcja << endl;
        plik << w.szczegoly << endl;
    }
    plik.close();
}
