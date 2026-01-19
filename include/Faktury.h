#ifndef FAKTURY_H
#define FAKTURY_H

#include <string>
#include <vector>

using namespace std;

struct PozycjaFaktury {
    int idProduktu;
    string nazwaProduktu;
    int ilosc;
    double cenaSztuki;
    double wartoscPozycji;
};

struct Faktura {
    int id;
    int idKlienta;
    string dataWystawienia;
    vector<PozycjaFaktury> pozycje;
    double wartoscNetto;
    double podatekVAT;
    double wartoscBrutto;
};

class Faktury {
private:
    vector<Faktura> listaFaktur;
    string sciezkaBazy;
    int nastepneId;
    
public:
    Faktury();
    ~Faktury();
    
    void menu();
    void utworzFakture();
    void wyswietlFaktury();
    void wyswietlSzczegoly();
    void usunFakture();
    void wczytajZPliku();
    void zapiszDoPliku();
};

#endif
