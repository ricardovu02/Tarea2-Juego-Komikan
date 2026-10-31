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

    for (int i =0; i< cantidad_perros; i++)
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

bool ubiFichas(Partida &p)
{
    p.posPuma = -1;

    for (int i = 0; i< cantidad_perros; i++)
    {
        p.posPerros[i] = -1;
    }
    int perros = 0;
    int total = TAM * TAM + TAM_TRONO;

    for (int i = 0; i < total; i++)
    {
        char ficha;

        if (i < TAM * TAM)
        {
            ficha = p.tablero[i / TAM][i % TAM];
        }
        else
        {
            ficha = p.trono[i - TAM * TAM];
        }

        if (ficha == 'P')
        {
            if (p.posPuma != -1)
            {
                return false;
            }
            p.posPuma = i;
        }
        else if (ficha == 'D')
        {
            if(perros >= cantidad_perros)
            {
                return false;
            }

            p.posPerros[perros] = i;
            perros++;
        }
    }
    return p.posPuma != -1 && perros == cantidad_perros;
}