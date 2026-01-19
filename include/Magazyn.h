#ifndef MAGAZYN_H
#define MAGAZYN_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

// Klasa bazowa dla produktu - umożliwia polimorfizm
class ProduktBase {
protected:
    int id;
    string nazwa;
    string kod;
    double cena;
    int dostepnaIlosc;
    
public:
    // Konstruktory
    ProduktBase();
    ProduktBase(int id, const string& nazwa, const string& kod, double cena, int ilosc);
    
    // Destruktor wirtualny (ważne dla polimorfizmu)
    virtual ~ProduktBase();
    
    // Gettery (enkapsulacja)
    int getId() const { return id; }
    string getNazwa() const { return nazwa; }
    string getKod() const { return kod; }
    double getCena() const { return cena; }
    int getDostepnaIlosc() const { return dostepnaIlosc; }
    
    // Settery (enkapsulacja z walidacją)
    void setId(int newId) { id = newId; }
    void setNazwa(const string& newNazwa) { nazwa = newNazwa; }
    void setKod(const string& newKod) { kod = newKod; }
    void setCena(double newCena) { if (newCena >= 0) cena = newCena; }
    void setDostepnaIlosc(int newIlosc) { if (newIlosc >= 0) dostepnaIlosc = newIlosc; }
    
    // Metoda wirtualna - może być nadpisana w klasach pochodnych
    virtual string getTyp() const { return "Produkt"; }
    virtual void wyswietlInfo() const;
};

// Klasa pochodna - standardowy produkt
class Produkt : public ProduktBase {
public:
    Produkt();
    Produkt(int id, const string& nazwa, const string& kod, double cena, int ilosc);
    ~Produkt() override;
    
    string getTyp() const override { return "Produkt Standardowy"; }
};

class Magazyn {
private:
    vector<shared_ptr<ProduktBase>> produkty;
    string sciezkaBazy;
    int nastepneId;
    
    // Prywatne metody pomocnicze (enkapsulacja)
    int znajdzProduktPoId(int id) const;
    int znajdzProduktPoKodzie(const string& kod) const;
    bool czyKodIstnieje(const string& kod) const;
    
public:
    // Konstruktor i destruktor
    Magazyn();
    ~Magazyn();
    
    // Główne operacje
    void menu();
    bool dodajProdukt(const string& nazwa, const string& kod, double cena, int ilosc);
    void wyswietlProdukty() const;
    bool edytujProdukt(int id, const string& nowaNazwa, const string& nowyKod, double nowaCena, int nowaIlosc);
    bool usunProdukt(int id);
    shared_ptr<ProduktBase> pobierzProdukt(int id) const;
    shared_ptr<ProduktBase> pobierzProduktPoKodzie(const string& kod) const;
    
    // Operacje na pliku JSON
    void wczytajZPliku();
    void zapiszDoPliku();
    
    // Gettery
    int getLiczbaProdukow() const { return produkty.size(); }
    const vector<shared_ptr<ProduktBase>>& getProdukty() const { return produkty; }
};

#endif
