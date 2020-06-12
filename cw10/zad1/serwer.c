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

}


int main(int argc, char** argv){

    if (argc < 3) wyjscie_z_bledem("Podano zbyt mało argumentów.");
    numer_portu = (int) strtol(argv[1], (char**)NULL, 10);
    sciezka_gniazda_UNIX = argv[2];

    inicjalizuj_serwer();



    wylacz_serwer();
    return 0;
}