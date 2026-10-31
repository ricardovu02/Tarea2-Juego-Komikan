#include "partida.h"

bool iniciarPartida(Partida &p)
{
    inicializarTablero(p.tablero);
    inicializarTrono(p.trono);

    p.capturas = 0;
    p.terminada = false;

    if(pthread_mutex_init(&p.mutex, nullptr) != 0)
    {
        return false;
    }
    return true;

}

void cerrarPartida(Partida &p)
{
    pthread_mutex_destroy(&p.mutex);
}