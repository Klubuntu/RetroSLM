#include "../include/Faktury.h"
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

Faktury::Faktury() {
    sciezkaBazy = "BazaDanych/dane/faktury.txt";
    nastepneId = 1;
    wczytajZPliku();
}

Faktury::~Faktury() {
    zapiszDoPliku();
}

void Faktury::menu() {
    int wybor;
    bool powrot = false;
    
    while (!powrot) {
        cout << "\n=== FAKTURY ===" << endl;
        cout << "1. Utworz fakture" << endl;
        cout << "2. Wyswietl faktury" << endl;
        cout << "3. Wyswietl szczegoly faktury" << endl;
        cout << "4. Usun fakture" << endl;
        cout << "0. Powrot" << endl;
        cout << "Wybierz opcje: ";
        cin >> wybor;
        
        switch (wybor) {
            case 1:
                utworzFakture();
                break;
            case 2:
                wyswietlFaktury();
                break;
            case 3:
                wyswietlSzczegoly();
                break;
            case 4:
                usunFakture();
                break;
            case 0:
                powrot = true;
                break;
            default:
                cout << "Nieprawidlowa opcja!" << endl;
        }
    }
}

void Faktury::utworzFakture() {
    Faktura nowa;
    nowa.id = nastepneId++;
    
    cout << "ID klienta: ";
    cin >> nowa.idKlienta;
    
    cin.ignore();
    cout << "Data wystawienia (dd.mm.rrrr): ";
    getline(cin, nowa.dataWystawienia);
    
    nowa.wartoscNetto = 0.0;
    
    char dodajKolejny;
    do {
        PozycjaFaktury pozycja;
        cout << "\nNowa pozycja:" << endl;
        cout << "ID produktu: ";
        cin >> pozycja.idProduktu;
        cin.ignore();
        cout << "Nazwa produktu: ";
        getline(cin, pozycja.nazwaProduktu);
        cout << "Ilosc: ";
        cin >> pozycja.ilosc;
        cout << "Cena za sztuke: ";
        cin >> pozycja.cenaSztuki;
        
        pozycja.wartoscPozycji = pozycja.ilosc * pozycja.cenaSztuki;
        nowa.wartoscNetto += pozycja.wartoscPozycji;
        nowa.pozycje.push_back(pozycja);
        
        cout << "Dodac kolejna pozycje? (t/n): ";
        cin >> dodajKolejny;
    } while (dodajKolejny == 't' || dodajKolejny == 'T');
    
    nowa.podatekVAT = nowa.wartoscNetto * 0.23;
    nowa.wartoscBrutto = nowa.wartoscNetto + nowa.podatekVAT;
    
    listaFaktur.push_back(nowa);
    zapiszDoPliku();
    cout << "Faktura utworzona pomyslnie!" << endl;
}

void Faktury::wyswietlFaktury() {
    if (listaFaktur.empty()) {
        cout << "Brak faktur w bazie." << endl;
        return;
    }
    
    cout << "\n--- Lista faktur ---" << endl;
    cout << fixed << setprecision(2);
    for (const auto& f : listaFaktur) {
        cout << "ID: " << f.id << " | Data: " << f.dataWystawienia 
             << " | Klient ID: " << f.idKlienta 
             << " | Brutto: " << f.wartoscBrutto << " PLN" << endl;
    }
}

void Faktury::wyswietlSzczegoly() {
    int id;
    cout << "Podaj ID faktury: ";
    cin >> id;
    
    for (const auto& f : listaFaktur) {
        if (f.id == id) {
            cout << "\n========== FAKTURA NR " << f.id << " ==========" << endl;
            cout << "Data wystawienia: " << f.dataWystawienia << endl;
            cout << "ID Klienta: " << f.idKlienta << endl;
            cout << "\nPozycje:" << endl;
            
            for (const auto& p : f.pozycje) {
                cout << "  " << p.nazwaProduktu << " (ID: " << p.idProduktu << ")" << endl;
                cout << "    " << p.ilosc << " x " << p.cenaSztuki 
                     << " PLN = " << p.wartoscPozycji << " PLN" << endl;
            }
            
            cout << fixed << setprecision(2);
            cout << "\nWartosc netto: " << f.wartoscNetto << " PLN" << endl;
            cout << "VAT (23%): " << f.podatekVAT << " PLN" << endl;
            cout << "Wartosc brutto: " << f.wartoscBrutto << " PLN" << endl;
            cout << "======================================" << endl;
            return;
        }
    }
    cout << "Faktura o ID " << id << " nie znaleziona." << endl;
}

void Faktury::usunFakture() {
    int id;
    cout << "Podaj ID faktury do usuniecia: ";
    cin >> id;
    
    for (auto it = listaFaktur.begin(); it != listaFaktur.end(); ++it) {
        if (it->id == id) {
            listaFaktur.erase(it);
            zapiszDoPliku();
            cout << "Faktura usunieta!" << endl;
            return;
        }
    }
    cout << "Faktura o ID " << id << " nie znaleziona." << endl;
}

void Faktury::wczytajZPliku() {
    ifstream plik(sciezkaBazy);
    if (!plik.is_open()) {
        return;
    }
    
    Faktura f;
    while (plik >> f.id) {
        plik >> f.idKlienta;
        plik.ignore();
        getline(plik, f.dataWystawienia);
        
        int liczbaPozycji;
        plik >> liczbaPozycji;
        f.pozycje.clear();
        
        for (int i = 0; i < liczbaPozycji; i++) {
            PozycjaFaktury p;
            plik >> p.idProduktu;
            plik.ignore();
            getline(plik, p.nazwaProduktu);
            plik >> p.ilosc >> p.cenaSztuki >> p.wartoscPozycji;
            f.pozycje.push_back(p);
        }
        
        plik >> f.wartoscNetto >> f.podatekVAT >> f.wartoscBrutto;
        listaFaktur.push_back(f);
        
        if (f.id >= nastepneId) {
            nastepneId = f.id + 1;
        }
    }
    plik.close();
}

void Faktury::zapiszDoPliku() {
    ofstream plik(sciezkaBazy);
    if (!plik.is_open()) {
        cerr << "Nie mozna otworzyc pliku do zapisu!" << endl;
        return;
    }
    
    for (const auto& f : listaFaktur) {
        plik << f.id << endl;
        plik << f.idKlienta << endl;
        plik << f.dataWystawienia << endl;
        plik << f.pozycje.size() << endl;
        
        for (const auto& p : f.pozycje) {
            plik << p.idProduktu << endl;
            plik << p.nazwaProduktu << endl;
            plik << p.ilosc << " " << p.cenaSztuki << " " << p.wartoscPozycji << endl;
        }
        
        plik << f.wartoscNetto << " " << f.podatekVAT << " " << f.wartoscBrutto << endl;
    }
    plik.close();
}
