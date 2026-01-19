#ifndef HISTORIA_H
#define HISTORIA_H

#include <string>
#include <vector>

using namespace std;

struct Wpis {
    string data;
    string czas;
    string modul;
    string akcja;
    string szczegoly;
};

class Historia {
private:
    vector<Wpis> wpisy;
    string sciezkaBazy;
    
public:
    Historia();
    ~Historia();
    
    void menu();
    void dodajWpis(const string& modul, const string& akcja, const string& szczegoly);
    void wyswietlHistorie();
    void filtrujPoModule();
    void wyczyscHistorie();
    void wczytajZPliku();
    void zapiszDoPliku();
};

#endif
