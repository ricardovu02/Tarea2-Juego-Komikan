#ifndef Partida_H
#define Partida_H
#include <pthread.h>
#include "../juego/juego.h"

struct Partida
{
    char tablero[TAM][TAM];
    char trono[TAM_TRONO];
    int capturas;
    bool terminada;

    pthread_mutex_t mutex;

};

bool iniciarPartida(Partida &p);
void cerrarPartida(Partida &p);

#endif
