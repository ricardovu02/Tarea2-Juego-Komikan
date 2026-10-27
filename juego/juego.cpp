#include "juego.h"
#include <iostream>

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
    for (int fila = 0; fila < TAM; fila++)
    {
        for (int columna = 0; columna < TAM; columna++)
        {
            cout << tablero[fila][columna];

            if (columna < TAM - 1)
            {
                cout << "---";
            }
        }
        cout << endl;

        if (fila< TAM - 1)
        {
            for (int columna = 0; columna < TAM; columna++)
            {
                cout << "|";

                if (columna < TAM - 1)
                {
                    cout << "   ";
                }
            }
            cout << endl;
        }
    }
}


