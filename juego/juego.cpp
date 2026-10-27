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
    const int ANCHO_CELDA = 7;
    const int ALTO_CELDA = 4;

    string guiones(ANCHO_CELDA, '-');
    string espaciosCelda(ANCHO_CELDA, ' ');


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

        if (fila< TAM - 1)
        {
            for (int espacio = 0; espacio <  ALTO_CELDA; espacio++)
            {
                if (fila == 1 && espacio == 1)
                {
                    cout <<"|       |    KOMIKAN    |       |       " << endl;

                }
                else
                 {
                    for (int columna = 0; columna < TAM; columna++)
                 {
                 cout << "|";

                  if (columna < TAM - 1)
                  { 
                    cout << espaciosCelda;
                   }
                 }
            }
                
                cout << endl;
            }
            
        }
    }
}


