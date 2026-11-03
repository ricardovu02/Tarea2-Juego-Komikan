#include "torneo.h"

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