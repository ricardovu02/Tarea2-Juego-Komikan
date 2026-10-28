#include <iostream>
#include <string>
#include <sstream>
#include "juego/juego.h"
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
    if(argc != 3 && argc != 4)
    {
        cerr << "Uso: ./torneo N M [config.txt]" << endl;
        return 1;
    }

    int n;
    int m;

    stringstream ssN(argv[1]);
    stringstream ssM(argv[2]);

    bool nValido = (ssN >> n ) && ssN.eof();
    bool mValido = (ssM >> m ) && ssM.eof();

    if(!nValido && !mValido)
    {
        cerr << "Error: N y M tienen que ser numeros enteros" << endl;
        return 1;
    }

    if (!nValido)
    {
        cerr << "Error: N tiene que ser un numero entero" << endl;
        return 1;
    }

    if (!mValido)
    {
        cerr << "Error: M tiene que ser un numero entero" << endl;
        return 1;
    }

    if (n <= 0 && m<= 0)
    {
        cerr << "Error: N y M tienen que ser mayores que 0" << endl; 
        return 1;
    }
    if (n <= 0)
    {
        cerr << "Error: N tiene que ser mayor que 0" << endl; 
        return 1;
    }

    if (m <= 0)
    {
        cerr << "Error: M tiene que ser mayor que 0" << endl; 
        return 1;
    }

    cout << "Cantidad de jugadores: " << n << endl;
    cout << "Maximo de mesas: " << m << endl; 

    char tablero[TAM][TAM];
    char trono[TAM_TRONO];

    inicializarTablero(tablero);
    inicializarTrono(trono);

    if(argc == 3)
    {
        trono[0] = 'P';
    tablero[2][0] = 'D';
    tablero[2][4] = 'D';

    for(int columna = 0; columna< TAM; columna++)
    {
        tablero[3][columna] = 'D';
        tablero[4][columna] = 'D';
    }
    }



    if (argc == 4)
    {
        ifstream archivo(argv[3]);

        if (!archivo.is_open())
        {
            cerr << "Error: no se pudo abrir el archivo de configuracion" << endl;
            return 1;
        }

        string parametros;
        int retardoMin;
        int retardoMax;
        int semilla;

        if (!(archivo >> parametros >> retardoMin) || parametros!= "retardo_min")
        {
            cerr << "Error al leer retardo_min" << endl;
            return 1;
        }
        if (!(archivo >> parametros >> retardoMax) || parametros != "retardo_max")
        {
            cerr << "Error al leer retardo_max" << endl;
            return 1;
        }
        if (!(archivo >> parametros >> semilla) || parametros != "semilla")
        {
            cerr << "Error al leer semilla" << endl;
            return 1;
        }
        if (retardoMin < 0 || retardoMax < retardoMin)
        {
            cerr << "Error: retardos invalidos" << endl;
            return 1;
        }

        cout << "Archivo de configuracion: " << argv[3] << endl;
        cout << "Retardo Minimo: " << retardoMin << "ms" << endl;
        cout << "Retardo Maximo: " << retardoMax << "ms" << endl;
        cout << "Semilla: " << semilla << endl;

        string ficha;
        string zona;

        if(!(archivo >> ficha >> zona) || ficha != "puma")
        {
            cerr << "Error al leer la posicion del Puma";
            return 1;
        }
        
        if (zona == "trono")
        {
            int posicion;

        if (!(archivo >> posicion) || posicion < 0 || posicion >= TAM_TRONO)
        {
            cerr << "Error: posicion del Puma invalida" << endl;
            return 1;
        }

        if (trono[posicion] != '.')
        {
            cerr << "Error: posicion ocupada" << endl;
            return 1;
        }

        trono[posicion] = 'P';

        }

        else if (zona == "tablero")
        {
            int fila;
            int columna;

            if (!(archivo >> fila >> columna) || fila < 0 || fila >= TAM || columna < 0 || columna >= TAM)
            {
                cerr << "Error: posicion del Puma invalida" << endl;
                return 1;
            }

            if (tablero[fila][columna] != '.')
            {
                cerr << "Error: posicion ocupada" << endl;
                return 1;
            }

            tablero[fila][columna] = 'P';
        }
        else
        {
            cerr << "Error: zona del Puma invalida" << endl;
            return 1;
        }

        for (int i = 0; i<12; i++)
        {
            if (!(archivo >> ficha >> zona) || ficha != "perro")
            {
                cerr << "Error al leer la posicion de un Perro" << endl;
                return 1;
            }
        
        if (zona == "trono")
        {
            int posicion;

        if (!(archivo >> posicion) || posicion < 0 || posicion >= TAM_TRONO)
        {
            cerr << "Error: posicion del Perro invalida" << endl;
            return 1;
        }

        if (trono[posicion] != '.')
        {
            cerr << "Error: posicion ocupada" << endl;
            return 1;
        }

        trono[posicion] = 'D';


        }

        else if (zona == "tablero")
        {
            int fila;
            int columna;

            if (!(archivo >> fila >> columna) || fila < 0 || fila >= TAM || columna < 0 || columna >= TAM)
            {
                cerr << "Error: posicion del Perro invalida" << endl;
                return 1;
            }

            if (tablero[fila][columna] != '.')
        {
            cerr << "Error: posicion ocupada" << endl;
            return 1;
        }

        tablero[fila][columna] = 'D';

        }
        else
        {
            cerr << "Error: zona del Perro invalida" << endl;
            return 1;
        }
       }
    }

    

    cout<<endl;
    cout<< "TABLERO INICIAL: " << endl;
    mostrarTablero(tablero, trono);

    


    return 0;

}