#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int max_dl = 41;
const int max_do = 7;

int odczytaj_liczbe(char* liczba, int podstawa){
    int n = 0;

    while (*liczba != '\n' && *liczba != '\0'){
        int cyfra = *liczba++ - '0';
        if (cyfra < 0){
            return -1;
        }
        n = n * podstawa + cyfra;
    }

    return n;
}

int wczytajLiczbe(FILE *fptr){
    char liczba[max_dl];
    if (fgets(liczba, max_dl, fptr) == NULL){
        return -1;
    }
    if (liczba[0] != '\n'){
        return -2;
    }
    if (fgets(liczba, max_dl, fptr) == NULL){
        return -3;
    }
    if (liczba[0] != '\n'){
        printf("liczba odczytana: %s", liczba);
        int wartosc = odczytaj_liczbe(liczba, 10);
        printf("wartosc odczytana: %d\n", wartosc);
        if (wartosc < 0){
            return -5;
        }
        return 0;
    }
    return -4;
}

int wykonajDzialanie(FILE *fptr){
    char operacja[max_do];
    if (fgets(operacja, max_do, fptr) == NULL){
        return -1;
    };
    if (operacja[0] == '\n'){
        return -1;
    }
    printf("\nop: %s", operacja);
    int liczba;
    while((liczba = wczytajLiczbe(fptr)) >= 0) ;
    
    return 0;
}

int main(){
    FILE *fptr;
    fptr = fopen("sa_01_in.txt", "r");
    while (!feof(fptr)){
        wykonajDzialanie(fptr);
    }
    fclose(fptr); 
    return 0;
}