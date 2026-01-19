#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <ctime>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

#include "include/Magazyn.h"
#include "include/Klienci.h"
#include "include/Faktury.h"
#include "include/Historia.h"

using namespace ftxui;
using namespace std;

struct Theme {
    Color menuBarBg;
    Color menuBarFg;
    Color contentBg;
    Color contentFg;
    Color statusBarBg;
    Color statusBarFg;
    Color accentColor;
    string name;
};

vector<Theme> themes = {
    {Color::NavyBlue, Color::White, Color::Black, Color::White, Color::GrayDark, Color::White, Color::Cyan, "Klasyczny"},
    {Color::Green, Color::Black, Color::Black, Color::GreenLight, Color::GrayDark, Color::GreenLight, Color::Yellow, "Zielony"},
    {Color::Red, Color::White, Color::Black, Color::RedLight, Color::GrayDark, Color::White, Color::Yellow, "Czerwony"},
    {Color::Blue, Color::White, Color::Black, Color::CyanLight, Color::GrayDark, Color::White, Color::Yellow, "Niebieski"},
    {Color::Magenta, Color::White, Color::Black, Color::MagentaLight, Color::GrayDark, Color::White, Color::Cyan, "Purpurowy"}
};

void zapiszKonfiguracje(int themId) {
    ofstream plik("BazaDanych/program.ini");
    if (plik.is_open()) {
        plik << "[theme]" << endl;
        plik << "color=" << themId << endl;
        plik.close();
    }
}

int wczytajKonfiguracje() {
    ifstream plik("BazaDanych/program.ini");
    if (plik.is_open()) {
        string line;
        bool inThemeSection = false;
        while (getline(plik, line)) {
            if (line == "[theme]") {
                inThemeSection = true;
                continue;
            }
            if (inThemeSection && line.find("color=") == 0) {
                int id = stoi(line.substr(6));
                plik.close();
                return id;
            }
        }
        plik.close();
    }
    return 0;
}

void zapiszHistorie(const string& akcja, const string& szczegoly) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    char dataBuffer[20];
    sprintf(dataBuffer, "%04d-%02d-%02d %02d:%02d:%02d",
            1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
            ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
    
    ofstream plik("BazaDanych/dane/historia.txt", ios::app);
    if (plik.is_open()) {
        plik << akcja << " | " << szczegoly << " | " << dataBuffer << "\n";
        plik.close();
    }
}

vector<string> wczytajHistorie() {
    vector<string> wpisy;
    ifstream plik("BazaDanych/dane/historia.txt");
    
    if (plik.is_open()) {
        string linia;
        while (getline(plik, linia)) {
            if (!linia.empty()) {
                wpisy.push_back(linia);
            }
        }
        plik.close();
    }
    
    return wpisy;
}

int main() {
    auto screen = ScreenInteractive::Fullscreen();
    
    Magazyn magazyn;
    Klienci klienci;
    Faktury faktury;
    Historia historia;
    
    bool pokazDialog = true;
    bool pokazHelp = false;
    bool pokazMotywy = false;
    bool pokazWyjscie = false;
    int selectedWyjscieOption = 0;
    int currentTheme = wczytajKonfiguracje();
    int selectedTheme = currentTheme;
    int wybranyModul = 0;
    int selectedMenu = 0;
    int selectedMagazynOption = 0;
    int selectedKlienciOption = 0;
    int selectedFakturyOption = 0;
    int selectedHistoriaOption = 0;
    bool focusOnTopMenu = false;
    
    bool pokazMagazynDialog = false;
    bool pokazListeProdukow = false;
    bool pokazPotwierdzUsun = false;
    int magazynDialogTyp = 0;
    int wybranyProduktIdx = 0;
    int selectedPotwierdzOption = 0;
    string produktNazwa = "";
    string produktKod = "";
    string produktCena = "";
    string produktIlosc = "";
    string produktId = "";
    string magazynKomunikat = "";
    
    bool pokazKlienciDialog = false;
    bool pokazListeKlientow = false;
    bool pokazPotwierdzUsunKlienta = false;
    int klienciDialogTyp = 0;
    int wybranyKlientIdx = 0;
    int selectedPotwierdzKlientOption = 0;
    string klientImie = "";
    string klientNazwisko = "";
    string klientPeselNip = "";
    string klientKodPocztowy = "";
    string klientMiejscowosc = "";
    string klientUlica = "";
    string klientNrDomu = "";
    string klientId = "";
    string klienciKomunikat = "";
    
    bool pokazListeKlientowFaktura = false;
    bool pokazPotwierdzWystawFakture = false;
    bool pokazDialogFaktury = false;
    int wybranyKlientFakturaIdx = 0;
    int selectedPotwierdzFakturaOption = 0;
    string fakturaKodProduktu = "";
    string fakturaIlosc = "";
    string fakturaNazwaProduktu = "";
    string fakturaCenaJednostkowa = "";
    string fakturaWartoscCalkowita = "";
    string fakturaKomunikat = "";
    int wybranyKlientIdFaktura = 0;
    string wybranyKlientNazwaFaktura = "";
    
    bool pokazListeHistorii = false;
    int wybranyWpisHistoriiIdx = 0;
    vector<string> wpisyHistorii;
    
    std::vector<std::string> menuEntries = {
        "Program", "Magazyn", "Klienci", "Faktury", "Historia"
    };
    
    std::vector<std::string> magazynOptions = {
        "Dodaj produkt",
        "Wyswietl produkty",
        "Edytuj produkt",
        "Usun produkt"
    };
    
    std::vector<std::string> klienciOptions = {
        "Dodaj klienta",
        "Wyswietl klientow",
        "Edytuj klienta",
        "Usun klienta"
    };
    
    std::vector<std::string> fakturyOptions = {
        "Utworz fakture",
        "Wyswietl faktury",
        "Wyswietl szczegoly",
        "Usun fakture"
    };
    
    std::vector<std::string> historiaOptions = {
        "Wyswietl historie",
        "Filtruj po module",
        "Wyczysc historie"
    };
    
    auto magazynMenu = Menu(&magazynOptions, &selectedMagazynOption);
    auto klienciMenu = Menu(&klienciOptions, &selectedKlienciOption);
    auto fakturyMenu = Menu(&fakturyOptions, &selectedFakturyOption);
    auto historiaMenu = Menu(&historiaOptions, &selectedHistoriaOption);
    
    auto inputNazwa = Input(&produktNazwa, "Nazwa produktu");
    auto inputKod = Input(&produktKod, "Kod produktu");
    auto inputCena = Input(&produktCena, "Cena");
    auto inputIlosc = Input(&produktIlosc, "Ilosc");
    auto inputId = Input(&produktId, "ID produktu");
    
    auto inputKlientImie = Input(&klientImie, "Imie");
    auto inputKlientNazwisko = Input(&klientNazwisko, "Nazwisko");
    auto inputKlientPeselNip = Input(&klientPeselNip, "PESEL/NIP");
    auto inputKlientKodPocztowy = Input(&klientKodPocztowy, "Kod pocztowy");
    auto inputKlientMiejscowosc = Input(&klientMiejscowosc, "Miejscowosc");
    auto inputKlientUlica = Input(&klientUlica, "Ulica");
    auto inputKlientNrDomu = Input(&klientNrDomu, "Nr domu");
    auto inputKlientId = Input(&klientId, "ID klienta");
    
    auto inputFakturaKodProduktu = Input(&fakturaKodProduktu, "Kod produktu");
    auto inputFakturaIlosc = Input(&fakturaIlosc, "Ilosc");
    
    auto fakturaInputContainer = Container::Vertical({
        inputFakturaKodProduktu,
        inputFakturaIlosc,
    });
    
    vector<string> potwierdzOptions = {"NIE", "TAK"};
    auto potwierdzMenu = Menu(&potwierdzOptions, &selectedPotwierdzOption);
    auto potwierdzKlientMenu = Menu(&potwierdzOptions, &selectedPotwierdzKlientOption);
    auto potwierdzFakturaMenu = Menu(&potwierdzOptions, &selectedPotwierdzFakturaOption);
    
    auto programScreen = Renderer([&] {
        return vbox({
            text("") | center,
            text("System Zarzadzania RetroSLM") | bold | color(Color::Cyan) | center,
            text("") | center,
            separator(),
            text(""),
            text("Wybierz modul z gornego menu:") | center,
            text(""),
            hbox({
                text("  "),
                vbox({
                    text("MAGAZYN") | bold | color(Color::Yellow),
                    text("Zarzadzanie produktami"),
                    text("i stanem magazynowym") | dim,
                }),
                text("     "),
                vbox({
                    text("KLIENCI") | bold | color(Color::Yellow),
                    text("Baza danych klientow"),
                    text("i ich informacji") | dim,
                }),
            }) | center,
            text(""),
            hbox({
                text("  "),
                vbox({
                    text("FAKTURY") | bold | color(Color::Yellow),
                    text("Tworzenie i przegladanie"),
                    text("dokumentow sprzedazy") | dim,
                }),
                text("     "),
                vbox({
                    text("HISTORIA") | bold | color(Color::Yellow),
                    text("Log wszystkich operacji"),
                    text("wykonanych w systemie") | dim,
                }),
            }) | center,
            text(""),
            separator(),
            text(""),
            text("Nawigacja:") | bold,
            text("  Strzalki <-> : poruszanie sie po menu gornym"),
            text("  ENTER       : wybor modulu / akcji"),
            text("  ESC         : wyjscie z programu / powrot"),
        }) | border | center;
    });
    
    auto magazynRenderer = Renderer(magazynMenu, [&] {
        return vbox({
            text("MAGAZYN / PRODUKTY") | bold | color(Color::Yellow) | center,
            text("") | center,
            separator(),
            text(""),
            magazynMenu->Render() | border | center | size(WIDTH, EQUAL, 40),
            text(""),
            separator(),
            text(""),
            text("Uzyj strzalek GORA/DOL do nawigacji") | dim | center,
            text("ENTER - potwierdz wybor | ESC - powrot") | dim | center,
        }) | center;
    });
    
    auto klienciRenderer = Renderer(klienciMenu, [&] {
        return vbox({
            text("KLIENCI") | bold | color(Color::Yellow) | center,
            text("") | center,
            separator(),
            text(""),
            klienciMenu->Render() | border | center | size(WIDTH, EQUAL, 40),
            text(""),
            separator(),
            text(""),
            text("Uzyj strzalek GORA/DOL do nawigacji") | dim | center,
            text("ENTER - potwierdz wybor | ESC - powrot") | dim | center,
        }) | center;
    });
    
    auto fakturyRenderer = Renderer(fakturyMenu, [&] {
        return vbox({
            text("FAKTURY - Wybierz klienta") | bold | color(Color::Yellow) | center,
            text("") | center,
            separator(),
            text(""),
            text("< Nacisnij ENTER - aby wejsc do wystawiania faktur >") | center | dim,
        }) | center;
    });
    
    auto historiaRenderer = Renderer(historiaMenu, [&] {
        if (pokazListeHistorii) {
            const Theme& theme = themes[currentTheme];
            Elements rows;
            
            rows.push_back(text("HISTORIA OPERACJI") | bold | color(theme.accentColor) | center);
            rows.push_back(text("") | center);
            rows.push_back(separator() | color(theme.accentColor));
            rows.push_back(text(""));
            
            if (wpisyHistorii.empty()) {
                rows.push_back(text("Brak wpisow w historii") | dim | center);
            } else {
                Elements headerRow;
                headerRow.push_back(text("AKCJA") | bold | color(theme.accentColor) | size(WIDTH, EQUAL, 25));
                headerRow.push_back(separator() | color(theme.accentColor));
                headerRow.push_back(text("SZCZEGOLY") | bold | color(theme.accentColor) | size(WIDTH, EQUAL, 40));
                headerRow.push_back(separator() | color(theme.accentColor));
                headerRow.push_back(text("DATA") | bold | color(theme.accentColor) | size(WIDTH, EQUAL, 20));
                rows.push_back(hbox(headerRow) | center);
                rows.push_back(separator() | color(theme.accentColor));
                
                for (size_t i = 0; i < wpisyHistorii.size(); i++) {
                    string wpis = wpisyHistorii[i];
                    
                    size_t pos1 = wpis.find(" | ");
                    size_t pos2 = wpis.rfind(" | ");
                    
                    string akcja = (pos1 != string::npos) ? wpis.substr(0, pos1) : wpis;
                    string szczegoly = (pos1 != string::npos && pos2 != string::npos && pos2 > pos1) 
                                        ? wpis.substr(pos1 + 3, pos2 - pos1 - 3) : "";
                    string data = (pos2 != string::npos && pos2 > pos1) ? wpis.substr(pos2 + 3) : "";
                    
                    Elements rowElements;
                    rowElements.push_back(text(akcja) | color(theme.contentFg) | size(WIDTH, EQUAL, 25));
                    rowElements.push_back(separator() | color(theme.accentColor));
                    rowElements.push_back(text(szczegoly) | color(theme.contentFg) | size(WIDTH, EQUAL, 40));
                    rowElements.push_back(separator() | color(theme.accentColor));
                    rowElements.push_back(text(data) | color(theme.contentFg) | size(WIDTH, EQUAL, 20));
                    
                    auto row = hbox(rowElements);
                    
                    if ((int)i == wybranyWpisHistoriiIdx) {
                        row = row | bgcolor(theme.accentColor) | color(Color::Black) | bold;
                    }
                    
                    rows.push_back(row);
                }
            }
            
            rows.push_back(text(""));
            rows.push_back(separator() | color(theme.accentColor));
            rows.push_back(text(""));
            rows.push_back(text("Uzyj strzalek GORA/DOL do przewijania") | dim | color(theme.contentFg) | center);
            rows.push_back(text("ESC - powrot") | dim | color(theme.contentFg) | center);
            
            return vbox(rows) | center | border | color(theme.accentColor) | bgcolor(theme.contentBg);
        }
        
        return vbox({
            text("HISTORIA") | bold | color(Color::Yellow) | center,
            text("") | center,
            separator(),
            text(""),
            historiaMenu->Render() | border | center | size(WIDTH, EQUAL, 40),
            text(""),
            separator(),
            text(""),
            text("Uzyj strzalek GORA/DOL do nawigacji") | dim | center,
            text("ENTER - potwierdz wybor | ESC - powrot") | dim | center,
        }) | center;
    });
    
    int modulTab = 0;
    auto modulScreens = Container::Tab({
        programScreen,
        magazynRenderer,
        klienciRenderer,
        fakturyRenderer,
        historiaRenderer,
    }, &modulTab);
    
    auto menuBar = Menu(&menuEntries, &selectedMenu, MenuOption::Horizontal());
    
    int activeContainer = 0;
    auto mainContainer = Container::Tab({
        modulScreens,
        menuBar,
    }, &activeContainer);
    
    auto mainRenderer = Renderer(mainContainer, [&] {
        modulTab = wybranyModul;
        Theme& theme = themes[currentTheme];
        
        return vbox({
            hbox({
                text(" ") | bgcolor(theme.menuBarBg) | color(theme.menuBarFg),
                menuBar->Render() | bgcolor(theme.menuBarBg) | color(theme.menuBarFg) | flex,
            }) | (activeContainer == 1 ? inverted : nothing),
            separator(),
            modulScreens->Render() | flex | bgcolor(theme.contentBg) | color(theme.contentFg),
            separator(),
            hbox({
                text(" F1=Help") | bgcolor(theme.statusBarBg) | color(theme.statusBarFg),
                text(" F3=Menu") | bgcolor(theme.statusBarBg) | color(theme.statusBarFg),
                text(" F8=Motyw") | bgcolor(theme.statusBarBg) | color(theme.statusBarFg),
                text(" ") | bgcolor(theme.statusBarBg) | flex,
                text("Ctrl+Q=Wyjscie | ESC=Powrot ") | bgcolor(theme.statusBarBg) | color(theme.statusBarFg),
            }),
        });
    });
    
    mainRenderer |= CatchEvent([&](Event event) {
        if (event == Event::Escape) {
            if (activeContainer == 1) {
                activeContainer = 0;
                return true;
            } else if (wybranyModul != 0) {
                wybranyModul = 0;
                selectedMenu = 0;
                modulTab = 0;
                pokazMagazynDialog = false;
                pokazListeProdukow = false;
                pokazKlienciDialog = false;
                pokazListeKlientow = false;
                pokazListeKlientowFaktura = false;
                pokazDialogFaktury = false;
                pokazPotwierdzWystawFakture = false;
                return true;
            }
            return true;
        }
        
        if (event == Event::Return && activeContainer == 1) {
            wybranyModul = selectedMenu;
            modulTab = selectedMenu;
            activeContainer = 0;
            
            selectedMagazynOption = 0;
            selectedKlienciOption = 0;
            selectedFakturyOption = 0;
            selectedHistoriaOption = 0;
            
            pokazMagazynDialog = false;
            pokazListeProdukow = false;
            pokazKlienciDialog = false;
            pokazListeKlientow = false;
            pokazListeKlientowFaktura = false;
            pokazDialogFaktury = false;
            pokazPotwierdzWystawFakture = false;
            wybranyKlientFakturaIdx = 0;
            
            return true;
        }
        
        if (event == Event::Return && activeContainer == 0 && wybranyModul > 0) {
            switch(wybranyModul) {
                case 1:
                    switch(selectedMagazynOption) {
                        case 0:
                            produktNazwa = "";
                            produktKod = "";
                            produktCena = "";
                            produktIlosc = "";
                            magazynDialogTyp = 0;
                            pokazMagazynDialog = true;
                            break;
                        case 1:
                            pokazListeProdukow = true;
                            wybranyProduktIdx = 0;
                            break;
                        case 2:
                            produktId = "";
                            produktNazwa = "";
                            produktKod = "";
                            produktCena = "";
                            produktIlosc = "";
                            magazynDialogTyp = 1;
                            pokazMagazynDialog = true;
                            break;
                        case 3:
                            produktId = "";
                            magazynDialogTyp = 2;
                            pokazMagazynDialog = true;
                            break;
                    }
                    break;
                case 2:
                    switch(selectedKlienciOption) {
                        case 0:
                            klientImie = "";
                            klientNazwisko = "";
                            klientPeselNip = "";
                            klientKodPocztowy = "";
                            klientMiejscowosc = "";
                            klientUlica = "";
                            klientNrDomu = "";
                            klienciDialogTyp = 0;
                            pokazKlienciDialog = true;
                            break;
                        case 1:
                            pokazListeKlientow = true;
                            wybranyKlientIdx = 0;
                            break;
                        case 2:
                            klientId = "";
                            klientImie = "";
                            klientNazwisko = "";
                            klientPeselNip = "";
                            klientKodPocztowy = "";
                            klientMiejscowosc = "";
                            klientUlica = "";
                            klientNrDomu = "";
                            klienciDialogTyp = 1;
                            pokazKlienciDialog = true;
                            break;
                        case 3:
                            klientId = "";
                            klienciDialogTyp = 2;
                            pokazKlienciDialog = true;
                            break;
                    }
                    break;
                case 3:
                    pokazListeKlientowFaktura = true;
                    wybranyKlientFakturaIdx = 0;
                    break;
                case 4:
                    wpisyHistorii = wczytajHistorie();
                    pokazListeHistorii = true;
                    wybranyWpisHistoriiIdx = 0;
                    break;
            }
            return true;
        }
        
        return false;
    });
    
    vector<string> themeNames;
    for (const auto& t : themes) {
        themeNames.push_back(t.name);
    }
    auto themeMenu = Menu(&themeNames, &selectedTheme);
    
    vector<string> wyjscieOptions = {"NIE", "TAK"};
    auto wyjscieMenu = Menu(&wyjscieOptions, &selectedWyjscieOption);
    
    auto magazynInputContainer = Container::Vertical({
        inputId,
        inputNazwa,
        inputKod,
        inputCena,
        inputIlosc,
    });
    
    auto klienciInputContainer = Container::Vertical({
        inputKlientId,
        inputKlientImie,
        inputKlientNazwisko,
        inputKlientPeselNip,
        inputKlientKodPocztowy,
        inputKlientMiejscowosc,
        inputKlientUlica,
        inputKlientNrDomu,
    });
    
    int dialogActive = 0;
    auto themeContainer = Container::Tab({
        mainRenderer,
        themeMenu,
        wyjscieMenu,
        magazynInputContainer,
        potwierdzMenu,
        klienciInputContainer,
        potwierdzKlientMenu,
        fakturaInputContainer,
        potwierdzFakturaMenu,
    }, &dialogActive);
    
    auto finalRenderer = Renderer(themeContainer, [&] {
        auto main = mainRenderer->Render();
        Theme& theme = themes[currentTheme];
        
        if (pokazMotywy) {
            dialogActive = 1;
            auto themeBox = vbox({
                text("=== WYBOR MOTYWU ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                themeMenu->Render() | center,
                text(""),
                separator(),
                text(""),
                text("ENTER - wybierz | ESC - anuluj") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 40) | size(HEIGHT, EQUAL, 18) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                themeBox | clear_under | center,
            });
        } else if (pokazWyjscie) {
            dialogActive = 2;
            auto exitBox = vbox({
                text("=== ZAMYKANIE PROGRAMU ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                text("Czy na pewno chcesz zamknac program?") | center | color(theme.contentFg),
                text(""),
                wyjscieMenu->Render() | center,
                text(""),
                separator(),
                text(""),
                text("ENTER - potwierdz | ESC - anuluj") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 50) | size(HEIGHT, EQUAL, 16) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                exitBox | clear_under | center | bgcolor(theme.contentBg),
            });
        } else {
            dialogActive = 0;
        }
        
        if (pokazListeProdukow) {
            vector<Element> produktyLista;
            const auto& produkty = magazyn.getProdukty();
            
            if (produkty.empty()) {
                produktyLista.push_back(text("Brak produktow w magazynie") | center | color(Color::Yellow));
            } else {
                produktyLista.push_back(
                    hbox({
                        text("ID") | bold | size(WIDTH, EQUAL, 6),
                        separator(),
                        text("Nazwa") | bold | size(WIDTH, EQUAL, 25),
                        separator(),
                        text("Kod") | bold | size(WIDTH, EQUAL, 15),
                        separator(),
                        text("Cena") | bold | size(WIDTH, EQUAL, 12),
                        separator(),
                        text("Ilosc") | bold | size(WIDTH, EQUAL, 10),
                    }) | bgcolor(theme.accentColor)
                );
                produktyLista.push_back(separator());
                
                for (size_t i = 0; i < produkty.size(); i++) {
                    const auto& p = produkty[i];
                    
                    stringstream cenaStream;
                    cenaStream << fixed << setprecision(2) << p->getCena();
                    string cenaFormatowana = cenaStream.str() + " zl";
                    
                    auto produktRow = hbox({
                        text(to_string(p->getId())) | size(WIDTH, EQUAL, 6),
                        separator(),
                        text(p->getNazwa()) | size(WIDTH, EQUAL, 25),
                        separator(),
                        text(p->getKod()) | size(WIDTH, EQUAL, 15),
                        separator(),
                        text(cenaFormatowana) | size(WIDTH, EQUAL, 12),
                        separator(),
                        text(to_string(p->getDostepnaIlosc()) + " szt") | size(WIDTH, EQUAL, 10),
                    });
                    
                    if ((int)i == wybranyProduktIdx) {
                        produktRow = produktRow | bgcolor(theme.accentColor) | color(Color::Black) | bold;
                    }
                    
                    produktyLista.push_back(produktRow);
                }
            }
            
            auto listaBox = vbox({
                text("=== LISTA PRODUKTOW ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                vbox(produktyLista) | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 20),
                text(""),
                separator(),
                text(""),
                text("Lacznie produktow: " + to_string(produkty.size())) | center | color(theme.contentFg),
                text(""),
                text("E - edytuj | DEL - usun | ESC - powrot") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 85) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                listaBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazMagazynDialog) {
            dialogActive = 3;
            Element magazynDialogBox;
            
            if (magazynDialogTyp == 0) {
                magazynDialogBox = vbox({
                    text("=== DODAJ PRODUKT ===") | bold | center | color(theme.accentColor),
                    text(""),
                    separator(),
                    text(""),
                    hbox({text("Nazwa:  ") | color(theme.menuBarFg), inputNazwa->Render() | color(theme.contentFg)}),
                    hbox({text("Kod:    ") | color(theme.menuBarFg), inputKod->Render() | color(theme.contentFg)}),
                    hbox({text("Cena:   ") | color(theme.menuBarFg), inputCena->Render() | color(theme.contentFg)}),
                    hbox({text("Ilosc:  ") | color(theme.menuBarFg), inputIlosc->Render() | color(theme.contentFg)}),
                    text(""),
                    separator(),
                    text(""),
                    text(magazynKomunikat) | center | color(magazynKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                    text(""),
                    text("ENTER - zapisz | ESC - anuluj") | dim | center,
                }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 60) | center | bgcolor(theme.contentBg);
            } else if (magazynDialogTyp == 1) {
                magazynDialogBox = vbox({
                    text("=== EDYTUJ PRODUKT ===") | bold | center | color(theme.accentColor),
                    text(""),
                    separator(),
                    text(""),
                    hbox({text("ID:     ") | color(theme.menuBarFg), inputId->Render() | color(theme.contentFg)}),
                    hbox({text("Nazwa:  ") | color(theme.menuBarFg), inputNazwa->Render() | color(theme.contentFg)}),
                    hbox({text("Kod:    ") | color(theme.menuBarFg), inputKod->Render() | color(theme.contentFg)}),
                    hbox({text("Cena:   ") | color(theme.menuBarFg), inputCena->Render() | color(theme.contentFg)}),
                    hbox({text("Ilosc:  ") | color(theme.menuBarFg), inputIlosc->Render() | color(theme.contentFg)}),
                    text(""),
                    separator(),
                    text(""),
                    text(magazynKomunikat) | center | color(magazynKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                    text(""),
                    text("ENTER - zapisz | ESC - anuluj") | dim | center,
                }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 60) | center | bgcolor(theme.contentBg);
            } else {
                magazynDialogBox = vbox({
                    text("=== USUN PRODUKT ===") | bold | center | color(theme.accentColor),
                    text(""),
                    separator(),
                    text(""),
                    hbox({text("ID produktu do usuniecia: ") | color(theme.menuBarFg), inputId->Render() | color(theme.contentFg)}),
                    text(""),
                    separator(),
                    text(""),
                    text(magazynKomunikat) | center | color(magazynKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                    text(""),
                    text("ENTER - usun | ESC - anuluj") | dim | center,
                }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 60) | center | bgcolor(theme.contentBg);
            }
            
            main = dbox({
                main | dim,
                magazynDialogBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazPotwierdzUsun) {
            auto potwierdzBox = vbox({
                text("=== POTWIERDZENIE ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                text("Czy na pewno chcesz usunac ten produkt?") | center | color(theme.contentFg),
                text(""),
                text("ID: " + produktId + " | " + produktNazwa) | center | color(Color::Yellow),
                text(""),

                separator(),
                text(""),
                potwierdzMenu->Render() | center,
                text(""),
                separator(),
                text(""),
                text("ENTER - potwierdz | ESC - anuluj") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 50) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                potwierdzBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazListeKlientow) {
            vector<Element> klienciLista;
            const auto& klienciData = klienci.getKlienci();
            
            if (klienciData.empty()) {
                klienciLista.push_back(text("Brak klientow w bazie") | center | color(Color::Yellow));
            } else {
                Elements headerElements;
                headerElements.push_back(text("ID") | bold | size(WIDTH, EQUAL, 5));
                headerElements.push_back(separator());
                headerElements.push_back(text("Imie Nazwisko") | bold | size(WIDTH, EQUAL, 30));
                headerElements.push_back(separator());
                headerElements.push_back(text("PESEL/NIP") | bold | size(WIDTH, EQUAL, 15));
                headerElements.push_back(separator());
                headerElements.push_back(text("Adres") | bold | size(WIDTH, EQUAL, 45));
                klienciLista.push_back(hbox(headerElements) | bgcolor(theme.accentColor));
                klienciLista.push_back(separator());
                
                for (size_t i = 0; i < klienciData.size(); i++) {
                    const auto& k = klienciData[i];
                    string adres = k->getKodPocztowy() + " " + k->getMiejscowosc() + ", " + 
                                   k->getUlica() + " " + k->getNrDomu();
                    
                    Elements rowElements;
                    rowElements.push_back(text(to_string(k->getId())) | size(WIDTH, EQUAL, 5));
                    rowElements.push_back(separator());
                    rowElements.push_back(text(k->getImie() + " " + k->getNazwisko()) | size(WIDTH, EQUAL, 30));
                    rowElements.push_back(separator());
                    rowElements.push_back(text(k->getPeselNip()) | size(WIDTH, EQUAL, 15));
                    rowElements.push_back(separator());
                    rowElements.push_back(text(adres) | size(WIDTH, EQUAL, 45));
                    auto klientRow = hbox(rowElements);
                    
                    if ((int)i == wybranyKlientIdx) {
                        klientRow = klientRow | bgcolor(theme.accentColor) | color(Color::Black) | bold;
                    }
                    
                    klienciLista.push_back(klientRow);
                }
            }
            
            Elements listaElements;
            listaElements.push_back(text("=== LISTA KLIENTOW ===") | bold | center | color(theme.accentColor));
            listaElements.push_back(text(""));
            listaElements.push_back(separator());
            listaElements.push_back(text(""));
            listaElements.push_back(vbox(klienciLista) | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 20));
            listaElements.push_back(text(""));
            listaElements.push_back(separator());
            listaElements.push_back(text(""));
            listaElements.push_back(text("Lacznie klientow: " + to_string(klienciData.size())) | center | color(theme.contentFg));
            listaElements.push_back(text(""));
            listaElements.push_back(text("E - edytuj | DEL - usun | ESC - powrot") | dim | center);
            auto listaBox = vbox(listaElements) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 100) | center | bgcolor(theme.contentBg);
            
            Elements mainElements;
            mainElements.push_back(main | dim);
            mainElements.push_back(listaBox | clear_under | center | bgcolor(theme.contentBg));
            main = dbox(mainElements);
        }
        
        if (pokazKlienciDialog) {
            dialogActive = 5;
            Element klienciDialogBox;
            
            if (klienciDialogTyp == 0) {
                klienciDialogBox = vbox({
                    text("=== DODAJ KLIENTA ===") | bold | center | color(theme.accentColor),
                    text(""),
                    separator(),
                    text(""),
                    hbox({text("Imie:           ") | color(theme.menuBarFg), inputKlientImie->Render() | color(theme.contentFg)}),
                    hbox({text("Nazwisko:       ") | color(theme.menuBarFg), inputKlientNazwisko->Render() | color(theme.contentFg)}),
                    hbox({text("PESEL/NIP:      ") | color(theme.menuBarFg), inputKlientPeselNip->Render() | color(theme.contentFg)}),
                    hbox({text("Kod pocztowy:   ") | color(theme.menuBarFg), inputKlientKodPocztowy->Render() | color(theme.contentFg)}),
                    hbox({text("Miejscowosc:    ") | color(theme.menuBarFg), inputKlientMiejscowosc->Render() | color(theme.contentFg)}),
                    hbox({text("Ulica:          ") | color(theme.menuBarFg), inputKlientUlica->Render() | color(theme.contentFg)}),
                    hbox({text("Nr domu:        ") | color(theme.menuBarFg), inputKlientNrDomu->Render() | color(theme.contentFg)}),
                    text(""),
                    separator(),
                    text(""),
                    text(klienciKomunikat) | center | color(klienciKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                    text(""),
                    text("ENTER - zapisz | ESC - anuluj") | dim | center,
                }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 70) | center | bgcolor(theme.contentBg);
            } else if (klienciDialogTyp == 1) {
                klienciDialogBox = vbox({
                    text("=== EDYTUJ KLIENTA ===") | bold | center | color(theme.accentColor),
                    text(""),
                    separator(),
                    text(""),
                    hbox({text("ID:             ") | color(theme.menuBarFg), inputKlientId->Render() | color(theme.contentFg)}),
                    hbox({text("Imie:           ") | color(theme.menuBarFg), inputKlientImie->Render() | color(theme.contentFg)}),
                    hbox({text("Nazwisko:       ") | color(theme.menuBarFg), inputKlientNazwisko->Render() | color(theme.contentFg)}),
                    hbox({text("PESEL/NIP:      ") | color(theme.menuBarFg), inputKlientPeselNip->Render() | color(theme.contentFg)}),
                    hbox({text("Kod pocztowy:   ") | color(theme.menuBarFg), inputKlientKodPocztowy->Render() | color(theme.contentFg)}),
                    hbox({text("Miejscowosc:    ") | color(theme.menuBarFg), inputKlientMiejscowosc->Render() | color(theme.contentFg)}),
                    hbox({text("Ulica:          ") | color(theme.menuBarFg), inputKlientUlica->Render() | color(theme.contentFg)}),
                    hbox({text("Nr domu:        ") | color(theme.menuBarFg), inputKlientNrDomu->Render() | color(theme.contentFg)}),
                    text(""),
                    separator(),
                    text(""),
                    text(klienciKomunikat) | center | color(klienciKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                    text(""),
                    text("ENTER - zapisz | ESC - anuluj") | dim | center,
                }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 70) | center | bgcolor(theme.contentBg);
            } else {
                klienciDialogBox = vbox({
                    text("=== USUN KLIENTA ===") | bold | center | color(theme.accentColor),
                    text(""),
                    separator(),
                    text(""),
                    hbox({text("ID klienta do usuniecia: ") | color(theme.menuBarFg), inputKlientId->Render() | color(theme.contentFg)}),
                    text(""),
                    separator(),
                    text(""),
                    text(klienciKomunikat) | center | color(klienciKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                    text(""),
                    text("ENTER - usun | ESC - anuluj") | dim | center,
                }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 70) | center | bgcolor(theme.contentBg);
            }
            
            main = dbox({
                main | dim,
                klienciDialogBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazPotwierdzUsunKlienta) {
            auto potwierdzKlientBox = vbox({
                text("=== POTWIERDZENIE ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                text("Czy na pewno chcesz usunac tego klienta?") | center | color(theme.contentFg),
                text(""),
                text(klientId + " | " + klientImie + " " + klientNazwisko) | center | color(Color::Yellow),
                text(""),
                separator(),
                text(""),
                potwierdzKlientMenu->Render() | center,
                text(""),
                separator(),
                text(""),
                text("ENTER - potwierdz | ESC - anuluj") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 50) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                potwierdzKlientBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazListeKlientowFaktura && wybranyModul == 3) {
            vector<Element> klienciFakturaLista;
            const auto& klienciData = klienci.getKlienci();
            
            if (klienciData.empty()) {
                klienciFakturaLista.push_back(text("Brak klientow w bazie") | center | color(Color::Yellow));
            } else {
                Elements headerElements;
                headerElements.push_back(text("ID") | bold | size(WIDTH, EQUAL, 5));
                headerElements.push_back(separator());
                headerElements.push_back(text("Imie Nazwisko") | bold | size(WIDTH, EQUAL, 30));
                headerElements.push_back(separator());
                headerElements.push_back(text("PESEL/NIP") | bold | size(WIDTH, EQUAL, 15));
                headerElements.push_back(separator());
                headerElements.push_back(text("Adres") | bold | size(WIDTH, EQUAL, 45));
                klienciFakturaLista.push_back(hbox(headerElements) | bgcolor(theme.accentColor));
                klienciFakturaLista.push_back(separator());
                
                for (size_t i = 0; i < klienciData.size(); i++) {
                    const auto& k = klienciData[i];
                    string adres = k->getKodPocztowy() + " " + k->getMiejscowosc() + ", " + 
                                   k->getUlica() + " " + k->getNrDomu();
                    
                    Elements rowElements;
                    rowElements.push_back(text(to_string(k->getId())) | size(WIDTH, EQUAL, 5));
                    rowElements.push_back(separator());
                    rowElements.push_back(text(k->getImie() + " " + k->getNazwisko()) | size(WIDTH, EQUAL, 30));
                    rowElements.push_back(separator());
                    rowElements.push_back(text(k->getPeselNip()) | size(WIDTH, EQUAL, 15));
                    rowElements.push_back(separator());
                    rowElements.push_back(text(adres) | size(WIDTH, EQUAL, 45));
                    auto klientRow = hbox(rowElements);
                    
                    if ((int)i == wybranyKlientFakturaIdx) {
                        klientRow = klientRow | bgcolor(theme.accentColor) | color(Color::Black) | bold;
                    }
                    
                    klienciFakturaLista.push_back(klientRow);
                }
            }
            
            Elements listaElements;
            listaElements.push_back(text("=== FAKTURY - WYBIERZ KLIENTA ===") | bold | center | color(theme.accentColor));
            listaElements.push_back(text(""));
            listaElements.push_back(separator());
            listaElements.push_back(text(""));
            listaElements.push_back(vbox(klienciFakturaLista) | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 20));
            listaElements.push_back(text(""));
            listaElements.push_back(separator());
            listaElements.push_back(text(""));
            listaElements.push_back(text("Lacznie klientow: " + to_string(klienciData.size())) | center | color(theme.contentFg));
            listaElements.push_back(text(""));
            listaElements.push_back(text("ENTER - wystawic fakture | ESC - powrot") | dim | center);
            auto listaBox = vbox(listaElements) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 100) | center | bgcolor(theme.contentBg);
            
            Elements mainElements;
            mainElements.push_back(main | dim);
            mainElements.push_back(listaBox | clear_under | center | bgcolor(theme.contentBg));
            main = dbox(mainElements);
        }
        
        if (pokazPotwierdzWystawFakture) {
            auto potwierdzFakturaBox = vbox({
                text("=== WYSTAWIC FAKTURE? ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                text("Czy wystawic fakture dla klienta:") | center | color(theme.contentFg),
                text(""),
                text(wybranyKlientNazwaFaktura) | center | color(Color::Yellow),
                text(""),
                separator(),
                text(""),
                potwierdzFakturaMenu->Render() | center,
                text(""),
                separator(),
                text(""),
                text("ENTER - potwierdz | ESC - anuluj") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 50) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                potwierdzFakturaBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazDialogFaktury) {
            dialogActive = 7;
            
            if (!fakturaKodProduktu.empty()) {
                auto produkt = magazyn.pobierzProduktPoKodzie(fakturaKodProduktu);
                if (produkt != nullptr) {
                    fakturaNazwaProduktu = produkt->getNazwa();
                    stringstream cenaStream;
                    cenaStream << fixed << setprecision(2) << produkt->getCena();
                    fakturaCenaJednostkowa = cenaStream.str();
                    
                    if (!fakturaIlosc.empty()) {
                        try {
                            int ilosc = stoi(fakturaIlosc);
                            double wartoscCalkowita = produkt->getCena() * ilosc;
                            stringstream wartoscStream;
                            wartoscStream << fixed << setprecision(2) << wartoscCalkowita;
                            fakturaWartoscCalkowita = wartoscStream.str();
                        } catch (...) {
                            fakturaWartoscCalkowita = "0.00";
                        }
                    }
                } else {
                    fakturaNazwaProduktu = "Nie znaleziono";
                    fakturaCenaJednostkowa = "0.00";
                    fakturaWartoscCalkowita = "0.00";
                }
            } else {
                fakturaNazwaProduktu = "";
                fakturaCenaJednostkowa = "";
                fakturaWartoscCalkowita = "";
            }
            
            auto fakturaDialogBox = vbox({
                text("=== WYSTAWIENIE FAKTURY ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                text("Klient: " + wybranyKlientNazwaFaktura) | color(theme.menuBarFg),
                text(""),
                hbox({text("Kod produktu:    ") | color(theme.menuBarFg), inputFakturaKodProduktu->Render() | color(theme.contentFg)}),
                hbox({text("Nazwa produktu:  ") | color(theme.menuBarFg), text(fakturaNazwaProduktu) | color(Color::Cyan)}),
                hbox({text("Cena jednostkowa:") | color(theme.menuBarFg), text(fakturaCenaJednostkowa + " zl") | color(Color::Cyan)}),
                text(""),
                hbox({text("Ilosc:           ") | color(theme.menuBarFg), inputFakturaIlosc->Render() | color(theme.contentFg)}),
                text(""),
                separator(),
                text(""),
                hbox({text("Wartosc calkowita: ") | bold | color(theme.menuBarFg), text(fakturaWartoscCalkowita + " zl") | bold | color(Color::Green)}),
                text(""),
                separator(),
                text(""),
                text(fakturaKomunikat) | center | color(fakturaKomunikat.find("Blad") != string::npos ? Color::Red : Color::Green),
                text(""),
                text("ENTER - zapisz fakture | ESC - anuluj") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 70) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                fakturaDialogBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazHelp) {
            auto helpBox = vbox({
                text("=== POMOC - RetroSLM ===") | bold | center | color(theme.accentColor),
                text(""),
                separator(),
                text(""),
                text("NAWIGACJA:") | bold | color(theme.accentColor),
                text("  Strzalki <- ->  : Poruszanie w menu gornym") | color(theme.contentFg),
                text("  Strzalki gora/dol : Poruszanie w menu modulow") | color(theme.contentFg),
                text("  ENTER           : Potwierdzenie wyboru") | color(theme.contentFg),
                text("  ESC             : Powrot / Wyjscie") | color(theme.contentFg),
                text(""),
                text("KLAWISZE FUNKCYJNE:") | bold | color(theme.accentColor),
                text("  F1              : Pokaz/ukryj pomoc") | color(theme.contentFg),
                text("  F3              : Przelacz focus na menu gorne") | color(theme.contentFg),
                text("  F8              : Zmien motyw kolorystyczny") | color(theme.contentFg),
                text(""),
                text("MODULY:") | bold | color(theme.accentColor),
                text("  MAGAZYN         : Zarzadzanie produktami") | color(theme.contentFg),
                text("  KLIENCI         : Baza danych klientow") | color(theme.contentFg),
                text("  FAKTURY         : Tworzenie faktur") | color(theme.contentFg),
                text("  HISTORIA        : Log operacji systemu") | color(theme.contentFg),
                text(""),
                separator(),
                text(""),
                text("Nacisnij F1, ESC lub ENTER aby zamknac") | dim | center,
            }) | border | color(theme.accentColor) | size(WIDTH, EQUAL, 70) | size(HEIGHT, EQUAL, 26) | center | bgcolor(theme.contentBg);
            
            main = dbox({
                main | dim,
                helpBox | clear_under | center | bgcolor(theme.contentBg),
            });
        }
        
        if (pokazDialog) {            
            auto dialogBox = vbox({
                text("") | center,
                text("Witaj w RetroSLM") | bold | center,
                text("") | center,
                text("Copyright (C) RetroSLM Project, 2026.") | center,
                text("Wszelkie prawa zastrzeżone.") | center,
                text("") | center,
                text("< Nacisnij Enter aby zobaczyc program >") | center,
            }) | border | size(WIDTH, EQUAL, 60) | size(HEIGHT, EQUAL, 10) | center;
            
            main = dbox({
                main | dim,
                dialogBox | clear_under | center,
            });
        }
        
        return main;
    });
    
    finalRenderer |= CatchEvent([&](Event event) {
        if ((event.character() == "\x1b" && event.is_character()) && !pokazDialog && !pokazHelp && !pokazMotywy && !pokazWyjscie) {
        }
        
        if (event == Event::Character("\x11") && !pokazDialog && !pokazHelp && !pokazMotywy) {
            pokazWyjscie = !pokazWyjscie;
            if (pokazWyjscie) {
                selectedWyjscieOption = 0;
                dialogActive = 2;
            } else {
                dialogActive = 0;
            }
            return true;
        }
        
        if ((event.character() == "F1" || event == Event::F1) && !pokazMotywy) {
            pokazHelp = !pokazHelp;
            return true;
        }
        
        if ((event.character() == "F3" || event == Event::F3) && !pokazMotywy && !pokazHelp && !pokazDialog) {
            activeContainer = (activeContainer == 0) ? 1 : 0;
            return true;
        }
        
        if (event == Event::F8 && !pokazHelp && !pokazDialog) {
            pokazMotywy = !pokazMotywy;
            if (pokazMotywy) {
                selectedTheme = currentTheme;
                dialogActive = 1;
            } else {
                dialogActive = 0;
                activeContainer = 0;
            }
            return true;
        }
        
        if (pokazListeProdukow && !pokazPotwierdzUsun && !pokazMagazynDialog) {
            const auto& produkty = magazyn.getProdukty();
            
            if (event == Event::ArrowDown) {
                if (!produkty.empty() && wybranyProduktIdx < (int)produkty.size() - 1) {
                    wybranyProduktIdx++;
                }
                return true;
            }
            if (event == Event::ArrowUp) {
                if (wybranyProduktIdx > 0) {
                    wybranyProduktIdx--;
                }
                return true;
            }
            
            if ((event == Event::Character("e") || event == Event::Character("E")) && !produkty.empty()) {
                const auto& p = produkty[wybranyProduktIdx];
                produktId = to_string(p->getId());
                produktNazwa = p->getNazwa();
                produktKod = p->getKod();
                
                stringstream cenaStream;
                cenaStream << fixed << setprecision(2) << p->getCena();
                produktCena = cenaStream.str();
                
                produktIlosc = to_string(p->getDostepnaIlosc());
                magazynDialogTyp = 1;
                pokazMagazynDialog = true;
                magazynKomunikat = "";
                return true;
            }
            
            if (event == Event::Delete && !produkty.empty()) {
                const auto& p = produkty[wybranyProduktIdx];
                produktId = to_string(p->getId());
                produktNazwa = p->getNazwa();
                selectedPotwierdzOption = 0;
                pokazPotwierdzUsun = true;
                return true;
            }
            
            if (event == Event::Escape) {
                pokazListeProdukow = false;
                wybranyProduktIdx = 0;
                return true;
            }
            
            return true;
        }
        
        if (pokazPotwierdzUsun) {
            dialogActive = 4;
            
            if (event == Event::Return) {
                if (selectedPotwierdzOption == 1) {
                    try {
                        int id = stoi(produktId);
                        if (magazyn.usunProdukt(id)) {
                            const auto& produkty = magazyn.getProdukty();
                            if (wybranyProduktIdx >= (int)produkty.size() && wybranyProduktIdx > 0) {
                                wybranyProduktIdx--;
                            }
                        }
                    } catch (...) {
                    }
                }
                pokazPotwierdzUsun = false;
                produktId = "";
                selectedPotwierdzOption = 0;
                dialogActive = 0;
                return true;
            }
            
            if (event == Event::Escape) {
                pokazPotwierdzUsun = false;
                produktId = "";
                selectedPotwierdzOption = 0;
                dialogActive = 0;
                return true;
            }
            
            if (event == Event::ArrowUp || event == Event::ArrowDown || event == Event::ArrowLeft || event == Event::ArrowRight) {
                return false;
            }
            
            return true;
        }
        
        if (pokazMagazynDialog) {
            if (event == Event::Return) {
                magazynKomunikat = "";
                
                if (magazynDialogTyp == 0) {
                    if (produktNazwa.empty() || produktKod.empty() || produktCena.empty() || produktIlosc.empty()) {
                        magazynKomunikat = "Blad: Wszystkie pola musza byc wypelnione!";
                        return true;
                    }
                    
                    try {
                        double cena = stod(produktCena);
                        int ilosc = stoi(produktIlosc);
                        
                        if (magazyn.dodajProdukt(produktNazwa, produktKod, cena, ilosc)) {
                            magazynKomunikat = "Produkt zostal dodany pomyslnie!";
                            zapiszHistorie("DODANO PRODUKT", "Nazwa: " + produktNazwa + ", Kod: " + produktKod);
                            produktNazwa = "";
                            produktKod = "";
                            produktCena = "";
                            produktIlosc = "";
                        } else {
                            magazynKomunikat = "Blad: Taki kod produktu juz istnieje!";
                        }
                    } catch (...) {
                        magazynKomunikat = "Blad: Nieprawidlowy format ceny lub ilosci!";
                    }
                } else if (magazynDialogTyp == 1) {
                    if (produktId.empty() || produktNazwa.empty() || produktKod.empty() || produktCena.empty() || produktIlosc.empty()) {
                        magazynKomunikat = "Blad: Wszystkie pola musza byc wypelnione!";
                        return true;
                    }
                    
                    try {
                        int id = stoi(produktId);
                        double cena = stod(produktCena);
                        int ilosc = stoi(produktIlosc);
                        
                        if (magazyn.edytujProdukt(id, produktNazwa, produktKod, cena, ilosc)) {
                            magazynKomunikat = "Produkt edytowany pomyslnie!";
                            zapiszHistorie("EDYTOWANO PRODUKT", "ID: " + produktId + ", Nazwa: " + produktNazwa + ", Kod: " + produktKod);
                            produktId = "";
                            produktNazwa = "";
                            produktKod = "";
                            produktCena = "";
                            produktIlosc = "";
                        } else {
                            magazynKomunikat = "Blad: Produkt o podanym ID nie istnieje!";
                        }
                    } catch (...) {
                        magazynKomunikat = "Blad: Nieprawidlowy format danych!";
                    }
                } else {
                    if (produktId.empty()) {
                        magazynKomunikat = "Blad: Podaj ID produktu!";
                        return true;
                    }
                    
                    try {
                        int id = stoi(produktId);
                        
                        if (magazyn.usunProdukt(id)) {
                            magazynKomunikat = "Produkt usuniety pomyslnie!";
                            zapiszHistorie("USUNIETO PRODUKT", "ID: " + produktId);
                            produktId = "";
                        } else {
                            magazynKomunikat = "Blad: Produkt o podanym ID nie istnieje!";
                        }
                    } catch (...) {
                        magazynKomunikat = "Blad: Nieprawidlowy format ID!";
                    }
                }
                return true;
            }
            
            if (event == Event::Escape) {
                pokazMagazynDialog = false;
                magazynKomunikat = "";
                produktNazwa = "";
                produktKod = "";
                produktCena = "";
                produktIlosc = "";
                produktId = "";
                return true;
            }
            
            return false;
        }
        
        if (pokazListeKlientow && !pokazPotwierdzUsunKlienta && !pokazKlienciDialog) {
            const auto& klienciData = klienci.getKlienci();
            
            if (event == Event::ArrowDown) {
                if (!klienciData.empty() && wybranyKlientIdx < (int)klienciData.size() - 1) {
                    wybranyKlientIdx++;
                }
                return true;
            }
            if (event == Event::ArrowUp) {
                if (wybranyKlientIdx > 0) {
                    wybranyKlientIdx--;
                }
                return true;
            }
            
            if ((event == Event::Character("e") || event == Event::Character("E")) && !klienciData.empty()) {
                const auto& k = klienciData[wybranyKlientIdx];
                klientId = to_string(k->getId());
                klientImie = k->getImie();
                klientNazwisko = k->getNazwisko();
                klientPeselNip = k->getPeselNip();
                klientKodPocztowy = k->getKodPocztowy();
                klientMiejscowosc = k->getMiejscowosc();
                klientUlica = k->getUlica();
                klientNrDomu = k->getNrDomu();
                klienciDialogTyp = 1;
                pokazKlienciDialog = true;
                klienciKomunikat = "";
                return true;
            }
            
            if (event == Event::Delete && !klienciData.empty()) {
                const auto& k = klienciData[wybranyKlientIdx];
                klientId = to_string(k->getId());
                klientImie = k->getImie();
                klientNazwisko = k->getNazwisko();
                selectedPotwierdzKlientOption = 0;
                pokazPotwierdzUsunKlienta = true;
                return true;
            }
            
            if (event == Event::Escape) {
                pokazListeKlientow = false;
                wybranyKlientIdx = 0;
                return true;
            }
            
            return true;
        }
        
        if (pokazPotwierdzUsunKlienta) {
            dialogActive = 6;
            
            if (event == Event::Return) {
                if (selectedPotwierdzKlientOption == 1) {
                    try {
                        int id = stoi(klientId);
                        if (klienci.usunKlienta(id)) {
                            const auto& klienciList = klienci.getKlienci();
                            if (wybranyKlientIdx >= (int)klienciList.size() && wybranyKlientIdx > 0) {
                                wybranyKlientIdx--;
                            }
                        }
                    } catch (...) {
                    }
                }
                pokazPotwierdzUsunKlienta = false;
                klientId = "";
                selectedPotwierdzKlientOption = 0;
                dialogActive = 0;
                return true;
            }
            
            if (event == Event::Escape) {
                pokazPotwierdzUsunKlienta = false;
                klientId = "";
                selectedPotwierdzKlientOption = 0;
                dialogActive = 0;
                return true;
            }
            
            if (event == Event::ArrowUp || event == Event::ArrowDown || event == Event::ArrowLeft || event == Event::ArrowRight) {
                return false;
            }
            
            return true;
        }
        
        if (pokazKlienciDialog) {
            if (event == Event::Return) {
                klienciKomunikat = "";
                
                if (klienciDialogTyp == 0) {
                    if (klientImie.empty() || klientNazwisko.empty() || klientPeselNip.empty()) {
                        klienciKomunikat = "Blad: Imie, nazwisko i PESEL/NIP sa wymagane!";
                        return true;
                    }
                    
                    if (klienci.dodajKlienta(klientImie, klientNazwisko, klientPeselNip,
                                               klientKodPocztowy, klientMiejscowosc, 
                                               klientUlica, klientNrDomu)) {
                        klienciKomunikat = "Klient dodany pomyslnie!";
                        zapiszHistorie("DODANO KLIENTA", klientImie + " " + klientNazwisko + ", PESEL/NIP: " + klientPeselNip);
                        klientImie = "";
                        klientNazwisko = "";
                        klientPeselNip = "";
                        klientKodPocztowy = "";
                        klientMiejscowosc = "";
                        klientUlica = "";
                        klientNrDomu = "";
                    } else {
                        klienciKomunikat = "Blad: Klient z takim PESEL/NIP juz istnieje!";
                    }
                } else if (klienciDialogTyp == 1) {
                    if (klientId.empty() || klientImie.empty() || klientNazwisko.empty() || klientPeselNip.empty()) {
                        klienciKomunikat = "Blad: Wszystkie wymagane pola musza byc wypelnione!";
                        return true;
                    }
                    
                    try {
                        int id = stoi(klientId);
                        
                        if (klienci.edytujKlienta(id, klientImie, klientNazwisko, klientPeselNip,
                                                    klientKodPocztowy, klientMiejscowosc,
                                                    klientUlica, klientNrDomu)) {
                            klienciKomunikat = "Klient edytowany pomyslnie!";
                            zapiszHistorie("EDYTOWANO KLIENTA", "ID: " + klientId + ", " + klientImie + " " + klientNazwisko);
                            klientId = "";
                            klientImie = "";
                            klientNazwisko = "";
                            klientPeselNip = "";
                            klientKodPocztowy = "";
                            klientMiejscowosc = "";
                            klientUlica = "";
                            klientNrDomu = "";
                        } else {
                            klienciKomunikat = "Blad: Klient o podanym ID nie istnieje!";
                        }
                    } catch (...) {
                        klienciKomunikat = "Blad: Nieprawidlowy format danych!";
                    }
                } else {
                    if (klientId.empty()) {
                        klienciKomunikat = "Blad: Podaj ID klienta!";
                        return true;
                    }
                    
                    try {
                        int id = stoi(klientId);
                        
                        if (klienci.usunKlienta(id)) {
                            klienciKomunikat = "Klient usuniety pomyslnie!";
                            zapiszHistorie("USUNIETO KLIENTA", "ID: " + klientId);
                            klientId = "";
                        } else {
                            klienciKomunikat = "Blad: Klient o podanym ID nie istnieje!";
                        }
                    } catch (...) {
                        klienciKomunikat = "Blad: Nieprawidlowy format ID!";
                    }
                }
                return true;
            }
            
            if (event == Event::Escape) {
                pokazKlienciDialog = false;
                klienciKomunikat = "";
                klientImie = "";
                klientNazwisko = "";
                klientPeselNip = "";
                klientKodPocztowy = "";
                klientMiejscowosc = "";
                klientUlica = "";
                klientNrDomu = "";
                klientId = "";
                return true;
            }
            
            return false;
        }
        
        if (pokazListeKlientowFaktura && wybranyModul == 3 && !pokazPotwierdzWystawFakture && !pokazDialogFaktury) {
            const auto& klienciData = klienci.getKlienci();
            
            if (event == Event::ArrowDown) {
                if (!klienciData.empty() && wybranyKlientFakturaIdx < (int)klienciData.size() - 1) {
                    wybranyKlientFakturaIdx++;
                }
                return true;
            }
            if (event == Event::ArrowUp) {
                if (wybranyKlientFakturaIdx > 0) {
                    wybranyKlientFakturaIdx--;
                }
                return true;
            }
            
            if (event == Event::Return && !klienciData.empty()) {
                const auto& k = klienciData[wybranyKlientFakturaIdx];
                wybranyKlientIdFaktura = k->getId();
                wybranyKlientNazwaFaktura = k->getImie() + " " + k->getNazwisko() + " (ID: " + to_string(k->getId()) + ")";
                selectedPotwierdzFakturaOption = 0;
                pokazPotwierdzWystawFakture = true;
                return true;
            }
            
            if (event == Event::Escape) {
                pokazListeKlientowFaktura = false;
                wybranyKlientFakturaIdx = 0;
                return true;
            }
            
            return true;
        }
        
        if (pokazListeHistorii && wybranyModul == 4) {
            if (event == Event::ArrowDown) {
                if (!wpisyHistorii.empty() && wybranyWpisHistoriiIdx < (int)wpisyHistorii.size() - 1) {
                    wybranyWpisHistoriiIdx++;
                }
                return true;
            }
            if (event == Event::ArrowUp) {
                if (wybranyWpisHistoriiIdx > 0) {
                    wybranyWpisHistoriiIdx--;
                }
                return true;
            }
            
            if (event == Event::Escape) {
                pokazListeHistorii = false;
                wybranyWpisHistoriiIdx = 0;
                return true;
            }
            
            return true;
        }
        
        if (pokazPotwierdzWystawFakture) {
            dialogActive = 8;
            
            if (event == Event::Return) {
                if (selectedPotwierdzFakturaOption == 1) {
                    pokazDialogFaktury = true;
                    fakturaKodProduktu = "";
                    fakturaIlosc = "";
                    fakturaNazwaProduktu = "";
                    fakturaCenaJednostkowa = "";
                    fakturaWartoscCalkowita = "";
                    fakturaKomunikat = "";
                }
                pokazPotwierdzWystawFakture = false;
                selectedPotwierdzFakturaOption = 0;
                dialogActive = pokazDialogFaktury ? 7 : 0;
                return true;
            }
            
            if (event == Event::Escape) {
                pokazPotwierdzWystawFakture = false;
                selectedPotwierdzFakturaOption = 0;
                dialogActive = 0;
                return true;
            }
            
            if (event == Event::ArrowUp || event == Event::ArrowDown || event == Event::ArrowLeft || event == Event::ArrowRight) {
                return false;
            }
            
            return true;
        }
        
        if (pokazDialogFaktury) {
            if (event == Event::Return) {
                fakturaKomunikat = "";
                
                if (fakturaKodProduktu.empty() || fakturaIlosc.empty()) {
                    fakturaKomunikat = "Blad: Kod produktu i ilosc sa wymagane!";
                    return true;
                }
                
                auto produkt = magazyn.pobierzProduktPoKodzie(fakturaKodProduktu);
                if (produkt == nullptr) {
                    fakturaKomunikat = "Blad: Produkt o podanym kodzie nie istnieje!";
                    return true;
                }
                
                int ilosc;
                try {
                    ilosc = stoi(fakturaIlosc);
                    if (ilosc <= 0) {
                        fakturaKomunikat = "Blad: Ilosc musi byc wieksza od 0!";
                        return true;
                    }
                } catch (...) {
                    fakturaKomunikat = "Blad: Nieprawidlowy format ilosci!";
                    return true;
                }
                
                if (ilosc > produkt->getDostepnaIlosc()) {
                    fakturaKomunikat = "Blad: Niewystarczajaca ilosc w magazynie (dostepne: " + to_string(produkt->getDostepnaIlosc()) + ")!";
                    return true;
                }
                
                try {
                    system("mkdir -p BazaDanych/faktury");
                    
                    time_t now = time(0);
                    tm* ltm = localtime(&now);
                    char filename[100];
                    sprintf(filename, "BazaDanych/faktury/faktura_%04d%02d%02d_%02d%02d%02d_K%d.txt",
                            1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday,
                            ltm->tm_hour, ltm->tm_min, ltm->tm_sec,
                            wybranyKlientIdFaktura);
                    
                    ofstream plik(filename);
                    if (plik.is_open()) {
                        plik << "===============================================\n";
                        plik << "           FAKTURA VAT\n";
                        plik << "===============================================\n\n";
                        
                        char dataBuffer[20];
                        sprintf(dataBuffer, "%02d.%02d.%04d", ltm->tm_mday, 1 + ltm->tm_mon, 1900 + ltm->tm_year);
                        plik << "Data wystawienia: " << dataBuffer << "\n\n";
                        
                        auto klient = klienci.pobierzKlienta(wybranyKlientIdFaktura);
                        if (klient != nullptr) {
                            plik << "NABYWCA:\n";
                            plik << "  " << klient->getImie() << " " << klient->getNazwisko() << "\n";
                            plik << "  PESEL/NIP: " << klient->getPeselNip() << "\n";
                            plik << "  Adres: " << klient->getKodPocztowy() << " " << klient->getMiejscowosc() << "\n";
                            plik << "         " << klient->getUlica() << " " << klient->getNrDomu() << "\n\n";
                        }
                        
                        plik << "-----------------------------------------------\n";
                        plik << "POZYCJE:\n";
                        plik << "-----------------------------------------------\n";
                        plik << fixed << setprecision(2);
                        plik << "Produkt: " << produkt->getNazwa() << "\n";
                        plik << "Kod: " << produkt->getKod() << "\n";
                        plik << "Cena jednostkowa: " << produkt->getCena() << " zl\n";
                        plik << "Ilosc: " << ilosc << " szt\n";
                        
                        double wartoscNetto = produkt->getCena() * ilosc;
                        double vat = wartoscNetto * 0.23;
                        double wartoscBrutto = wartoscNetto + vat;
                        
                        plik << "\n-----------------------------------------------\n";
                        plik << "Wartosc netto:  " << wartoscNetto << " zl\n";
                        plik << "VAT (23%):      " << vat << " zl\n";
                        plik << "Wartosc brutto: " << wartoscBrutto << " zl\n";
                        plik << "===============================================\n";
                        
                        plik.close();
                        
                        magazyn.edytujProdukt(produkt->getId(), produkt->getNazwa(), produkt->getKod(),
                                              produkt->getCena(), produkt->getDostepnaIlosc() - ilosc);
                        
                        fakturaKomunikat = "Faktura zapisana: " + string(filename);
                        
                        string klientInfo = (klient != nullptr) ? (klient->getImie() + " " + klient->getNazwisko()) : "Nieznany";
                        zapiszHistorie("WYSTAWIONO FAKTURE", "Klient: " + klientInfo + ", Produkt: " + produkt->getNazwa() + " x" + to_string(ilosc));
                        
                    } else {
                        fakturaKomunikat = "Blad: Nie mozna zapisac faktury!";
                    }
                } catch (...) {
                    fakturaKomunikat = "Blad: Wystapil blad podczas zapisu faktury!";
                }
                
                return true;
            }
            
            if (event == Event::Escape) {
                pokazDialogFaktury = false;
                fakturaKodProduktu = "";
                fakturaIlosc = "";
                fakturaNazwaProduktu = "";
                fakturaCenaJednostkowa = "";
                fakturaWartoscCalkowita = "";
                fakturaKomunikat = "";
                dialogActive = 0;
                return true;
            }
            
            return false;
        }
        
        if (pokazWyjscie) {
            if (event == Event::Return) {
                if (selectedWyjscieOption == 1) {
                    screen.ExitLoopClosure()();
                    return true;
                }
                pokazWyjscie = false;
                dialogActive = 0;
                selectedWyjscieOption = 0;
                return true;
            }
            if (event == Event::Escape) {
                pokazWyjscie = false;
                dialogActive = 0;
                selectedWyjscieOption = 0;
                return true;
            }
            if (event == Event::ArrowUp || event == Event::ArrowDown || event == Event::ArrowLeft || event == Event::ArrowRight) {
                return false;
            }
            return true;
        }
        
        if (pokazMotywy) {
            if (event == Event::Return) {
                currentTheme = selectedTheme;
                zapiszKonfiguracje(currentTheme);
                pokazMotywy = false;
                dialogActive = 0;
                activeContainer = 0;
                return true;
            }
            if (event == Event::Escape) {
                selectedTheme = currentTheme;
                pokazMotywy = false;
                dialogActive = 0;
                activeContainer = 0;
                return true;
            }
            if (event == Event::ArrowUp || event == Event::ArrowDown) {
                return false;
            }
            return true;
        }
        
        if (pokazHelp) {
            if (event == Event::Return || event == Event::Escape) {
                pokazHelp = false;
                return true;
            }
            return true;
        }
        
        if (pokazDialog) {
            if (event == Event::Return || event == Event::Escape) {
                pokazDialog = false;
                return true;
            }
            return true;
        }
        return false;
    });
    
    screen.Loop(finalRenderer);
    
    return 0;
}
