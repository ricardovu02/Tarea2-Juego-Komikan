#include "torneo.h"
#include "../partida/partida.h"
#include <iostream>

struct DatoJugador
{
    Emparejamiento *emparejamiento;
    int jugador;
};

void* threadJugador(void* dato)
{
    DatoJugador *info = (DatoJugador *)dato;
    registrarJugador(*info->emparejamiento, info->jugador);
    return nullptr;
}

bool iniciarEmparejamiento(Emparejamiento &e)
{
    e.jugadorEsperando = -1;
    e.parejas.clear();

    if (pthread_mutex_init(&e.mutex, nullptr) != 0)
    {
        return false;
    }

    return true;
}

void registrarJugador(Emparejamiento &e, int jugador)
{
    pthread_mutex_lock(&e.mutex);

    if (e.jugadorEsperando == -1)
    {
        e.jugadorEsperando = jugador;
    }
    else
    {
        Pareja nueva;

        nueva.jugadorPuma = e.jugadorEsperando;
        nueva.jugadorPerros = jugador;

        e.parejas.push_back(nueva);
        e.jugadorEsperando = -1;
    }

    pthread_mutex_unlock(&e.mutex);
}

bool recibirJugadores(Emparejamiento &e, int cantidad)
{
    if (cantidad < 1)
    {
        return false;
    }

    std::vector<pthread_t> hilos(cantidad);
    std::vector<DatoJugador> datos(cantidad);
    int creados = 0;

    for ( int i = 0; i < cantidad; i++)
    {
        datos[i].emparejamiento = &e;
        datos[i].jugador = i + 1;

        if (pthread_create(&hilos[i], nullptr, threadJugador, &datos[i]) != 0)
        {
            break;
        }

        creados++;
    }

    for (int i = 0; i < creados; i++)
    {
        pthread_join(hilos[i], nullptr);
    }

    return creados == cantidad;
}

void cerrarEmparejamiento(Emparejamiento &e)
{
    pthread_mutex_destroy(&e.mutex);
}

bool iniciarMesas(AdminMesas &a, int cantidad)
{
    if (cantidad < 1)
    {
        return false;
    }

    a.cantMesas = cantidad;
    a.ocupadas.assign(cantidad, 0);

    if (pthread_mutex_init(&a.mutex, nullptr) != 0)
    {
        return false;
    }

    if (pthread_cond_init(&a.mesaDisponible, nullptr) != 0)
    {
        pthread_mutex_destroy(&a.mutex);
        return false;
    }

    return true;
}

int solicitarMesa(AdminMesas &a)
{
    pthread_mutex_lock(&a.mutex);

    int mesa = -1;

    while (mesa == -1)
    {
        for (int i =0; i< a.cantMesas; i++)
        {
            if (a.ocupadas[i] == 0)
            {
                a.ocupadas[i] = 1;
                mesa = i + 1;
                break;
            }
        }

        if (mesa== -1)
        {
            pthread_cond_wait(&a.mesaDisponible, &a.mutex);
        }
    }
    pthread_mutex_unlock(&a.mutex);
    return mesa;
}

void liberarMesa(AdminMesas &a, int mesa)
{
    pthread_mutex_lock(&a.mutex);

    if (mesa >= 1 && mesa <= a.cantMesas)
    {
        if (a.ocupadas[mesa - 1] == 1)
        {
            a.ocupadas[mesa - 1] = 0;
            pthread_cond_signal(&a.mesaDisponible);
        }
    }
    pthread_mutex_unlock(&a.mutex);

}

void cerrarMesas(AdminMesas &a)
{
    pthread_cond_destroy(&a.mesaDisponible);
    pthread_mutex_destroy(&a.mutex);
}

void* threadParejas(void* dato)
{
    DatosPareja *info = (DatosPareja *)dato;

    info->correcto = false;

    int mesa= solicitarMesa(*info->admin);
    Partida p;

    if (!iniciarPartida(p))
    {
        std::cerr << "Mesa " << mesa << " | Error al iniciar partida" << std::endl;

        liberarMesa(*info->admin, mesa);
        return nullptr;
    }

    p.mesa = mesa;
    p.retardoMin = info->retardoMin;
    p.retardoMax = info->retardoMax;
    p.semilla = info->semilla + (unsigned int)info->pareja.jugadorPuma;

    for (int fila = 0; fila < TAM; fila++)
    {
        for (int columna = 0; columna < TAM; columna++)
        {
            p.tablero[fila][columna] = info->tableroInicial[fila][columna];
        }
    }

    for (int i = 0; i < TAM_TRONO; i++)
    {
        p.trono[i] = info->tronoInicial[i];
    }

    if (!ubicacionFichas(p))
    {
        std::cerr << "Mesa " << " | Posiciones Invalidas" << std::endl;

        cerrarPartida(p);
        liberarMesa(*info->admin, mesa);
        return nullptr;
    }
    bool resultado = threadsFichas(p);

    cerrarPartida(p);
    liberarMesa(*info->admin, mesa);
    info->correcto = resultado;
    
    return nullptr;
}

