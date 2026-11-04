#ifndef Partida_H
#define Partida_H
#include <pthread.h>
#include "../juego/juego.h"

const int cantidad_perros = 12;
const int abandono_puma = 1;
const int abandono_perro = 2;

struct Partida
{
    char tablero[TAM][TAM];
    char trono[TAM_TRONO];
    int capturas;
    bool terminada;
    bool interrumpida;
    int abandono;
    int retardoMin;
    int retardoMax;
    unsigned int semilla;
    int mesa;
    bool arbitroListo;
    bool capturaPendiente;
    int perroCapturado;
    int origenCaptura;
    int destinoCaptura;
    int posPuma;
    int posPerros[cantidad_perros];

    pthread_mutex_t mutex;
    pthread_cond_t avisoArbitro;

};

bool iniciarPartida(Partida &p);
bool ubicacionFichas(Partida &p);
void cerrarPartida(Partida &p);
void detenerPartida(Partida &p);
void* threadPuma(void* dato);
bool threadsFichas(Partida &p);

#endif
