#include "torneo.h"
#include "../partida/partida.h"
#include <iostream>
#include <signal.h>

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
    a.partidasActivas.assign(cantidad, nullptr);
    a.mesasActivas = 0;
    a.maxSimultaneas = 0;
    a.detener = false;

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

    while (mesa == -1 && !a.detener)
    {
        for (int i =0; i< a.cantMesas; i++)
        {
            if (a.ocupadas[i] == 0)
            {
                a.ocupadas[i] = 1;
                a.mesasActivas++;

                if(a.mesasActivas > a.maxSimultaneas)
                {
                    a.maxSimultaneas = a.mesasActivas;
                }
                mesa = i + 1;
                break;
            }
        }

        if (mesa== -1 && !a.detener)
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
            a.mesasActivas--;
            pthread_cond_signal(&a.mesaDisponible);
        }
    }
    pthread_mutex_unlock(&a.mutex);

}

bool registrarPartidaActiva(AdminMesas &a, int mesa, Partida &p)
{
    pthread_mutex_lock(&a.mutex);
    bool registrado = false;

    if (!a.detener && mesa >= 1 && mesa <= a.cantMesas)
    {
        if (a.ocupadas[mesa - 1] == 1 && a.partidasActivas[mesa - 1] == nullptr)
        {
            a.partidasActivas[mesa - 1] = &p;
            registrado = true;
        }
    }
    pthread_mutex_unlock(&a.mutex);
    return registrado;
}

void quitarPartidaActiva(AdminMesas &a, int mesa, Partida &p)
{
    pthread_mutex_lock(&a.mutex);

    if (mesa >= 1 && mesa <= a.cantMesas)
    {
        if (a.partidasActivas[mesa - 1] == &p)
        {
            a.partidasActivas[mesa - 1] = nullptr;
        }
    }

    pthread_mutex_unlock(&a.mutex);
}

void detenerMesas(AdminMesas &a)
{
    pthread_mutex_lock(&a.mutex);
    a.detener = true;
    pthread_cond_broadcast(&a.mesaDisponible);

    for (int i = 0; i< a.cantMesas; i++)
    {
        if (a.partidasActivas[i] != nullptr)
        {
            detenerPartida(*a.partidasActivas[i]);
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
    info->mesaUsada = -1;
    info->capturasFinales = 0;
    info->ganador = -1;

    int mesa= solicitarMesa(*info->admin);
    if (mesa == -1)
    {
        return nullptr;
    }
    info->mesaUsada = mesa;
    Partida p;

    if (!iniciarPartida(p))
    {
        std::cerr << "Mesa " << mesa << " | Error al iniciar partida" << std::endl;

        liberarMesa(*info->admin, mesa);
        return nullptr;
    }

    p.mesa = mesa;
    p.jugadorPuma = info->pareja.jugadorPuma;
    p.jugadorPerro = info->pareja.jugadorPerros;
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

    if (!registrarPartidaActiva(*info->admin, mesa, p))
    {
        cerrarPartida(p);
        liberarMesa(*info->admin, mesa);
        return nullptr;
    }

    bool resultado = threadsFichas(p);

    info->capturasFinales = p.capturas;

    if (resultado)
    {
        if (ganaPuma(p.capturas))
        {
            info->ganador = 1;
        }
        else if (gananPerros(p.tablero, p.trono))
        {
            info->ganador = 2;
        }
        else
        {
            info->ganador = 0;
        }
    }

    quitarPartidaActiva(*info->admin, mesa, p);
    cerrarPartida(p);
    liberarMesa(*info->admin, mesa);
    info->correcto = resultado;
    
    return nullptr;
}

void* threadSenales(void* dato)
{
    AdminMesas *admin = (AdminMesas *)dato;


    sigset_t senales;
    sigemptyset(&senales);
    sigaddset(&senales, SIGINT);
    sigaddset(&senales, SIGUSR1);


    int recibida = 0;


    if (sigwait(&senales, &recibida) == 0)
    {
        if (recibida == SIGINT)
        {
            std::cout << "\nServidor: Ctrl+C recibido"
                      << std::endl;


            std::cout << "Deteniendo partidas..."
                      << std::endl;


            detenerMesas(*admin);
        }
    }


    return nullptr;
}

bool ejecutarTorneo(int n, int m, char tablero[TAM][TAM], char trono[TAM_TRONO], int retardoMin, int retardoMax, unsigned int semilla)
{
    Emparejamiento e;

    if(!iniciarEmparejamiento(e))
    {
        std::cerr << "Error al iniciar emparejamiento" << std::endl;
        return false;
    }

    if (!recibirJugadores(e, n))
    {
        std::cerr << "Error al recibir jugadores" << std::endl;
        cerrarEmparejamiento(e);
        return false;
    }

    std::cout << "Parejas formadas: " << e.parejas.size() << std::endl;

    if (e.jugadorEsperando != -1)
    {
        std::cout << "Jugador " << e.jugadorEsperando << " quedo sin pareja " << std::endl;
    }

    AdminMesas admin;

    if (!iniciarMesas(admin, m))
    {
        std::cerr << "Error al iniciar mesas" << std::endl;
        cerrarEmparejamiento(e);
        return false;
    }

    pthread_t hiloSenales;

    if (pthread_create(&hiloSenales, nullptr, threadSenales, &admin) != 0)
    {
        std::cerr << "Error al crear el thread de señales" << std::endl;

        cerrarMesas(admin);
        cerrarEmparejamiento(e);
        return false;
    }

    int total = (int)e.parejas.size();

    std::vector<pthread_t> hilos(total);
    std::vector<DatosPareja> datos(total);

    int creados = 0;

    for (int i = 0; i<total; i++)
    {
        datos[i].pareja = e.parejas[i];
        datos[i].admin = &admin;
        datos[i].retardoMin= retardoMin;
        datos[i].retardoMax = retardoMax;
        datos[i].semilla = semilla;
        datos[i].correcto = false;
        datos[i].mesaUsada = -1;
        datos[i].capturasFinales = 0;
        datos[i].ganador = -1;

        for (int fila = 0; fila < TAM; fila++)
        {
            for (int columna = 0; columna < TAM; columna++)
            {
                datos[i].tableroInicial[fila][columna] = tablero[fila][columna];
            }
        }

        for (int j=0; j< TAM_TRONO; j++)
        {
            datos[i].tronoInicial[j] = trono[j];
        }

        if (pthread_create(&hilos[i], nullptr, threadParejas, &datos[i]) !=0)
        {
            std::cerr << "Error al crear thread de pareja " << i + 1 << std::endl;
            break;
        }

        creados++;
    }

    bool  correcto = (creados == total);

    for (int i = 0; i < creados; i++)
    {
        if (pthread_join(hilos[i], nullptr) != 0)
        {
            correcto = false;
        }

        if (!datos[i].correcto)
        {
            correcto = false;
        }
    }

    pthread_kill(hiloSenales, SIGUSR1);
    pthread_join(hiloSenales, nullptr);
    pthread_mutex_lock(&admin.mutex);


    bool interrumpido = admin.detener;


    pthread_mutex_unlock(&admin.mutex);


    if (interrumpido)
    {
        correcto = false;


        std::cout << "\nServidor: torneo interrumpido" << std::endl;
    }

    std::cout << "\n========== RESUMEN DEL TORNEO ==========" << std::endl;


    std::cout << "Jugadores recibidos: " << n << std::endl;
    std::cout << "Parejas formadas: " << total << std::endl;
    std::cout << "Mesas disponibles: " << m << std::endl;

    for (int mesa = 1; mesa <= m; mesa++)
    {
        int partidasMesa = 0;


        std::cout << "\nMesa " << mesa << ":" << std::endl;


        for (int i = 0; i < creados; i++)
        {
            if (datos[i].mesaUsada == mesa)
            {
            partidasMesa++;


            std::cout << "  Pareja " << i + 1 << " (Puma: Jugador " << datos[i].pareja.jugadorPuma << ", Perros: Jugador " << datos[i].pareja.jugadorPerros << ")" << std::endl;


            std::cout << "  Resultado: ";


            if (datos[i].ganador == 1)
            {
                std::cout << "Gana el Puma";
            }
            else if (datos[i].ganador == 2)
            {
                std::cout << "Ganan los Perros";
            }
            else if (datos[i].ganador == 0)
            {
                std::cout << "Sin ganador";
            }
            else
            {
                std::cout << "Error en la partida";
            }


            std::cout << " | Capturas: " << datos[i].capturasFinales << std::endl;
        }
    }


    if (partidasMesa == 0)
    {
        std::cout << "  Sin partidas" << std::endl;
    }


    std::cout << "  Partidas en esta mesa: " << partidasMesa << std::endl;
    }

    for (int i = 0; i < creados; i++)
    {
        if (datos[i].mesaUsada == -1)
        {
            std::cout << "Pareja " << i + 1 << " no obtuvo mesa" << std::endl;
        }
    }


    if (creados < total)
    {
        std::cout << "Parejas sin thread creado: " << total - creados << std::endl;
    }


    std::cout << "\nMaximo de mesas simultaneas: "  << admin.maxSimultaneas << " de " << m << std::endl;


    std::cout << "Mesas ocupadas al finalizar: " << admin.mesasActivas << std::endl;


    if (admin.maxSimultaneas <= m && admin.mesasActivas == 0)
    {
        std::cout << "Control de mesas: CORRECTO" << std::endl;
    }
    else
    {
        std::cout << "Control de mesas: REVISAR" << std::endl;
    }


    std::cout << "========================================" << std::endl;
    std::cout << "                KOMIKAN                 " << std::endl;
    std::cout << "========================================" << std::endl;

    cerrarMesas(admin);
    cerrarEmparejamiento(e);

    if (correcto)
    {
        std::cout << "Todas las parejas fueron procesadas" << std::endl;
    }

    return correcto;
}


