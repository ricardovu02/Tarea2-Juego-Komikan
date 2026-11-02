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

bool ubicacionFichas(Partida &p)
{
    int pumas = 0;
    int perros = 0;

    for(int pos = 0; pos < TAM * TAM + TAM_TRONO; pos++)
    {
        char ficha;

        if (pos < TAM * TAM)
        {
            ficha = p.tablero[pos / TAM][pos % TAM];
        }
        else
        {
            ficha = p.trono[pos - TAM * TAM];

        }
        if (ficha == 'P')
        {
            p.posPuma = pos;
            pumas++;
        }
        else if (ficha == 'D')
        {
            if (perros < cant_Perros)
            {
                p.posPerros[perros] = pos;
            }

            perros++;
        }
    }
    if (pumas != 1)
    {
        cout << "Cantidad de Pumas: " << pumas <<endl;
        cout << "Numero invalido. Se requiere exactamente 1 Puma." << endl;
    }
    if (perros != cant_Perros)
    {
        cout << "Cantidad de Perros: " << perros << endl;
        cout << "Numero invalido. Se requieren exactamente " << cant_Perros << " Perros." << endl;
    }
    return pumas == 1 && perros == cant_Perros;
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

struct DatoPerro
{
    Partida *partida;
    int id;
};

void* threadPerro(void* dato)
{
    DatoPerro *info = (DatoPerro *)dato;
    Partida *p = info->partida;

    pthread_mutex_lock(&p->mutex);

    cout << "Perro " << info->id + 1 << " en posicion: " << p->posPerros[info->id] << endl;

    pthread_mutex_unlock(&p->mutex);

    return nullptr;
}

bool threadsPerros(Partida &p)
{
    pthread_t hilos[cant_Perros];
    DatoPerro datos[cant_Perros];
    int creados = 0;

    for (int i = 0; i < cant_Perros; i++)
    {
        datos[i].partida = &p;
        datos[i].id = i;

        if (pthread_create(&hilos[i], nullptr, threadPerro, &datos[i]) != 0)
        {
            cout << "Error al crear el hilo del Perro " << i + 1 << endl;
            break;
        }

        creados++;

    }

    for (int i = 0; i < creados; i++)
    {
        pthread_join(hilos[i], nullptr);
    }

    return creados == cant_Perros;

}
