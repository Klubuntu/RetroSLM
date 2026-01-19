#include "../include/Magazyn.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>

using namespace std;
using json = nlohmann::json;


ProduktBase::ProduktBase() 
    : id(0), nazwa(""), kod(""), cena(0.0), dostepnaIlosc(0) {
}

ProduktBase::ProduktBase(int id, const string& nazwa, const string& kod, double cena, int ilosc)
    : id(id), nazwa(nazwa), kod(kod), cena(cena), dostepnaIlosc(ilosc) {
    if (cena < 0) this->cena = 0;
    if (ilosc < 0) this->dostepnaIlosc = 0;
}

ProduktBase::~ProduktBase() {
}

void ProduktBase::wyswietlInfo() const {
    cout << "ID: " << id 
         << " | Kod: " << kod
         << " | " << nazwa 
         << " | Cena: " << cena << " PLN"
         << " | Dostępna ilość: " << dostepnaIlosc << " szt." << endl;
}


Produkt::Produkt() : ProduktBase() {
}

Produkt::Produkt(int id, const string& nazwa, const string& kod, double cena, int ilosc)
    : ProduktBase(id, nazwa, kod, cena, ilosc) {
}

Produkt::~Produkt() {
}


Magazyn::Magazyn() {
    sciezkaBazy = "BazaDanych/dane/produkty.json";
    nastepneId = 1;
    wczytajZPliku();
}

Magazyn::~Magazyn() {
    zapiszDoPliku();
}

int Magazyn::znajdzProduktPoId(int id) const {
    for (size_t i = 0; i < produkty.size(); i++) {
        if (produkty[i]->getId() == id) {
            return i;
        }
    }
    return -1;
}

int Magazyn::znajdzProduktPoKodzie(const string& kod) const {
    for (size_t i = 0; i < produkty.size(); i++) {
        if (produkty[i]->getKod() == kod) {
            return i;
        }
    }
    return -1;
}

bool Magazyn::czyKodIstnieje(const string& kod) const {
    return znajdzProduktPoKodzie(kod) != -1;
}

bool Magazyn::dodajProdukt(const string& nazwa, const string& kod, double cena, int ilosc) {
    if (nazwa.empty() || kod.empty()) {
        cout << "Błąd: Nazwa i kod produktu nie mogą być puste!" << endl;
        return false;
    }
    
    if (czyKodIstnieje(kod)) {
        cout << "Błąd: Produkt o kodzie " << kod << " już istnieje!" << endl;
        return false;
    }
    
    if (cena < 0 || ilosc < 0) {
        cout << "Błąd: Cena i ilość muszą być nieujemne!" << endl;
        return false;
    }
    
    auto nowyProdukt = make_shared<Produkt>(nastepneId++, nazwa, kod, cena, ilosc);
    produkty.push_back(nowyProdukt);
    zapiszDoPliku();
    
    cout << "Produkt '" << nazwa << "' został dodany pomyślnie!" << endl;
    return true;
}

void Magazyn::wyswietlProdukty() const {
    if (produkty.empty()) {
        cout << "Brak produktów w magazynie." << endl;
        return;
    }
    
    cout << "\n=== Lista produktów ===" << endl;
    for (const auto& p : produkty) {
        p->wyswietlInfo();
    }
    cout << "\nŁącznie produktów: " << produkty.size() << endl;
}

bool Magazyn::edytujProdukt(int id, const string& nowaNazwa, const string& nowyKod, 
                            double nowaCena, int nowaIlosc) {
    int index = znajdzProduktPoId(id);
    if (index == -1) {
        cout << "Błąd: Nie znaleziono produktu o ID " << id << endl;
        return false;
    }
    
    if (nowyKod != produkty[index]->getKod() && czyKodIstnieje(nowyKod)) {
        cout << "Błąd: Kod " << nowyKod << " jest już używany!" << endl;
        return false;
    }
    
    produkty[index]->setNazwa(nowaNazwa);
    produkty[index]->setKod(nowyKod);
    produkty[index]->setCena(nowaCena);
    produkty[index]->setDostepnaIlosc(nowaIlosc);
    
    zapiszDoPliku();
    cout << "Produkt zaktualizowany pomyślnie!" << endl;
    return true;
}

bool Magazyn::usunProdukt(int id) {
    int index = znajdzProduktPoId(id);
    if (index == -1) {
        cout << "Błąd: Nie znaleziono produktu o ID " << id << endl;
        return false;
    }
    
    string nazwa = produkty[index]->getNazwa();
    produkty.erase(produkty.begin() + index);
    zapiszDoPliku();
    
    cout << "Produkt '" << nazwa << "' został usunięty." << endl;
    return true;
}

shared_ptr<ProduktBase> Magazyn::pobierzProdukt(int id) const {
    int index = znajdzProduktPoId(id);
    if (index == -1) {
        return nullptr;
    }
    return produkty[index];
}

shared_ptr<ProduktBase> Magazyn::pobierzProduktPoKodzie(const string& kod) const {
    int index = znajdzProduktPoKodzie(kod);
    if (index == -1) {
        return nullptr;
    }
    return produkty[index];
}

void Magazyn::wczytajZPliku() {
    ifstream plik(sciezkaBazy);
    if (!plik.is_open()) {
        cout << "Brak pliku bazy danych, utworzę nowy." << endl;
        return;
    }
    
    try {
        json j;
        plik >> j;
        plik.close();
        
        if (j.contains("produkty") && j["produkty"].is_array()) {
            for (const auto& item : j["produkty"]) {
                auto produkt = make_shared<Produkt>(
                    item["id"].get<int>(),
                    item["nazwa"].get<string>(),
                    item["kod"].get<string>(),
                    item["cena"].get<double>(),
                    item["dostepnaIlosc"].get<int>()
                );
                produkty.push_back(produkt);
                
                int itemId = item["id"].get<int>();
                if (itemId >= nastepneId) {
                    nastepneId = itemId + 1;
                }
            }
        }
        
        cout << "Wczytano " << produkty.size() << " produktów z bazy danych." << endl;
    }
    catch (const exception& e) {
        cout << "Błąd podczas wczytywania: " << e.what() << endl;
    }
}

void Magazyn::zapiszDoPliku() {
    json j;
    json produktyArray = json::array();
    
    for (const auto& p : produkty) {
        json produktJson;
        produktJson["id"] = p->getId();
        produktJson["nazwa"] = p->getNazwa();
        produktJson["kod"] = p->getKod();
        produktJson["cena"] = p->getCena();
        produktJson["dostepnaIlosc"] = p->getDostepnaIlosc();
        produktJson["typ"] = p->getTyp();
        
        produktyArray.push_back(produktJson);
    }
    
    j["produkty"] = produktyArray;
    j["ostatnieId"] = nastepneId - 1;
    
    ofstream plik(sciezkaBazy);
    if (plik.is_open()) {
        plik << j.dump(2);
        plik.close();
    }
}

void Magazyn::menu() {
    int wybor;
    bool powrot = false;
    
    while (!powrot) {
        cout << "\n=== MAGAZYN/PRODUKTY ===" << endl;
        cout << "1. Dodaj produkt" << endl;
        cout << "2. Wyświetl produkty" << endl;
        cout << "3. Edytuj produkt" << endl;
        cout << "4. Usuń produkt" << endl;
        cout << "0. Powrót" << endl;
        cout << "Wybierz opcję: ";
        cin >> wybor;
        
        switch (wybor) {
            case 1: {
                string nazwa, kod;
                double cena;
                int ilosc;
                
                cin.ignore();
                cout << "Nazwa produktu: ";
                getline(cin, nazwa);
                cout << "Kod produktu: ";
                getline(cin, kod);
                cout << "Cena (PLN): ";
                cin >> cena;
                cout << "Dostępna ilość: ";
                cin >> ilosc;
                
                dodajProdukt(nazwa, kod, cena, ilosc);
                break;
            }
            case 2:
                wyswietlProdukty();
                break;
            case 3: {
                int id;
                string nazwa, kod;
                double cena;
                int ilosc;
                
                cout << "Podaj ID produktu do edycji: ";
                cin >> id;
                
                auto produkt = pobierzProdukt(id);
                if (produkt) {
                    cin.ignore();
                    cout << "Nowa nazwa [" << produkt->getNazwa() << "]: ";
                    getline(cin, nazwa);
                    cout << "Nowy kod [" << produkt->getKod() << "]: ";
                    getline(cin, kod);
                    cout << "Nowa cena [" << produkt->getCena() << "]: ";
                    cin >> cena;
                    cout << "Nowa ilość [" << produkt->getDostepnaIlosc() << "]: ";
                    cin >> ilosc;
                    
                    if (nazwa.empty()) nazwa = produkt->getNazwa();
                    if (kod.empty()) kod = produkt->getKod();
                    
                    edytujProdukt(id, nazwa, kod, cena, ilosc);
                }
                break;
            }
            case 4: {
                int id;
                cout << "Podaj ID produktu do usunięcia: ";
                cin >> id;
                usunProdukt(id);
                break;
            }
            case 0:
                powrot = true;
                break;
            default:
                cout << "Nieprawidłowa opcja!" << endl;
        }
    }
}
