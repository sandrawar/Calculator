#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

const int max_dl_liczby = 40;
const int max_dl_linii = max_dl_liczby + 1;
const int max_do = 7;

int wykonajDzialanie(FILE *fptr, FILE *fptr2);
int wczytajLiczbe(FILE *fptr, FILE *fptr2, int system);
int odczytaj_liczbe(char* liczba, int podstawa);
int wczytajSystem(char *operacja);
char wczytajZnak(char *operacja);
int policz(int liczba1, int liczba2, char znak);
int zapiszWynik(int wartosc, int podstawa, FILE *fptr2);

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
    char operacja[max_do];
    if (fgets(operacja, max_do, fptr) == NULL){
        return -1;
    };
    if (operacja[0] == '\n'){
        return -1;
    }
    printf("\nop: %s", operacja);
    fputs(operacja, fptr2);
    fputc('\n', fptr2);
    char znak = wczytajZnak(operacja);
    int podstawa = 0;
    int wynik;
    if(znak != '>'){
        podstawa = wczytajSystem(operacja);
        wynik = wczytajLiczbe(fptr, fptr2, podstawa);
        int liczba;
        while((liczba = wczytajLiczbe(fptr, fptr2, podstawa)) >= 0){
            //liczba = wczytajLiczbe(fptr, podstawa);
            //fputs(liczba, fptr2);
            wynik = policz(wynik, liczba, znak);
            //printf("%d\n", wynik);
        }
        printf("wynik: %d\n", wynik);
        zapiszWynik(wynik, podstawa, fptr2);
    }
    else{
        int nowaPodstawa = 0;
        int i = 0;
        for(int j = 0; operacja[j] != ' '; j++){
            podstawa *= 10;
            podstawa += operacja[j] - '0';
            i++;
        }
        i++;
        //printf("%d\n", i);
        for(i; operacja[i] != '\0' && operacja[i] != '\n'; i++){
            nowaPodstawa *= 10;
            nowaPodstawa += operacja[i] - '0';
        }
        printf("nowa podstawa: %d\n", nowaPodstawa);
        int liczba;
        while((liczba = wczytajLiczbe(fptr, fptr2, podstawa)) >= 0){
            zapiszWynik(liczba, nowaPodstawa, fptr2);
        }
        //int system = wczytajLiczbe(operacja, 10);
    }
    //printf("%d\n", podstawa);
    return 0;
}

int wczytajLiczbe(FILE *fptr, FILE *fptr2, int system){
    //printf("sys: %d\n", system);
    char liczba[max_dl_linii];
    if (fgets(liczba, max_dl_linii, fptr) == NULL){
        return -1;
    }
    if (liczba[0] != '\n'){
        return -2;
    }
    if (fgets(liczba, max_dl_linii, fptr) == NULL){
        return -3;
    }
    if (liczba[0] != '\n'){
        printf("liczba odczytana: %s", liczba);
        fputs(liczba, fptr2);
        fputc('\n', fptr2);
        int wartosc = odczytaj_liczbe(liczba, system);
        printf("wartosc odczytana: %d\n", wartosc);
        if (wartosc < 0){
            return -5;
        }
        return wartosc;
    }
    return -4;
 }

int odczytaj_liczbe(char* liczba, int podstawa){
    int n = 0;
    //printf("podstawa: %d\n", podstawa);
    while (*liczba != '\n' && *liczba != '\0'){
        int cyfra;
        if(isdigit(*liczba)){
            if(*liczba - '0' >= podstawa){
                return -2;
            }
            cyfra = *liczba++ - '0';
        }
        else{
            if(*liczba - 'A' + 10 >= podstawa){
                return -3;
            }
            cyfra = *liczba++ - 'A' + 10;
        }
        if (cyfra < 0){
            return -1;
        }
        n = n * podstawa + cyfra;
    }

    return n;
}

int wczytajSystem(char *operacja){
    int system = 0;
    while(*operacja != ' '){
        //printf("%c", operacja);
        *operacja++;
    }
    *operacja++;
    while(*operacja != '\0' && *operacja != '\n'){
        //printf("%d\n", system);
        system *= 10;
        system += *operacja++ - '0';
    }
    if(system < 2 || system > 16){
        return -1;
    }
    printf("system: %d\n", system);
    return system;
}

char wczytajZnak(char *operacja){
    //printf("%c", *operacja);
    if(isdigit(*operacja)){
        //printf("%c\n", '>');
        return '>';
    }
    printf("znak: %c\n", *operacja);
    return *operacja;
}

int policz(int liczba1, int liczba2, char znak){
    //printf("liczba1: %d\n", liczba1);
    if(znak == '+'){
        //printf("wynik: %d\n", liczba1 + liczba2);
        return liczba1 + liczba2;
    }
    if(znak == '*'){
        return liczba1 * liczba2;
    }
    if(znak == '%'){
        return liczba1 % liczba2;
    }
    if(znak == '^'){
        return liczba1 ^ liczba2;
    }
    if(znak == '/'){
        return liczba1 / liczba2;
    }
    return -1;
}

int zapiszWynik(int wartosc, int podstawa, FILE *fptr2){
    int wynik[max_dl_linii];
    for(int i = 0; i < max_dl_linii; i++){
        wynik[i] = -1;
    }
    int liczba = wartosc;
    int i = max_dl_linii - 1;
    while(liczba > 0){
        int cyfra = liczba % podstawa;
        if(cyfra < 10){
            wynik[i] = cyfra + '0';
        }
        else{
           wynik[i] = cyfra + 'A' - 11;
        }
        //printf("%d ", cyfra);
        liczba /= podstawa;
        i --;
    }
    printf("\nwynik: ");
    for(int i = 0; i < max_dl_linii; i++){
        if(wynik[i] != -1){
            printf("%c", wynik[i]);
            fputc(wynik[i], fptr2);
        }
    }
    fputs("\n\n\n", fptr2);
    printf("\n");
    return 0;
    //printf("\nwynik: %s\n", wynik);
}

