#ifndef KLIENCI_H
#define KLIENCI_H

#include <string>
#include <vector>
#include <memory>

using namespace std;

class KlientBase {
protected:
    int id;
    string imie;
    string nazwisko;
    string peselNip;
    string kodPocztowy;
    string miejscowosc;
    string ulica;
    string nrDomu;
    
public:
    KlientBase();
    KlientBase(int id, const string& imie, const string& nazwisko, const string& peselNip,
               const string& kodPocztowy, const string& miejscowosc, 
               const string& ulica, const string& nrDomu);
    
    virtual ~KlientBase();
    
    int getId() const { return id; }
    string getImie() const { return imie; }
    string getNazwisko() const { return nazwisko; }
    string getPeselNip() const { return peselNip; }
    string getKodPocztowy() const { return kodPocztowy; }
    string getMiejscowosc() const { return miejscowosc; }
    string getUlica() const { return ulica; }
    string getNrDomu() const { return nrDomu; }
    
    void setId(int newId) { id = newId; }
    void setImie(const string& newImie) { imie = newImie; }
    void setNazwisko(const string& newNazwisko) { nazwisko = newNazwisko; }
    void setPeselNip(const string& newPeselNip) { peselNip = newPeselNip; }
    void setKodPocztowy(const string& newKodPocztowy) { kodPocztowy = newKodPocztowy; }
    void setMiejscowosc(const string& newMiejscowosc) { miejscowosc = newMiejscowosc; }
    void setUlica(const string& newUlica) { ulica = newUlica; }
    void setNrDomu(const string& newNrDomu) { nrDomu = newNrDomu; }
    
    virtual string getTyp() const { return "Klient"; }
    virtual void wyswietlInfo() const;
};

class Klient : public KlientBase {
public:
    Klient();
    Klient(int id, const string& imie, const string& nazwisko, const string& peselNip,
           const string& kodPocztowy, const string& miejscowosc, 
           const string& ulica, const string& nrDomu);
    ~Klient() override;
    
    string getTyp() const override { return "Klient Standardowy"; }
};

class Klienci {
private:
    vector<shared_ptr<KlientBase>> klienci;
    string sciezkaBazy;
    int nastepneId;
    
    int znajdzKlientaPoId(int id) const;
    int znajdzKlientaPoPeselNip(const string& peselNip) const;
    bool czyPeselNipIstnieje(const string& peselNip) const;
    static bool walidujPeselNip(const string& peselNip, string& komunikat);
    
public:
    Klienci();
    ~Klienci();
    
    void menu();
    bool dodajKlienta(const string& imie, const string& nazwisko, const string& peselNip,
                      const string& kodPocztowy, const string& miejscowosc,
                      const string& ulica, const string& nrDomu);
    void wyswietlKlientow() const;
    bool edytujKlienta(int id, const string& imie, const string& nazwisko, const string& peselNip,
                       const string& kodPocztowy, const string& miejscowosc,
                       const string& ulica, const string& nrDomu);
    bool usunKlienta(int id);
    shared_ptr<KlientBase> pobierzKlienta(int id) const;
    shared_ptr<KlientBase> pobierzKlientaPoPeselNip(const string& peselNip) const;
    
    void wczytajZPliku();
    void zapiszDoPliku();
    
    int getLiczbaKlientow() const { return klienci.size(); }
    const vector<shared_ptr<KlientBase>>& getKlienci() const { return klienci; }
};

#endif
