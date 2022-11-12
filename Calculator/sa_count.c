#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define max_dl_liczby 40
#define max_dl_linii (max_dl_liczby + 1)
#define max_do 7

enum Operacja {dodawanie = 1, mnozenie = 2, potegowanie = 3, dzielenie = 4, modulo = 5, zmiana_podstawy = 6, nieznana = 0};

int wykonajDzialanie(FILE *fptr, FILE *fptr2);
enum Operacja wczytajOperacje(char *operacja);
int wczytajPodstawe(char *operacja);
int wczytajLiczbe(FILE *fptr, FILE *fptr2, int* liczba, int podstawa);
int zamienNaCyfry(char* znaki, int* liczba, int podstawa);
int zamienNaCyfre(char znak, int podstawa);
int zamianaPodstawy(int* liczba, int* wynik, int podstawa, int nowapodstawa);
int policz(int* liczba1, int* liczba2, int* wynik, int podstawa, enum Operacja operacja);
int wykonajDodawanie(int* liczba1, int* liczba2, int* wynik, int podstawa);
int wykonajMnozenie(int* liczba1, int* liczba2, int* wynik, int podstawa);
int wykonajPotegowanie(int* liczba1, int* liczba2, int* wynik, int podstawa);
int wykonajDzielenie(int* liczba1, int* liczba2, int* wynik, int* reszta, int podstawa);
int wykonajModulo(int* liczba1, int* liczba2, int* wynik, int podstawa);
int wykonajOdejmowanie(int* liczba1, int* liczba2, int* wynik, int podstawa);
int zapiszWynik(int* wynik, int podstawa, FILE *fptr2);
int porownajLiczby(int* liczba1, int* liczba2);
int dlugoscLiczby(int* liczba);
char zamienNaZnak(int liczba);
void kopiujCyfry(int* zrodlo, int* przeznaczenie);
void zerujCyfry(int* liczba);

int main(int argc, char *argv[]){
    char *fin = "sa_01_in.txt";
    char *fout = "sa_01_out.txt";
    if(argc > 1){
        fin = argv[1];
    }
    if(argc > 2){
        fout = argv[2];
    }
    FILE *fptr;
    fptr = fopen(fin, "r");
    FILE *fptr2;
    fptr2 = fopen(fout, "w");
    while (!feof(fptr)){
        wykonajDzialanie(fptr, fptr2);
    }
    fclose(fptr);
    fclose(fptr2); 
    return 0;
}

int wykonajDzialanie(FILE *fptr, FILE *fptr2){
    char definicja_operacji[max_do];
    if (fgets(definicja_operacji, max_do, fptr) == NULL){
        return -1;
    };
    if (definicja_operacji[0] == '\n'){
        return -1;
    }
    printf("\nop: %s", definicja_operacji);
    fputs(definicja_operacji, fptr2);
    fputc('\n', fptr2);
    enum Operacja operacja = wczytajOperacje(definicja_operacji);
    printf("operacja %d\n", operacja);
    if (operacja == nieznana){
        return -2;
    }
    int podstawa = wczytajPodstawe(definicja_operacji);
    if (podstawa <= 0){
        return -3;
    }
    printf("podstawa %d\n", podstawa);
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
        printf("nowa podstawa: %d\n", nowaPodstawa);

        while((wczytajLiczbe(fptr, fptr2, liczba1, podstawa)) >= 0){
            printf("liczba1: %s\n", liczba1);
            zerujCyfry(wynik);
            if (zamianaPodstawy(liczba1, wynik, podstawa, nowaPodstawa) >= 0){
                printf("wynik: %s\n", wynik);
                zapiszWynik(wynik, nowaPodstawa, fptr2);
            }
        }
    }
    else{
        if (wczytajLiczbe(fptr, fptr2, wynik, podstawa) == 0){
            printf("liczba1 (wynik): %s\n", wynik);
            int liczba;
            while((wczytajLiczbe(fptr, fptr2, liczba2, podstawa)) == 0){
                int* tmp = wynik;
                wynik = liczba1;
                liczba1 = tmp;
                printf("liczba1: %s\n", liczba1);
                printf("liczba2: %s\n", liczba2);
                policz(liczba1, liczba2, wynik, podstawa, operacja);
                printf("wynik: %s\n", wynik);
            }
            printf("wynik: %s\n", wynik);
            zapiszWynik(wynik, podstawa, fptr2);
        }
    }
    free(liczba1);
    free(liczba2);
    free(wynik);

    return 0;
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

int wczytajLiczbe(FILE *fptr, FILE *fptr2, int* liczba, int podstawa){
    char bufor[max_dl_linii];
    if (fgets(bufor, max_dl_linii, fptr) == NULL){
        return -1;
    }
    if (bufor[0] != '\n'){
        return -2;
    }
    if (fgets(bufor, max_dl_linii, fptr) == NULL){
        return -3;
    }
    if (bufor[0] != '\n'){
        fputs(bufor, fptr2);
        fputs("\n", fptr2);
        size_t len = strlen(bufor);
        if (len > 0 && bufor[--len] == '\n') {
            bufor[len] = '\0';
        }
        printf("odczyt: %s\n", bufor);
        zamienNaCyfry(bufor, liczba, podstawa);
        return 0;
    }
    return -4;
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
    printf("liczby: ");
    for(int i = 0; i < max_dl_liczby; i++){
        printf("%d,", liczba[i]);
    }
    printf("\n");
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

int zamianaPodstawy(int* liczba, int* wynik, int podstawa, int nowaPodstawa){
    if (podstawa == nowaPodstawa){
        kopiujCyfry(liczba, wynik);
        return 0;
    }

    if (podstawa > nowaPodstawa){
        int liczbaKopia[max_dl_liczby];
        kopiujCyfry(liczba, liczbaKopia);

        for (int i=max_dl_liczby-1; i>=0; i--){
            wynik[i] += liczbaKopia[i] % nowaPodstawa;
            if (i > 0){
                if (wynik[i] >= nowaPodstawa){
                    wynik[i-1] += wynik[i] / nowaPodstawa;
                    wynik[i] = wynik[i] % nowaPodstawa;
                }
                wynik[i-1] += liczbaKopia[i] / nowaPodstawa;
            }
        }

        return 0;
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

    for(int i=max_dl_liczby - 1; i >=0; i--){
        wynik[i] = liczba2[i] + kopiaLiczba1[i];
        if(wynik[i] >= podstawa){
            kopiaLiczba1[i-1]++;
            wynik[i] = wynik[i] - podstawa;
        }
    }
    return 0;
}

int wykonajMnozenie(int* liczba1, int* liczba2, int* wynik, int podstawa){
    for(int i=max_dl_liczby - 1; i >=0; i--){
        for(int j = max_dl_liczby - 1; j >= 0; j--){
            wynik[j + i - max_dl_liczby + 1] += liczba1[j] * liczba2[i];
        }
    }
    for(int i=max_dl_liczby; i > 0; i--){
        if(wynik[i] >= podstawa){
            wynik[i -1] += wynik[i] / podstawa;
            wynik[i] %= podstawa; 
        }
    }
    return 0;
}

int wykonajPotegowanie(int* liczba1, int* liczba2, int* wynik, int podstawa){
    kopiujCyfry(liczba1, wynik);
    return 0;
}

int wykonajDzielenie(int* liczba1, int* liczba2, int* wynik, int* reszta, int podstawa){
    int porownanie = porownajLiczby(liczba1, liczba2);
    if (porownanie < 0){
        return 0;
    }
    if (porownanie == 0){
        wynik[max_dl_liczby-1] = 1;
        return 0;
    }

    int dlugosc1 = dlugoscLiczby(liczba1);
    int dlugosc2 = dlugoscLiczby(liczba2);

    if (dlugosc2 == 1 && liczba2[max_dl_liczby-1] == 1){
        kopiujCyfry(liczba1, wynik);
        return 0;
    }

    int dzielnik[max_dl_liczby];
    kopiujCyfry(liczba2, dzielnik);
    int jeden[max_dl_liczby] = {0};
    jeden[max_dl_liczby - 1] = 1;
    int dzielnikPoprzedni[max_dl_liczby];

    while(porownajLiczby(liczba1, dzielnik) > 0){
        wykonajDodawanie(wynik, jeden, wynik, podstawa);
        if (reszta != NULL){
            kopiujCyfry(dzielnik, dzielnikPoprzedni);
        }
        wykonajDodawanie(dzielnik, liczba2, dzielnik, podstawa);
    }

    if (reszta != NULL){
        wykonajOdejmowanie(liczba1, dzielnikPoprzedni, reszta, podstawa);
    }

    return 0;
}

int wykonajModulo(int* liczba1, int* liczba2, int* wynik, int podstawa){
    int reszta[max_dl_liczby] = {0};
    int blad = wykonajDzielenie(liczba1, liczba2, wynik, reszta, podstawa);
    if (blad != 0){
        return blad;
    }
    kopiujCyfry(reszta, wynik);
    return 0;
}

int wykonajOdejmowanie(int* liczba1, int* liczba2, int* wynik, int podstawa){
    int porownanie = porownajLiczby(liczba1, liczba2);
    if (porownanie <= 0){
        return 0;
    }

    int liczba1Kopia[max_dl_liczby];
    kopiujCyfry(liczba1, liczba1Kopia);

    for(int i=max_dl_liczby-1; i>=0; i--){
        if (liczba1Kopia[i] < 0){
            liczba1Kopia[i] = podstawa - 1;
            liczba1Kopia[i-1] -= 1; 
        }
        wynik[i] = liczba1Kopia[i] - liczba2[i];
        if (wynik[i]<0){
            wynik[i] += podstawa;
            liczba1Kopia[i-1] -= 1;
        }
    }
}


int zapiszWynik(int* wynik, int podstawa, FILE *fptr2){
    printf("zapiszWynik: ");
    int i = 0;
    while (i < max_dl_liczby && wynik[i] == 0){
        i++;
    }
    while (i < max_dl_liczby){
        char znak = zamienNaZnak(wynik[i++]); 
        printf("%c", znak);
        fputc(znak, fptr2);
    }
    printf("\n");
    fputs("\n\n\n", fptr2);
    return 0;
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
    for(int i=0; liczba[i] == 0; i++){
        dlugosc--;
    }
    return dlugosc;
}

char zamienNaZnak(int liczba){
    return (liczba < 10) ? liczba + '0' : liczba - 10 + 'A';
}

void kopiujCyfry(int* zrodlo, int* przeznaczenie){
    for(int i=0; i < max_dl_liczby; i++){
        przeznaczenie[i] = zrodlo[i];
    }
}

void zerujCyfry(int* liczba){
    for (int i = 0; i < max_dl_liczby; i++){
        liczba[i] = 0;
    }
}
