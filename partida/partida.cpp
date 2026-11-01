#include "partida.h"
#include <iostream>

using namespace std;

bool iniciarPartida(Partida &p)
{
    inicializarTablero(p.tablero);
    inicializarTrono(p.trono);

    p.capturas = 0;
    p.terminada = false;
    p.posPuma = -1;
    
    for (int i = 0; i < cant_Perros; i++)
    {
        p.posPerros[i] = -1;
    }

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

void* threadPuma(void* dato)
{
    Partida *p = (Partida *)dato;
    pthread_mutex_lock(&p->mutex);

    cout << "Thread del Puma iniciado" << endl;
    cout << "Capturas actuales: " << p->capturas << endl;

    pthread_mutex_unlock(&p->mutex);
    return nullptr;
}