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
    
    for (int i = 0; i < cantidad_perros; i++)
    {
        p.posPerros[i] = -1;
    }

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
            if (perros < cantidad_perros)
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
    if (perros != cantidad_perros)
    {
        cout << "Cantidad de Perros: " << perros << endl;
        cout << "Numero invalido. Se requieren exactamente " << cantidad_perros << " Perros." << endl;
    }
    return pumas == 1 && perros == cantidad_perros;
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

bool threadsFichas(Partida &p)
{
    pthread_t hiloPuma;
    pthread_t hilos[cantidad_perros];
    DatoPerro datos[cantidad_perros];
    int creados = 0;

    if(pthread_create(&hiloPuma, nullptr, threadPuma, &p) != 0) 
    {
        cout << "Error al crear el hilo del Puma" << endl;
        return false;
    }

    for (int i = 0; i < cantidad_perros; i++)
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

    pthread_join(hiloPuma, nullptr);

    for (int i = 0; i < creados; i++)
    {
        pthread_join(hilos[i], nullptr);
    }

    return creados == cantidad_perros;

}
