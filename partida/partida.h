#ifndef Partida_H
#define Partida_H
#include <pthread.h>
#include "../juego/juego.h"

const int cant_Perros = 12;

struct Partida
{
    char tablero[TAM][TAM];
    char trono[TAM_TRONO];
    int capturas;
    bool terminada;

    int posPuma;
    int posPerros[cant_Perros];

    pthread_mutex_t mutex;

};

bool iniciarPartida(Partida &p);
bool ubicacionFichas(Partida &p);
void cerrarPartida(Partida &p);
void* threadPuma(void* dato);

#endif
