#ifndef TORNEO_H
#define TORNEO_H
#include <pthread.h>
#include <vector>

struct Pareja
{
    int jugadorPuma;
    int jugadorPerros;
};

struct Emparejamiento
{
    int jugadorEsperando;
    std::vector<Pareja> parejas;
    pthread_mutex_t mutex;
};

bool iniciarEmparejamiento(Emparejamiento &e);
void registrarJugador(Emparejamiento &e, int jugador);
bool recibirJugadores(Emparejamiento &e, int cantidad);
void cerrarEmparejamiento(Emparejamiento &e);

#endif