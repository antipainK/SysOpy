#include "pomocnicze.h"
#include "funkcje_tekstowe.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <endian.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <signal.h>
#include <pthread.h>
#include <poll.h>
#include <time.h>
#include <netdb.h>


int numer_portu;
char* sciezka_gniazda_UNIX;

struct sockaddr_un adres_gniazda_unix;
int deskryptor_gniazda_unix;
struct sockaddr_in adres_gniazda_inet;
int deskryptor_gniazda_inet;

pthread_t watek_obslugujacy_siec;
pthread_t watek_pingujacy;

void inicjalizuj_serwer(){
    adres_gniazda_unix.sun_family = AF_UNIX;
    strcpy(adres_gniazda_unix.sun_path, sciezka_gniazda_UNIX);

    deskryptor_gniazda_unix = socket(AF_UNIX, SOCK_STREAM, 0);
    if(deskryptor_gniazda_unix < 0) wyjscie_z_bledem("Nieudane utworzenie gniazda (Unix).");

    if( bind(deskryptor_gniazda_unix, (struct sockaddr*) &adres_gniazda_unix, sizeof(adres_gniazda_unix)) < 0 ) wyjscie_z_bledem("Nieudane związanie gniazda z jego adresem (Unix).");
    if( listen(deskryptor_gniazda_unix, MAX_KLIENTOW) < 0 ) wyjscie_z_bledem("Nieudane rozpoczecie akceptowania polaczen od klientow (Unix).");

    printf("Gniazdo UNIX nasluchuje na %s\n", sciezka_gniazda_UNIX);

    /*************************************************/

    struct in_addr temp = *(struct in_addr*) gethostbyname("localhost")->h_addr;
    adres_gniazda_inet.sin_family = AF_INET;
    adres_gniazda_inet.sin_port = htons(numer_portu);
    adres_gniazda_inet.sin_addr.s_addr = temp.s_addr;

    deskryptor_gniazda_inet = socket(AF_INET, SOCK_STREAM, 0);
    if(deskryptor_gniazda_inet < 0) wyjscie_z_bledem("Nieudane utworzenie gniazda (Inet).");

    if( bind(deskryptor_gniazda_inet, (struct sockaddr*) &adres_gniazda_inet, sizeof(adres_gniazda_inet)) < 0 ) wyjscie_z_bledem("Nieudane związanie gniazda z jego adresem (Inet).");
    if( listen(deskryptor_gniazda_inet, MAX_KLIENTOW) < 0 ) wyjscie_z_bledem("Nieudane rozpoczecie akceptowania polaczen od klientow (Inet).");

    printf("Gniazdo INET nasluchuje na %s:%d\n", inet_ntoa(temp), numer_portu);
}


void wylacz_serwer(){
    if( shutdown(deskryptor_gniazda_unix, SHUT_RDWR) < 0 ) wyjscie_z_bledem("Nie udalo sie wylaczyc gniazda (Unix).");
    if( close(deskryptor_gniazda_unix) < 0 ) wyjscie_z_bledem("Nie udalo sie zamknac gniazda (Unix).");

    if( shutdown(deskryptor_gniazda_inet, SHUT_RDWR) < 0 ) wyjscie_z_bledem("Nie udalo sie wylaczyc gniazda (Inet).");
    if( close(deskryptor_gniazda_inet) < 0 ) wyjscie_z_bledem("Nie udalo sie zamknac gniazda (Inet).");

}

void* obsluga_sieci(){
    return 0;
}

void* obsluga_pingowania(){
    return 0;
}


int main(int argc, char** argv){

    if (argc < 3) wyjscie_z_bledem("Podano zbyt mało argumentów.");
    numer_portu = (int) strtol(argv[1], (char**)NULL, 10);
    sciezka_gniazda_UNIX = argv[2];

    inicjalizuj_serwer();
    signal(SIGINT, wylacz_serwer);

    if( pthread_create(&watek_obslugujacy_siec, NULL, obsluga_sieci, NULL) != 0) wyjscie_z_bledem("Nie udalo sie utworzyc watku (do obslugi sieci).");
    if( pthread_create(&watek_pingujacy, NULL, obsluga_pingowania, NULL) != 0) wyjscie_z_bledem("Nie udalo sie utworzyc watku (do obslugi pingowania).");

    pthread_join(watek_obslugujacy_siec, NULL);
    pthread_join(watek_pingujacy, NULL);

    wylacz_serwer();
    return 0;
}