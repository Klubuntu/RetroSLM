#include "../include/Klienci.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;
using json = nlohmann::json;


KlientBase::KlientBase() : id(0), imie(""), nazwisko(""), peselNip(""), 
                           kodPocztowy(""), miejscowosc(""), ulica(""), nrDomu("") {}

KlientBase::KlientBase(int id, const string& imie, const string& nazwisko, const string& peselNip,
                       const string& kodPocztowy, const string& miejscowosc,
                       const string& ulica, const string& nrDomu)
    : id(id), imie(imie), nazwisko(nazwisko), peselNip(peselNip),
      kodPocztowy(kodPocztowy), miejscowosc(miejscowosc), ulica(ulica), nrDomu(nrDomu) {}

KlientBase::~KlientBase() {}

void KlientBase::wyswietlInfo() const {
    cout << "ID: " << id << " | " << imie << " " << nazwisko << endl;
    cout << "  PESEL/NIP: " << peselNip << endl;
    cout << "  Adres: " << kodPocztowy << " " << miejscowosc << ", " 
         << ulica << " " << nrDomu << endl;
}


Klient::Klient() : KlientBase() {}

Klient::Klient(int id, const string& imie, const string& nazwisko, const string& peselNip,
               const string& kodPocztowy, const string& miejscowosc,
               const string& ulica, const string& nrDomu)
    : KlientBase(id, imie, nazwisko, peselNip, kodPocztowy, miejscowosc, ulica, nrDomu) {}

Klient::~Klient() {}


int Klienci::znajdzKlientaPoId(int id) const {
    for (size_t i = 0; i < klienci.size(); i++) {
        if (klienci[i]->getId() == id) {
            return i;
        }
    }
    return -1;
}

int Klienci::znajdzKlientaPoPeselNip(const string& peselNip) const {
    for (size_t i = 0; i < klienci.size(); i++) {
        if (klienci[i]->getPeselNip() == peselNip) {
            return i;
        }
    }
    return -1;
}

bool Klienci::czyPeselNipIstnieje(const string& peselNip) const {
    return znajdzKlientaPoPeselNip(peselNip) != -1;
}

bool Klienci::walidujPeselNip(const string& peselNip, string& komunikat) {
    if (peselNip.empty()) {
        komunikat = "PESEL/NIP nie moze byc pusty!";
        return false;
    }
    
    for (char c : peselNip) {
        if (!isdigit(c)) {
            komunikat = "PESEL/NIP moze zawierac tylko cyfry!";
            return false;
        }
    }
    
    if (peselNip.length() == 11) {
        komunikat = "Poprawny PESEL (11 cyfr)";
        return true;
    } else if (peselNip.length() == 10) {
        komunikat = "Poprawny NIP (10 cyfr)";
        return true;
    } else {
        komunikat = "Nieprawidlowa dlugosc! PESEL: 11 cyfr, NIP: 10 cyfr";
        return false;
    }
}


Klienci::Klienci() {
    sciezkaBazy = "BazaDanych/dane/klienci.json";
    nastepneId = 1;
    wczytajZPliku();
    
    if (klienci.empty()) {
        cout << "Wczytano 0 klientow z bazy danych." << endl;
    } else {
        cout << "Wczytano " << klienci.size() << " klientow z bazy danych." << endl;
    }
}

Klienci::~Klienci() {
    zapiszDoPliku();
}


void Klienci::menu() {
    int wybor;
    bool powrot = false;
    
    while (!powrot) {
        cout << "\n=== KLIENCI ===" << endl;
        cout << "1. Dodaj klienta" << endl;
        cout << "2. Wyswietl klientow" << endl;
        cout << "3. Edytuj klienta" << endl;
        cout << "4. Usun klienta" << endl;
        cout << "0. Powrot" << endl;
        cout << "Wybierz opcje: ";
        cin >> wybor;
        
        switch (wybor) {
            case 1: {
                string imie, nazwisko, peselNip, kodPocztowy, miejscowosc, ulica, nrDomu;
                cin.ignore();
                cout << "Imie: ";
                getline(cin, imie);
                cout << "Nazwisko: ";
                getline(cin, nazwisko);
                
                bool peselNipPoprawny = false;
                while (!peselNipPoprawny) {
                    cout << "PESEL/NIP (PESEL: 11 cyfr, NIP: 10 cyfr): ";
                    getline(cin, peselNip);
                    
                    string komunikat;
                    if (walidujPeselNip(peselNip, komunikat)) {
                        cout << "  " << komunikat << endl;
                        peselNipPoprawny = true;
                    } else {
                        cout << "  Blad: " << komunikat << endl;
                        cout << "  Sprobuj ponownie lub wpisz 'q' aby anulowac: ";
                        string odpowiedz;
                        getline(cin, odpowiedz);
                        if (odpowiedz == "q" || odpowiedz == "Q") {
                            cout << "Anulowano dodawanie klienta." << endl;
                            break;
                        }
                    }
                }
                
                if (!peselNipPoprawny) {
                    break;
                }
                
                cout << "Kod pocztowy: ";
                getline(cin, kodPocztowy);
                cout << "Miejscowosc: ";
                getline(cin, miejscowosc);
                cout << "Ulica: ";
                getline(cin, ulica);
                cout << "Nr domu: ";
                getline(cin, nrDomu);
                
                if (dodajKlienta(imie, nazwisko, peselNip, kodPocztowy, miejscowosc, ulica, nrDomu)) {
                    cout << "Klient dodany pomyslnie!" << endl;
                } else {
                    cout << "Blad: Klient z takim PESEL/NIP juz istnieje!" << endl;
                }
                break;
            }
            case 2:
                wyswietlKlientow();
                break;
            case 3: {
                int id;
                cout << "Podaj ID klienta do edycji: ";
                cin >> id;
                cin.ignore();
                
                string imie, nazwisko, peselNip, kodPocztowy, miejscowosc, ulica, nrDomu;
                cout << "Nowe imie: ";
                getline(cin, imie);
                cout << "Nowe nazwisko: ";
                getline(cin, nazwisko);
                cout << "Nowy PESEL/NIP: ";
                getline(cin, peselNip);
                cout << "Nowy kod pocztowy: ";
                getline(cin, kodPocztowy);
                cout << "Nowa miejscowosc: ";
                getline(cin, miejscowosc);
                cout << "Nowa ulica: ";
                getline(cin, ulica);
                cout << "Nowy nr domu: ";
                getline(cin, nrDomu);
                
                if (edytujKlienta(id, imie, nazwisko, peselNip, kodPocztowy, miejscowosc, ulica, nrDomu)) {
                    cout << "Klient zaktualizowany!" << endl;
                } else {
                    cout << "Blad: Klient o ID " << id << " nie znaleziony." << endl;
                }
                break;
            }
            case 4: {
                int id;
                cout << "Podaj ID klienta do usuniecia: ";
                cin >> id;
                
                if (usunKlienta(id)) {
                    cout << "Klient usuniety!" << endl;
                } else {
                    cout << "Blad: Klient o ID " << id << " nie znaleziony." << endl;
                }
                break;
            }
            case 0:
                powrot = true;
                break;
            default:
                cout << "Nieprawidlowa opcja!" << endl;
        }
    }
}

bool Klienci::dodajKlienta(const string& imie, const string& nazwisko, const string& peselNip,
                           const string& kodPocztowy, const string& miejscowosc,
                           const string& ulica, const string& nrDomu) {
    if (imie.empty() || nazwisko.empty() || peselNip.empty()) {
        return false;
    }
    
    if (czyPeselNipIstnieje(peselNip)) {
        return false;
    }
    
    auto nowyKlient = make_shared<Klient>(nastepneId++, imie, nazwisko, peselNip,
                                          kodPocztowy, miejscowosc, ulica, nrDomu);
    klienci.push_back(nowyKlient);
    zapiszDoPliku();
    
    return true;
}

void Klienci::wyswietlKlientow() const {
    if (klienci.empty()) {
        cout << "Brak klientow w bazie." << endl;
        return;
    }
    
    cout << "\n=== Lista klientow ===" << endl;
    for (const auto& k : klienci) {
        k->wyswietlInfo();
        cout << endl;
    }
    cout << "Lacznie klientow: " << klienci.size() << endl;
}

bool Klienci::edytujKlienta(int id, const string& imie, const string& nazwisko, const string& peselNip,
                            const string& kodPocztowy, const string& miejscowosc,
                            const string& ulica, const string& nrDomu) {
    int index = znajdzKlientaPoId(id);
    if (index == -1) {
        return false;
    }
    
    if (imie.empty() || nazwisko.empty() || peselNip.empty()) {
        return false;
    }
    
    klienci[index]->setImie(imie);
    klienci[index]->setNazwisko(nazwisko);
    klienci[index]->setPeselNip(peselNip);
    klienci[index]->setKodPocztowy(kodPocztowy);
    klienci[index]->setMiejscowosc(miejscowosc);
    klienci[index]->setUlica(ulica);
    klienci[index]->setNrDomu(nrDomu);
    
    zapiszDoPliku();
    return true;
}

bool Klienci::usunKlienta(int id) {
    int index = znajdzKlientaPoId(id);
    if (index == -1) {
        return false;
    }
    
    klienci.erase(klienci.begin() + index);
    zapiszDoPliku();
    return true;
}

shared_ptr<KlientBase> Klienci::pobierzKlienta(int id) const {
    int index = znajdzKlientaPoId(id);
    if (index == -1) {
        return nullptr;
    }
    return klienci[index];
}

shared_ptr<KlientBase> Klienci::pobierzKlientaPoPeselNip(const string& peselNip) const {
    int index = znajdzKlientaPoPeselNip(peselNip);
    if (index == -1) {
        return nullptr;
    }
    return klienci[index];
}


void Klienci::wczytajZPliku() {
    ifstream plik(sciezkaBazy);
    if (!plik.is_open()) {
        cout << "Brak pliku bazy danych, utworze nowy." << endl;
        return;
    }
    
    try {
        json j;
        plik >> j;
        plik.close();
        
        for (const auto& item : j["klienci"]) {
            int itemId = item["id"].get<int>();
            auto klient = make_shared<Klient>(
                itemId,
                item["imie"].get<string>(),
                item["nazwisko"].get<string>(),
                item["peselNip"].get<string>(),
                item["kodPocztowy"].get<string>(),
                item["miejscowosc"].get<string>(),
                item["ulica"].get<string>(),
                item["nrDomu"].get<string>()
            );
            klienci.push_back(klient);
            
            if (itemId >= nastepneId) {
                nastepneId = itemId + 1;
            }
        }
    } catch (const exception& e) {
        cout << "Blad wczytywania danych: " << e.what() << endl;
    }
}

void Klienci::zapiszDoPliku() {
    json j;
    j["klienci"] = json::array();
    
    for (const auto& k : klienci) {
        json klientJson;
        klientJson["id"] = k->getId();
        klientJson["imie"] = k->getImie();
        klientJson["nazwisko"] = k->getNazwisko();
        klientJson["peselNip"] = k->getPeselNip();
        klientJson["kodPocztowy"] = k->getKodPocztowy();
        klientJson["miejscowosc"] = k->getMiejscowosc();
        klientJson["ulica"] = k->getUlica();
        klientJson["nrDomu"] = k->getNrDomu();
        
        j["klienci"].push_back(klientJson);
    }
    
    ofstream plik(sciezkaBazy);
    if (plik.is_open()) {
        plik << j.dump(2);
        plik.close();
    }
}
