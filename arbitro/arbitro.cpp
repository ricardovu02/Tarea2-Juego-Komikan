#include "arbitro.h"
#include <iostream>

using namespace std;

static pthread_mutex_t mutexConsola = PTHREAD_MUTEX_INITIALIZER;

void registrarMov(const Partida &p, const char *ficha, int numero, int origen, int destino)
 {
    pthread_mutex_lock(&mutexConsola);
    cout << "Mesa " << p.mesa << " | " << ficha;

    if (numero >0)
    {
        cout << " " << numero;
    }
    cout << ": " << origen << " -> " << destino << endl;

    pthread_mutex_unlock(&mutexConsola);
 }

 void revisarFin(Partida &p)
 {
    if (ganaPuma(p.capturas) || gananPerros(p.tablero, p.trono))
    {
        p.terminada = true;
        pthread_cond_broadcast(&p.avisoArbitro);
    }
 }

 void* threadArbitro(void* dato)
 {
    Partida *p = (Partida *)dato;
    pthread_mutex_lock(&p->mutex);
    pthread_mutex_lock(&mutexConsola);

    cout << "\n=== MESA" << p->mesa << ": TABLERO INICIAL ===" << endl;

    mostrarTablero(p->tablero, p->trono);

    pthread_mutex_unlock(&mutexConsola);

    if (gananPerros(p->tablero, p->trono))
    {
        p->terminada = true;
    }

    p->arbitroListo = true;
    pthread_cond_broadcast(&p->avisoArbitro);

    while (!p->terminada || p->capturaPendiente)
    {
        while (!p->capturaPendiente && !p->terminada)
        {
            pthread_cond_wait(&p->avisoArbitro, &p->mutex);
        }

        if (p->capturaPendiente)
        {
            pthread_mutex_lock(&mutexConsola);

            cout << "\n=== MESA " << p->mesa << ": CAPTURA ===" << endl;

            cout << "Mesa " << p->mesa << " | Puma " << p->origenCaptura << " -> " << p->destinoCaptura << " captura al perro " << p->perroCapturado << endl;
            cout << "Capturas " << p->capturas << endl;

            mostrarTablero(p->tablero, p->trono);

            pthread_mutex_unlock(&mutexConsola);

            p->capturaPendiente = false;
            pthread_cond_broadcast(&p->avisoArbitro);
        }
    }

    pthread_mutex_lock(&mutexConsola);

    if (p->interrumpida)
    {
        cout << "Mesa " << p->mesa << " | Partida interrumpida por el servidor" << endl;
    }
    else if (p->abandono == abandono_puma)
    {
        cout << "Mesa " << p->mesa << " | El jugador del Puma abandono la partida" << endl;
        cout << "Mesa " << p->mesa << " | Ganador: jugador de los Perros por abandono" << endl;
    }
    else if (p->abandono == abandono_perro)
    {
        cout << "Mesa " << p->mesa << " | El jugador de los Perros abandono la partida" << endl;
        cout << "Mesa " << p->mesa << " | Ganador: jugador del Puma por abandono" << endl;
    }
    else if (ganaPuma(p->capturas))
    {
        cout << "\n===  MESA " << p->mesa << ": RESULTADO ===" << endl;

        cout << "Ganador: Puma " << endl;
        cout <<"Capturas: " << p->capturas << endl;

        mostrarTablero(p->tablero, p->trono);
    }
    else if (gananPerros(p->tablero, p->trono))
    {
        cout << "\n=== MESA " << p->mesa << ": RESULTADO ===" << endl;

        cout << "Ganadores: Perros" << endl;
        cout << "Puma inmovilizado" << endl;

        mostrarTablero(p->tablero, p->trono);
    }
    else
    {
        cout << "Mesa " << p->mesa << " | Partida detenida sin ganador" << endl;
        cout <<"Estado final mesa " << p->mesa << " DETENIDA" << endl;
        mostrarTablero(p->tablero, p->trono);
    }

    pthread_mutex_unlock(&mutexConsola);
    pthread_mutex_unlock(&p->mutex);

    return nullptr;
 }