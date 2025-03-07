#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define max_dl_liczby 40
#define max_dl_linii (max_dl_liczby + 1)
#define max_do 7

enum Operacja {dodawanie = 1, mnozenie = 2, potegowanie = 3, dzielenie = 4, modulo = 5, zmiana_podstawy = 6, nieznana = 0};

int wykonajDzialanie(FILE *daneFptr, FILE *wynikiFptr);
int zamianaPodstawy(int* liczba, int* wynik, int podstawa, int nowapodstawa);
int policz(int* liczba1, int* liczba2, int* wynik, int podstawa, enum Operacja operacja);
int wykonajDodawanie(int* liczba1, int* liczba2, int* wynik, int podstawa);
int wykonajMnozenie(int* liczba1, int* liczba2, int* wynik, int podstawa);
int wykonajPotegowanie(int* liczba, int* potega, int* wynik, int podstawa);
int wykonajDzielenie(int* dzielna, int* dzielnik, int* wynik, int* reszta, int podstawa);
int wykonajModulo(int* dzielna, int* dzielnik, int* wynik, int podstawa);
int wykonajOdejmowanie(int* odjemna, int* odjemnik, int* wynik, int podstawa);
enum Operacja wczytajOperacje(char *operacja);
int wczytajPodstawe(char *operacja);
int wczytajLiczbe(FILE *daneFptr, FILE *wynikiFptr, int* liczba, int podstawa);
char zamienNaZnak(int liczba);
int zamienNaCyfry(char* znaki, int* liczba, int podstawa);
int zamienNaCyfre(char znak, int podstawa);
int policzWartosc(int* liczba, int podstawa);
void zapiszWynik(int* wynik, int podstawa, FILE *wynikiFptr);
void zapiszBledneDane(FILE *wynikiFptr);
int porownajLiczby(int* liczba1, int* liczba2);
int dlugoscLiczby(int* liczba);
void kopiujCyfry(int* zrodlo, int* przeznaczenie);
void zerujCyfry(int* liczba);
void ustawCyfry(int* liczba, int cyfra);
int stworzLiczbe(int liczba, int podstawa, int* wynik);

int main(int argc, char *argv[]){
    char *fin = "sa_01_in.txt";
    char *fout = "sa_01_out.txt";
    if(argc > 1){
        fin = argv[1];
    }
    if(argc > 2){
        fout = argv[2];
    }
    FILE *daneFptr = fopen(fin, "r");
    FILE *wynikiFptr = fopen(fout, "w");
    while (!feof(daneFptr)){
        wykonajDzialanie(daneFptr, wynikiFptr);
    }
    fclose(daneFptr);
    fclose(wynikiFptr); 
    return 0;
}

int wykonajDzialanie(FILE *daneFptr, FILE *wynikiFptr){
    char definicja_operacji[max_do];
    if (fgets(definicja_operacji, max_do, daneFptr) == NULL){
        return -1;
    };
    if (definicja_operacji[0] == '\n'){
        return -1;
    }
    if (strchr(definicja_operacji, '\n') == NULL){
        return -1;
    }
    enum Operacja operacja = wczytajOperacje(definicja_operacji);
    if (operacja == nieznana){
        return -2;
    }
    int podstawa = wczytajPodstawe(definicja_operacji);
    if (podstawa <= 0){
        return -3;
    }

    fputs(definicja_operacji, wynikiFptr);
    fputc('\n', wynikiFptr);

    int* liczba1 = malloc(sizeof(int) * max_dl_liczby);
    int* liczba2 = malloc(sizeof(int) * max_dl_liczby);
    int* wynik = malloc(sizeof(int) * max_dl_liczby);
    zerujCyfry(liczba1);
    zerujCyfry(liczba2);
    zerujCyfry(wynik);

    if(operacja == zmiana_podstawy){
        char* npStart = definicja_operacji;
        while (isdigit(*npStart)){
            npStart++;
        }
        int nowaPodstawa = wczytajPodstawe(npStart);

        while((wczytajLiczbe(daneFptr, wynikiFptr, liczba1, podstawa)) >= 0){
            zerujCyfry(wynik);
            if (zamianaPodstawy(liczba1, wynik, podstawa, nowaPodstawa) >= 0){
                zapiszWynik(wynik, nowaPodstawa, wynikiFptr);
            }
        }
    }
    else{
        if (wczytajLiczbe(daneFptr, wynikiFptr, wynik, podstawa) == 0){
            int liczba;
            int wynikWczytania = wczytajLiczbe(daneFptr, wynikiFptr, liczba2, podstawa);
            while(wynikWczytania == 0){
                int* tmp = wynik;
                wynik = liczba1;
                liczba1 = tmp;
                policz(liczba1, liczba2, wynik, podstawa, operacja);
                 wynikWczytania = wczytajLiczbe(daneFptr, wynikiFptr, liczba2, podstawa);
            }
            if (wynikWczytania > -5){
                zapiszWynik(wynik, podstawa, wynikiFptr);
            }
        }
    }
    fputs("\n\n", wynikiFptr);
    free(liczba1);
    free(liczba2);
    free(wynik);

    return 0;
}

int zamianaPodstawy(int* liczba, int* wynik, int podstawa, int nowaPodstawa){
    if (podstawa == nowaPodstawa){
        kopiujCyfry(liczba, wynik);
        return 0;
    }

    if (nowaPodstawa < podstawa){
        int logP = log2(podstawa) / log2(nowaPodstawa);
        if (logP * dlugoscLiczby(liczba) > max_dl_liczby){
            ustawCyfry(wynik, -1);
            return 1;
        }
    }

    int kopiaLiczba[max_dl_liczby];
    kopiujCyfry(liczba, kopiaLiczba);

    int nowaPodstawaTab[max_dl_liczby] = {0};
    stworzLiczbe(nowaPodstawa, podstawa, nowaPodstawaTab);

    int zero[max_dl_liczby] = {0};
    int reszta[max_dl_liczby] = {0};

    int i = max_dl_liczby;
    while (porownajLiczby(kopiaLiczba, zero) > 0){
        wykonajDzielenie(kopiaLiczba, nowaPodstawaTab, kopiaLiczba, reszta, podstawa);
        int resztaWartosc = policzWartosc(reszta, podstawa);
        wynik[--i] = resztaWartosc;
    }
    
    return 0;

    int podstawaTab[max_dl_liczby] = {0};
    podstawaTab[max_dl_liczby - 1] = nowaPodstawa % 10;
    podstawaTab[max_dl_liczby - 2] = nowaPodstawa / 10;
    int podstawa2 = nowaPodstawa;
    int potega = 1;
    int podstawaWsystemie[max_dl_liczby];
    int cyfra[max_dl_liczby] = {0};
    while((podstawa2 / 10) > 0){
        cyfra[max_dl_liczby - 1] = (podstawa2 % 10);
        podstawa2 /= 10;
        wykonajMnozenie(podstawaTab, cyfra, cyfra, nowaPodstawa);
        wykonajDodawanie(podstawaWsystemie, cyfra, podstawaWsystemie, nowaPodstawa);
        potega *= nowaPodstawa;
    }
    int jeden[max_dl_liczby] = {0};
    jeden[max_dl_liczby-1] = 1;
    for(int i=0; i<max_dl_liczby; i++){
        int wartosc = liczba[i] * podstawa;
        while((wartosc / nowaPodstawa) > 0){
            cyfra[max_dl_liczby - 1] = (wartosc % nowaPodstawa);
            wartosc /= nowaPodstawa;
            wykonajMnozenie(podstawaWsystemie, cyfra, cyfra, nowaPodstawa);
            wykonajDodawanie(wynik, cyfra, wynik, nowaPodstawa);
            wykonajMnozenie(podstawaWsystemie, podstawaTab, podstawaWsystemie, nowaPodstawa);
        }
    }

    kopiujCyfry(liczba, wynik);
    return 0;
}

int policz(int* liczba1, int* liczba2, int* wynik, int podstawa, enum Operacja operacja){
    switch (operacja)
    {
        case dodawanie:
            return wykonajDodawanie(liczba1, liczba2, wynik, podstawa);

        case mnozenie:
            return wykonajMnozenie(liczba1, liczba2, wynik, podstawa);

        case modulo:
            return wykonajModulo(liczba1, liczba2, wynik, podstawa);

        case potegowanie:
            return wykonajPotegowanie(liczba1, liczba2, wynik, podstawa);

        case dzielenie:
            return wykonajDzielenie(liczba1, liczba2, wynik, NULL, podstawa);

        default:
            return -1;
    }
}

int wykonajDodawanie(int* liczba1, int* liczba2, int* wynik, int podstawa){
    int kopiaLiczba1[max_dl_liczby];
    kopiujCyfry(liczba1, kopiaLiczba1);
    int kopiaLiczba2[max_dl_liczby];
    kopiujCyfry(liczba2, kopiaLiczba2);
    zerujCyfry(wynik);

    int dlugoscLiczby1 = dlugoscLiczby(kopiaLiczba1);
    int dlugoscLiczby2 = dlugoscLiczby(kopiaLiczba2);
    if (dlugoscLiczby1 == max_dl_liczby && dlugoscLiczby2 == max_dl_liczby){
        ustawCyfry(wynik, -1);
        return 1;
    }

    for(int i=max_dl_liczby - 1; i >=0; i--){
        wynik[i] = kopiaLiczba1[i] + kopiaLiczba2[i];
        if(wynik[i] >= podstawa){
            kopiaLiczba1[i-1]++;
            wynik[i] = wynik[i] - podstawa;
        }
    }
    return 0;
}

int wykonajMnozenie(int* liczba1, int* liczba2, int* wynik, int podstawa){
    int kopiaLiczba1[max_dl_liczby];
    kopiujCyfry(liczba1, kopiaLiczba1);
    int kopiaLiczba2[max_dl_liczby];
    kopiujCyfry(liczba2, kopiaLiczba2);
    zerujCyfry(wynik);

    int dlugoscLiczby1 = dlugoscLiczby(kopiaLiczba1);
    int dlugoscLiczby2 = dlugoscLiczby(kopiaLiczba2);
    if (dlugoscLiczby1 + dlugoscLiczby2 > max_dl_liczby){
        ustawCyfry(wynik, -1);
        return 1;
    }

    for(int i=max_dl_liczby - 1; i >= max_dl_liczby - dlugoscLiczby1; i--){
        for(int j = max_dl_liczby - 1; j >= max_dl_liczby - dlugoscLiczby2; j--){
            wynik[j + i - max_dl_liczby + 1] += kopiaLiczba1[i] * kopiaLiczba2[j];
        }
    }
    for(int i=max_dl_liczby - 1; i >= 0; i--){
        if(wynik[i] >= podstawa){
            if (i>0){
                wynik[i -1] += wynik[i] / podstawa;
            }
            wynik[i] %= podstawa; 
        }
    }
    return 0;
}

int wykonajPotegowanie(int* liczba, int* potega, int* wynik, int podstawa){
    zerujCyfry(wynik);
    wynik[max_dl_liczby-1] = 1;

    int dlugoscPotegi = dlugoscLiczby(potega);
    if (dlugoscPotegi == 0){
        return 0;
    }
    if(dlugoscPotegi == 1 && potega[max_dl_liczby-1] == 1){
        kopiujCyfry(liczba, wynik);
        return 0;
    }

    int dlugosc_Liczby = dlugoscLiczby(liczba);
    int dlugoscLiczbyTab[max_dl_liczby] = {0};
    stworzLiczbe(dlugosc_Liczby, podstawa, dlugoscLiczbyTab);

    int maxDlugoscLiczbyTab[max_dl_liczby] = {0}; 
    stworzLiczbe(max_dl_liczby, podstawa, maxDlugoscLiczbyTab);

    int liczbaRazyPotega[max_dl_liczby] = {0};
    wykonajMnozenie(dlugoscLiczbyTab, potega, liczbaRazyPotega, podstawa);
    if (porownajLiczby(liczbaRazyPotega, maxDlugoscLiczbyTab) > 0){
        ustawCyfry(wynik, -1);
        return 1;
    }

    int kopiaLiczba[max_dl_liczby];
    kopiujCyfry(liczba, kopiaLiczba);
    int kopiaPotega[max_dl_liczby];
    kopiujCyfry(potega, kopiaPotega);

    int zero[max_dl_liczby] = {0};
    int jeden[max_dl_liczby] = {0};
    jeden[max_dl_liczby-1] = 1;
    int dwa[max_dl_liczby] = {0};
    dwa[max_dl_liczby-1] = (podstawa == 2) ? 0 : 2;
    dwa[max_dl_liczby-2] = (podstawa == 2) ? 1 : 0;
    int reszta[max_dl_liczby] = {0}; 

    while (porownajLiczby(kopiaPotega, jeden) > 0){
        wykonajDzielenie(kopiaPotega, dwa, kopiaPotega, reszta, podstawa);
        if (porownajLiczby(reszta, zero) > 0){
            wykonajMnozenie(wynik, kopiaLiczba, wynik, podstawa);
        }
        if (porownajLiczby(kopiaPotega, zero) > 0){
            wykonajMnozenie(kopiaLiczba, kopiaLiczba, kopiaLiczba, podstawa);
        }
    }
    wykonajMnozenie(kopiaLiczba, wynik, wynik, podstawa);

    return 0;
}

int wykonajDzielenie(int* dzielna, int* dzielnik, int* wynik, int* reszta, int podstawa){
    int kopiaDzielna[max_dl_liczby];
    kopiujCyfry(dzielna, kopiaDzielna);
    int kopiaDzielnik[max_dl_liczby];
    kopiujCyfry(dzielnik, kopiaDzielnik);
    zerujCyfry(wynik);

    int porownanie = porownajLiczby(kopiaDzielna, kopiaDzielnik);
    if (porownanie < 0){
        if (reszta != NULL){
            kopiujCyfry(kopiaDzielna, reszta);
        }
        return 0;
    }
    if (porownanie == 0){
        wynik[max_dl_liczby-1] = 1;
        if (reszta != NULL){
            int zero[max_dl_liczby] = {0};
            kopiujCyfry(zero, reszta);
        }
        return 0;
    }

    int dlugoscDzielna = dlugoscLiczby(kopiaDzielna);
    int dlugoscDzielnik = dlugoscLiczby(kopiaDzielnik);

    if (dlugoscDzielnik == 1 && kopiaDzielnik[max_dl_liczby-1] == 1){
        kopiujCyfry(kopiaDzielna, wynik);
        return 0;
    }

    int nowyDzielnik[max_dl_liczby];
    kopiujCyfry(kopiaDzielnik, nowyDzielnik);
    int jeden[max_dl_liczby] = {0};
    jeden[max_dl_liczby - 1] = 1;
    int dzielnikPoprzedni[max_dl_liczby];

    int wynikPoprzedni[max_dl_liczby] = {0};
    wynik[max_dl_liczby-1] = 1;
    int dwa[max_dl_liczby] = {0};
    stworzLiczbe(2, podstawa, dwa);

    while(porownajLiczby(kopiaDzielna, nowyDzielnik) >= 0){
        kopiujCyfry(nowyDzielnik, dzielnikPoprzedni);
        wykonajMnozenie(nowyDzielnik, dwa, nowyDzielnik, podstawa);
        kopiujCyfry(wynik, wynikPoprzedni);
        wykonajMnozenie(wynik, dwa, wynik, podstawa);
    }

    kopiujCyfry(wynikPoprzedni, wynik);
    kopiujCyfry(dzielnikPoprzedni, nowyDzielnik);

    int nowaDzielna[max_dl_liczby];
    wykonajOdejmowanie(kopiaDzielna, nowyDzielnik, nowaDzielna, podstawa);
    wykonajDzielenie(nowaDzielna, kopiaDzielnik, wynikPoprzedni, reszta, podstawa);

    wykonajDodawanie(wynik, wynikPoprzedni, wynik, podstawa);

    return 0;
}

int wykonajModulo(int* dzielna, int* dzielnik, int* wynik, int podstawa){
    int reszta[max_dl_liczby] = {0};
    int blad = wykonajDzielenie(dzielna, dzielnik, wynik, reszta, podstawa);
    if (blad != 0){
        return blad;
    }
    kopiujCyfry(reszta, wynik);
    return 0;
}

int wykonajOdejmowanie(int* odjemna, int* odjemnik, int* wynik, int podstawa){
    int kopiaOdjemna[max_dl_liczby];
    kopiujCyfry(odjemna, kopiaOdjemna);
    int kopiaOdjemnik[max_dl_liczby];
    kopiujCyfry(odjemnik, kopiaOdjemnik);

    zerujCyfry(wynik);
    
    int porownanie = porownajLiczby(kopiaOdjemna, kopiaOdjemnik);
    if (porownanie <= 0){
        return 0;
    }

    for(int i=max_dl_liczby-1; i>=0; i--){
        if (kopiaOdjemna[i] < 0){
            kopiaOdjemna[i] = podstawa - 1;
            kopiaOdjemna[i-1] -= 1; 
        }
        wynik[i] = kopiaOdjemna[i] - kopiaOdjemnik[i];
        if (wynik[i]<0){
            wynik[i] += podstawa;
            kopiaOdjemna[i-1] -= 1;
        }
    }
}

enum Operacja wczytajOperacje(char *operacja){
    if(isdigit(*operacja)){
        return zmiana_podstawy;
    }
    if(*operacja == '+'){
        return dodawanie;
    }
    if(*operacja == '*'){
        return mnozenie;
    }
    if(*operacja == '%'){
        return modulo;
    }
    if(*operacja == '^'){
        return potegowanie;
    }
    if(*operacja == '/'){
        return dzielenie;
    }
    return nieznana;
}

int wczytajPodstawe(char *operacja){
    int podstawa = 0;
    while(*operacja != '\0' && *operacja != '\n' && !isdigit(*operacja)){
        operacja++;
    }
    if (!isdigit(*operacja)){
        return -1;
    }
    while(*operacja != '\0' && *operacja != '\n'&& isdigit(*operacja)){
        podstawa *= 10;
        podstawa += *operacja++ - '0';
    }
    if(podstawa < 2 || podstawa > 16){
        return -2;
    }
    return podstawa;
}

int wczytajLiczbe(FILE *daneFptr, FILE *wynikiFptr, int* liczba, int podstawa){
    char bufor[max_dl_linii];
    if (fgets(bufor, max_dl_linii, daneFptr) == NULL){
        return -1;
    }
    if (bufor[0] != '\n'){
        return -2;
    }
    if (fgets(bufor, max_dl_linii, daneFptr) == NULL){
        return -3;
    }
    if (bufor[0] != '\n'){
        fputs(bufor, wynikiFptr);
        fputs("\n", wynikiFptr);

        if (strchr(bufor, '\n') == NULL){
            zapiszBledneDane(wynikiFptr);
            return -5;
        }

        size_t len = strlen(bufor);
        if (len > 0 && bufor[--len] == '\n') {
            bufor[len] = '\0';
        }
        if (zamienNaCyfry(bufor, liczba, podstawa) == 0){
            return 0;
        }
        zapiszBledneDane(wynikiFptr);
        return -6;
    }
    return -4;
 }

char zamienNaZnak(int liczba){
    return (liczba < 10) ? liczba + '0' : liczba - 10 + 'A';
}

int zamienNaCyfry(char* znaki, int* liczba, int podstawa){
    int len = strlen(znaki);
    if (len <= 0){
        return -1;
    }

    int i = max_dl_liczby;
    while (--len >= 0)
    {
        int cyfra = zamienNaCyfre(znaki[len], podstawa);
        if (cyfra < 0){
            return cyfra;
        }
        liczba[--i] = cyfra;
    }
    while (--i >= 0){
        liczba[i] = 0;
    }
    return 0;
}

int zamienNaCyfre(char znak, int podstawa){
    int cyfra;
    if(isdigit(znak)){
        cyfra = znak - '0';
        if(cyfra >= podstawa){
            return -2;
        }
    }
    else{
        cyfra = znak - 'A' + 10;
        if(cyfra >= podstawa){
            return -3;
        }
    }

    return (cyfra >= 0) ? cyfra : -4;
}

void zapiszWynik(int* wynik, int podstawa, FILE *wynikiFptr){
    if (wynik[0] < 0){
        fputs("poza zakresem\n\n", wynikiFptr);
        return;
    }
    int i = 0;
    while (i < max_dl_liczby && wynik[i] == 0){
        i++;
    }
    if(i == max_dl_liczby){
        fputc('0', wynikiFptr);
    }
    while (i < max_dl_liczby){
        char znak = zamienNaZnak(wynik[i++]); 
        fputc(znak, wynikiFptr);
    }
    fputs("\n\n", wynikiFptr);
    return;
}

void zapiszBledneDane(FILE *wynikiFptr){
     fputs("błędne dane\n\n", wynikiFptr);
}

int porownajLiczby(int* liczba1, int* liczba2){
    int dlugosc1 = dlugoscLiczby(liczba1);
    int dlugosc2 = dlugoscLiczby(liczba2);

    if (dlugosc1 == dlugosc2){
        int i = (max_dl_liczby) - dlugosc1;
        while (liczba1[i] == liczba2[i] && i < max_dl_liczby){
            i++;
        }
        if (i == max_dl_liczby){
            return 0;
        }

        return (liczba1[i] - liczba2[i]);
    }

    return (dlugosc1 > dlugosc2) ? 1 : -1;    

}

int dlugoscLiczby(int* liczba){
    int dlugosc = max_dl_liczby;
    for(int i=0; liczba[i] == 0 && i < max_dl_liczby; i++){
        dlugosc--;
    }
    return dlugosc;
}

void kopiujCyfry(int* zrodlo, int* przeznaczenie){
    for(int i=0; i < max_dl_liczby; i++){
        przeznaczenie[i] = zrodlo[i];
    }
}

void zerujCyfry(int* liczba){
    ustawCyfry(liczba, 0);
}

void ustawCyfry(int* liczba, int cyfra){
    for (int i = 0; i < max_dl_liczby; i++){
        liczba[i] = cyfra;
    }
}

int stworzLiczbe(int liczba, int podstawa, int* wynik){
    int i = max_dl_liczby;
    while (liczba > 0){
        int reszta = liczba % podstawa;
        liczba /= podstawa;
        wynik[--i] = reszta;
    }
}

int policzWartosc(int* liczba, int podstawa){
    int wynik = 0;
    for (int i=0; i<max_dl_liczby; i++){
        wynik = wynik * podstawa + liczba[i];
    }

    return wynik;
}
