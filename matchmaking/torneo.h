#ifndef TORNEO_H
#define TORNEO_H
#include <pthread.h>
#include <vector>
#include "../juego/juego.h"

struct Partida;

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

struct AdminMesas
{
    int cantMesas;
    std::vector<int> ocupadas;
    std::vector<Partida*> partidasActivas;
    int mesasActivas;
    int maxSimultaneas;
    bool detener;
    pthread_mutex_t mutex;
    pthread_cond_t mesaDisponible;
};

struct DatosPareja
{
    Pareja pareja;
    AdminMesas *admin;

    char tableroInicial[TAM][TAM];
    char tronoInicial[TAM_TRONO];
    int retardoMin;
    int retardoMax;
    unsigned int semilla;
    bool correcto;
    int mesaUsada;
    int capturasFinales;
    int ganador;
};

bool iniciarEmparejamiento(Emparejamiento &e);
void registrarJugador(Emparejamiento &e, int jugador);
bool recibirJugadores(Emparejamiento &e, int cantidad);
void cerrarEmparejamiento(Emparejamiento &e);
bool iniciarMesas(AdminMesas &a, int cantidad);
int solicitarMesa(AdminMesas &a);
void liberarMesa(AdminMesas &a, int mesa);
void detenerMesas(AdminMesas &a);
void cerrarMesas(AdminMesas &a);
void* threadParejas(void* dato);
bool ejecutarTorneo(int n, int m, char tablero[TAM][TAM], char trono[TAM_TRONO], int retardoMin, int retardoMax, unsigned int semilla);
bool registrarPartidaActiva(AdminMesas &a, int mesa, Partida &p);
void quitarPartidaActiva(AdminMesas &a, int mesa, Partida &p);

#endif