#include "partida.h"
#include "../arbitro/arbitro.h"
#include <iostream>
#include <cstdlib>
#include <unistd.h>

using namespace std;

bool iniciarPartida(Partida &p)
{
    inicializarTablero(p.tablero);
    inicializarTrono(p.trono);

    p.capturas = 0;
    p.terminada = false;
    p.retardoMin = 50;
    p.retardoMax = 300;
    p.semilla = 1;
    p.posPuma = -1;
    p.mesa = 1;
    p.arbitroListo = false;
    p.capturaPendiente = false;
    p.perroCapturado = -1;
    p.origenCaptura = -1;
    p.destinoCaptura = -1;
    
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
    if (pthread_cond_init(&p.avisoArbitro, nullptr) != 0)
    {
        pthread_mutex_destroy(&p.mutex);
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
    pthread_cond_destroy(&p.avisoArbitro);
    pthread_mutex_destroy(&p.mutex);
}

int pausa(Partida &p, unsigned int &estado)
{
    int rango = p.retardoMax - p.retardoMin + 1;
    int tiempo = p.retardoMin + rand_r(&estado) % rango;

    usleep(tiempo * 1000);
    return tiempo;
}

void moverPerro(Partida &p, int id, unsigned int &estado)
{
    int origen = p.posPerros[id];
    int destinos[TAM * TAM + TAM_TRONO];
    int cantidad = 0;
    const int total = TAM * TAM + TAM_TRONO;

    for (int destino = 0; destino < total; destino++)
    {
        if (movValido(p.tablero, p.trono, origen, destino))
        {
            destinos[cantidad] = destino;
            cantidad++;
        }
    }
    if (cantidad > 0)
    {
        int destino = destinos[rand_r(&estado) % cantidad];

        if (movimiento(p.tablero, p.trono, origen, destino))
        {
            p.posPerros[id] = destino;

            registrarMov(p, "Perro", id + 1, origen, destino);
        }
    }
}

void moverPuma(Partida &p, unsigned int &estado)
{
    int origen = p.posPuma;
    const int total = TAM * TAM + TAM_TRONO;
    int destinos[TAM* TAM + TAM_TRONO];
    int cantidad = 0;

    for (int destino = 0; destino < total; destino++)
    {
        char copia[TAM][TAM];
        char copiaTrono[TAM_TRONO];

        for (int fila = 0; fila < TAM; fila++)
        {
            for (int columna = 0; columna < TAM; columna++)
            {
                copia[fila][columna] = p.tablero[fila][columna];
            }
        }

        for (int i = 0; i < TAM_TRONO; i++)
        {
            copiaTrono[i] = p.trono[i];
        }
        int pruebaCapturas = p.capturas;

        if (captura(copia, copiaTrono, origen, destino, pruebaCapturas))
        {
            destinos[cantidad] = destino;
            cantidad++;
        }
    }
    if (cantidad > 0)
    {
        int destino = destinos[rand_r(&estado) % cantidad];

        if (captura(p.tablero, p.trono, origen, destino, p.capturas))
        {
            p.posPuma = destino;

            for (int i = 0; i < cantidad_perros; i++)
            {
                int pos = p.posPerros[i];

                if ( pos < 0)
                {
                    continue;
                }
                char ficha;

                if (pos < TAM * TAM)
                {
                    ficha = p.tablero[pos / TAM][pos % TAM];

                }
                else
                {
                    ficha = p.trono[pos - TAM * TAM];
                }

                if (ficha != 'D')
                {
                    p.posPerros[i] = -1;

                    p.perroCapturado = i + 1;
                    p.origenCaptura =origen;
                    p.destinoCaptura= destino;
                    p.capturaPendiente = true;

                    pthread_cond_broadcast(&p.avisoArbitro);
                    
                    break;
                }
            }
        }
        return;
    }

    for (int destino = 0; destino < total; destino++)
    {
        if (movValido(p.tablero, p.trono, origen, destino))
        {
            destinos[cantidad] = destino;
            cantidad++;
        }
    }
    if (cantidad >0)
    {
        int destino = destinos[rand_r(&estado) % cantidad];

        if (movimiento(p.tablero, p.trono, origen, destino))
        {
            p.posPuma = destino;

            registrarMov(p, "Puma", 0, origen, destino);
        }
    }
}

void* threadPuma(void* dato)
{
    Partida *p = (Partida *)dato;
    unsigned int estado = p->semilla + 100;

    while (true)
    {
    pausa(*p, estado);
    pthread_mutex_lock(&p->mutex);

    while (p->capturaPendiente)
    {
        pthread_cond_wait(&p->avisoArbitro, &p->mutex);
    }

    if (p->terminada)
    {
        pthread_mutex_unlock(&p->mutex);
        break;
    }


    if (!gananPerros(p->tablero, p->trono))
    {
        moverPuma(*p, estado);
    }
    revisarFin(*p);
    pthread_mutex_unlock(&p->mutex);
    }
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
    unsigned int estado = p->semilla + info->id + 1;

    while (true)
    {
        pausa(*p, estado);
        pthread_mutex_lock(&p->mutex);

        while(p->capturaPendiente)
        {
            pthread_cond_wait(&p->avisoArbitro, &p->mutex);
        }

        if (p->terminada || p->posPerros[info->id] == -1)
        {
            pthread_mutex_unlock(&p->mutex);
            break;
        }
        moverPerro(*p, info->id, estado);
        revisarFin(*p);
        pthread_mutex_unlock(&p->mutex);
    }
    return nullptr;
}

bool threadsFichas(Partida &p)
{
    pthread_t arbitro;
    pthread_t hiloPuma;
    pthread_t hilos[cantidad_perros];
    DatoPerro datos[cantidad_perros];
    int creados = 0;

    if(pthread_create(&arbitro, nullptr, threadArbitro, &p) != 0) 
    {
        cout << "Error al crear el Arbitro" << endl;
        return false;
    }
    pthread_mutex_lock(&p.mutex);

    while (!p.arbitroListo)
    {
        pthread_cond_wait(&p.avisoArbitro, &p.mutex);
    }

    bool terminoAlInicio =p.terminada;

    pthread_mutex_unlock(&p.mutex);

    if(terminoAlInicio)
    {
        pthread_join(arbitro, nullptr);
        return true;
    }

    if(pthread_create(&hiloPuma, nullptr, threadPuma, &p) != 0) 
    {
        cout << "Error al crear el hilo del Puma" << endl;

        pthread_mutex_lock(&p.mutex);
        p.terminada = true;
        pthread_cond_broadcast(&p.avisoArbitro);
        pthread_mutex_unlock(&p.mutex);

        pthread_join(arbitro, nullptr);
        return false;
    }

    for (int i = 0; i < cantidad_perros; i++)
    {
        datos[i].partida = &p;
        datos[i].id = i;

        if (pthread_create(&hilos[i], nullptr, threadPerro, &datos[i]) != 0)
        {
            cout << "Error al crear el hilo del Perro " << i + 1 << endl;

            pthread_mutex_lock(&p.mutex);
            p.terminada = true;
            pthread_cond_broadcast(&p.avisoArbitro);
            pthread_mutex_unlock(&p.mutex);
            break;
        }

        creados++;

    }

    pthread_join(hiloPuma, nullptr);

    for (int i = 0; i < creados; i++)
    {
        pthread_join(hilos[i], nullptr);
    }

    pthread_join(arbitro, nullptr);

    return creados == cantidad_perros;

}
