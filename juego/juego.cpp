#include "juego.h"
#include <iostream>
#include <string>

using namespace std;

void inicializarTablero(char tablero[TAM][TAM])
{
    for (int fila =0; fila < TAM; fila++)
    {
        for (int columna=0; columna <TAM; columna++)
        {
            tablero[fila][columna] = '.';
        }
    }
}
void mostrarTablero(const char tablero[TAM][TAM])
{
    const int ANCHO_CELDA = 11;
    const int ALTO_CELDA = 7;


    string guiones(ANCHO_CELDA, '-');
    string espaciosCelda(ANCHO_CELDA, ' ');


    const string logo[5] = {
        "█   █  ███  █   █ █████ █   █  ███  █   █",
        "█  █  █   █ ██ ██   █   █  █  █   █ ██  █",
        "███   █   █ █ █ █   █   ███   █████ █ █ █",
        "█  █  █   █ █   █   █   █  █  █   █ █  ██",
        "█   █  ███  █   █ █████ █   █ █   █ █   █"
    };


    cout << string(24, ' ') << "T" << endl;
cout << string(21, ' ') << "/  |  \\" << endl;
cout << string(18, ' ') << "/     |     \\" << endl;
cout << string(15, ' ') << "/        |        \\" << endl;
cout << string(12, ' ') << ".-----------+-----------." << endl;
cout << string(9, ' ') << "/  |           |           |  \\" << endl;
cout << string(6, ' ') << "/     |           |           |     \\" << endl;
cout << string(3, ' ') << "/        |           |           |        \\" << endl;


    for (int fila = 0; fila < TAM; fila++)
    {
        for (int columna = 0; columna < TAM; columna++)
        {
            cout << tablero[fila][columna];


            if (columna < TAM - 1)
            {
                cout << guiones;
            }
        }


        cout << endl;


        if (fila < TAM - 1)
        {
            for (int espacio = 0; espacio < ALTO_CELDA; espacio++)
            {
                for (int columna = 0; columna < TAM; columna++)
                {
                    cout << "|";


                    if (columna < TAM - 1)
                    {
                        cout << espaciosCelda;
                    }
                }


                cout << endl;
            }
        }
    }


    cout << endl;


    for (int i = 0; i < 5; i++)
    {
        cout << "   " << logo[i] << endl;
    }
}


