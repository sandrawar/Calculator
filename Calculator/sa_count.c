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
int wczytajLiczbe(FILE *fptr, FILE *fptr2, char* liczba, int podstawa);
int zamienNaLiczby(char* znaki, char* liczba, int podstawa);
int zamianaPodstawy(char* liczba, char* wynik, int podstawa, int nowapodstawa);
int policz(char* liczba1, char* liczba2, char* wynik, int podstawa, enum Operacja operacja);
int wykonajDodawanie(char* liczba1, char* liczba2, char* wynik, int podstawa);
int wykonajMnozenie(char* liczba1, char* liczba2, char* wynik, int podstawa);
int wykonajPotegowanie(char* liczba1, char* liczba2, char* wynik, int podstawa);
int wykonajDzielenie(char* liczba1, char* liczba2, char* wynik, int podstawa);
int wykonajModulo(char* liczba1, char* liczba2, char* wynik, int podstawa);
int zapiszWynik(char* wynik, int podstawa, FILE *fptr2);
char zamienNaZnak(char liczba);

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
    char *liczba1 = malloc(sizeof(char) * max_dl_liczby);
    char *liczba2 = malloc(sizeof(char) * max_dl_liczby);
    char *wynik = malloc(sizeof(char) * max_dl_liczby);

    if(operacja == zmiana_podstawy){
        char* npStart = definicja_operacji;
        while (isdigit(*npStart)){
            npStart++;
        }
        int nowaPodstawa = wczytajPodstawe(npStart);
        printf("nowa podstawa: %d\n", nowaPodstawa);
        int liczba;
        while((wczytajLiczbe(fptr, fptr2, liczba1, podstawa)) >= 0){
            printf("liczba1: %s\n", liczba1);
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
                char* tmp = wynik;
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

int wczytajLiczbe(FILE *fptr, FILE *fptr2, char* liczba, int podstawa){
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
        zamienNaLiczby(bufor, liczba, podstawa);
        return 0;
    }
    return -4;
 }

int zamienNaLiczby(char* znaki, char* liczba, int podstawa){
    int len = strlen(znaki);
    if (len <= 0){
        return -1;
    }

    int i = max_dl_liczby;
    //char* znak = znaki + sizeof(char) * len;
    while (--len >= 0)
    {
        int cyfra;
        if(isdigit(znaki[len])){
            cyfra = znaki[len] - '0';
            if(cyfra >= podstawa){
                return -2;
            }
        }
        else{
            cyfra = znaki[len] - 'A' + 10;
            if(cyfra >= podstawa){
                return -3;
            }
        }
        if (cyfra < 0){
            return -4;
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

int zamianaPodstawy(char* liczba, char* wynik, int podstawa, int nowaPodstawa){
    for(int i=0; i < max_dl_liczby; i++){
        wynik[i] = liczba[i];
    }
    return 0;
}

int policz(char* liczba1, char* liczba2, char* wynik, int podstawa, enum Operacja operacja){
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
            return wykonajDzielenie(liczba1, liczba2, wynik, podstawa);

        default:
            return -1;
    }
}

int wykonajDodawanie(char* liczba1, char* liczba2, char* wynik, int podstawa){
    for(int i=0; i < max_dl_liczby; i++){
        wynik[i] = liczba2[i];
    }
}

int wykonajMnozenie(char* liczba1, char* liczba2, char* wynik, int podstawa){
    for(int i=0; i < max_dl_liczby; i++){
        wynik[i] = liczba2[i];
    }
}

int wykonajPotegowanie(char* liczba1, char* liczba2, char* wynik, int podstawa){
    for(int i=0; i < max_dl_liczby; i++){
        wynik[i] = liczba2[i];
    }
}

int wykonajDzielenie(char* liczba1, char* liczba2, char* wynik, int podstawa){
    for(int i=0; i < max_dl_liczby; i++){
        wynik[i] = liczba2[i];
    }
}

int wykonajModulo(char* liczba1, char* liczba2, char* wynik, int podstawa){
    for(int i=0; i < max_dl_liczby; i++){
        wynik[i] = liczba2[i];
    }
}

int zapiszWynik(char* wynik, int podstawa, FILE *fptr2){
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

char zamienNaZnak(char liczba){
    return (liczba < 10) ? liczba + '0' : liczba - 10 + 'A';
}

