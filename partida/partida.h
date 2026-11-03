#ifndef Partida_H
#define Partida_H
#include <pthread.h>
#include "../juego/juego.h"

const int cantidad_perros = 12;
struct Partida
{
    char tablero[TAM][TAM];
    char trono[TAM_TRONO];
    int capturas;
    bool terminada;
    int retardoMin;
    int retardoMax;
    unsigned int semilla;
    int intentos;
    int posPuma;
    int posPerros[cantidad_perros];

    pthread_mutex_t mutex;

};

bool iniciarPartida(Partida &p);
bool ubicacionFichas(Partida &p);
void cerrarPartida(Partida &p);
void* threadPuma(void* dato);
bool threadsFichas(Partida &p);

#endif
