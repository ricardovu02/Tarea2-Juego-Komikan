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

void inicializarTrono(char trono[TAM_TRONO])
{
    for (int i=0; i<TAM_TRONO; i++)
    {
        trono[i] = '.';
    }
}

void mostrarTablero(const char tablero[TAM][TAM], const char trono[TAM_TRONO])
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


    string ficha_arriba = "T";
    
    if (trono[0] == 'P')
    {
        ficha_arriba = "Puma";
    }
    else if (trono[0] == 'D')
    {
        ficha_arriba = "Perro";
    }

    cout << string(24 - ficha_arriba.length() / 2, ' ') << ficha_arriba << endl;
    cout << string(21, ' ') << "/  |  \\" << endl;
    cout << string(18, ' ') << "/     |     \\" << endl;
    cout << string(15, ' ') << "/        |        \\" << endl;

    string lineaTrono = string(12, ' ') + ".-----------+-----------.";

    if (trono[1] == 'P')
    {
        lineaTrono.replace(10, 4, "Puma");
    }
    else if (trono[1] == 'D')
    {
        lineaTrono.replace(10, 5, "Perro");
    }
     if (trono[2] == 'P')
    {
        lineaTrono.replace(34, 4, "Puma");
    }
    else if (trono[2] == 'D')
    {
        lineaTrono.replace(34, 5, "Perro");
    }

    cout << lineaTrono << endl;
    cout << string(9, ' ') << "/  |           |           |  \\" << endl;
    cout << string(6, ' ') << "/     |           |           |     \\" << endl;
    cout << string(3, ' ') << "/        |           |           |        \\" << endl;

    for (int fila = 0; fila < TAM; fila++)
    { string linea((TAM-1)*(ANCHO_CELDA+1)+1,'-');
    
        for (int columna = 0; columna < TAM; columna++)
        {
            int posicion = columna * (ANCHO_CELDA+1);
          if(tablero[fila][columna] == 'P' || tablero[fila][columna] == 'D')
          {
            string ficha;

            if (tablero[fila][columna] == 'P')
            {
                ficha = "Puma";
            }
            else
            {
                ficha = "Perro";
            }
            int inicio = posicion - static_cast<int>(ficha.length()) / 2;

            if (inicio < 0)
            {
                inicio=0;
            }

            if (inicio + ficha.length() > linea.length())
            {
                inicio = linea.length() - ficha.length();
            }

            linea.replace(inicio, ficha.length(), ficha);
            
          }
          else
          {
            linea[posicion] = tablero[fila][columna];
          }
        }


        cout << linea << endl;


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

bool conectadas(int origen, int destino)
{
    const int POS_TABLERO = TAM * TAM;
    const int TOTAL_POS = POS_TABLERO + TAM_TRONO;

    if (origen < 0 || origen >= TOTAL_POS || destino < 0 || destino >= TOTAL_POS || origen == destino)
    {
        return false;
    }
    
    if (origen < POS_TABLERO && destino < POS_TABLERO)
    {
        int fila1 = origen / TAM;
        int columna1 = origen % TAM;

        int fila2 = destino / TAM;
        int columna2 = destino % TAM;
    
     if (fila1 == fila2 && (columna1 + 1 == columna2 || columna2 + 1 == columna1))
     {
         return true;
     }

     if (columna1 == columna2 && (fila1 + 1 == fila2 || fila2 + 1 == fila1))
     {
            return true;
     }

        return false;

    }

    if (origen > destino)
    {
        int auxiliar = origen;
        origen = destino;
        destino =auxiliar;
    }

    return (origen == 0 && destino == TRONO_IZQ) || (origen == 1 && destino == TRONO_IZQ) || (origen == 2 && destino == TRONO_SUP) || (origen == 3 || destino == TRONO_DER) || (origen == 4 && destino == TRONO_DER) || (origen == TRONO_SUP && destino == TRONO_IZQ) || (origen == TRONO_SUP && destino == TRONO_DER) || (origen == TRONO_IZQ && destino == TRONO_DER);
}

bool movValido(const char tablero[TAM][TAM], const char trono[TAM_TRONO], int origen, int destino)
{
    if (!conectadas(origen, destino))
    {
        return false;
    }

    char fichaOrigen;
    char fichaDestino;

    if (origen < TAM * TAM)
    {
        fichaOrigen = tablero[origen / TAM][origen % TAM];
    }
    else 
    {
        fichaOrigen = trono[origen- TAM*TAM];
    }

    if (destino< TAM*TAM)
    {
        fichaDestino= tablero[destino/ TAM][destino % TAM];
    }
    else
    {
        fichaDestino = trono[destino - TAM*TAM];
    }

    if (fichaOrigen != 'P' && fichaOrigen != 'D')
    {
        return false;
    }
    if (fichaDestino != '.')
    {
        return false;
    }

    return true;
}

bool movimiento(char tablero[TAM][TAM], char trono[TAM_TRONO], int origen, int destino)
{
    if(!movValido(tablero, trono, origen, destino))
    {
        return false;
    }

    char ficha;

    if (origen < TAM * TAM)
    {
        ficha = tablero[origen / TAM][origen % TAM];
    }
    else
    {
        ficha = trono[origen - TAM * TAM];
    }

    if (destino < TAM * TAM)
    {
        tablero[destino / TAM][destino % TAM] = ficha;
    }
    else
    {
        trono[destino - TAM*TAM] = ficha;
    }

    if (origen < TAM*TAM)
    {
        tablero[origen / TAM][origen % TAM] = '.';
    }
    else
    {
        trono[origen - TAM*TAM] = '.';
    }

    return true;

}

bool captura(char tablero[TAM][TAM], char trono[TAM_TRONO], int origen, int destino)
{
    const int Total = TAM * TAM + TAM_TRONO;
    if (origen < 0 || origen >= Total || destino < 0 || destino >= Total)
    {
        return false;
    }

    int medio = -1;

    if (origen < TAM * TAM && destino < TAM * TAM)
    {
        int fila1 = origen / TAM;
        int columna1 = origen % TAM;

        int fila2 = destino / TAM;
        int columna2 = destino % TAM;

        bool horizontal = fila1 == fila2 && (columna2 == columna1 + 2 || columna2 || columna1 -2);
        bool vertical = columna1 == columna2 && (fila2 == fila1 + 2 || fila2 == fila1 - 2);

         if (horizontal || vertical)
        {
         medio = (origen + destino) / 2;
        }
    }

    else
    {
        int menor= origen;
        int mayor = destino;

        if (menor > mayor)
        {
            int aux = menor;
            menor = mayor;
            mayor = aux;
        }

        if (menor == 0 && mayor == TRONO_SUP)
        {
            medio = TRONO_IZQ;
        }
        else if (menor == 4 && mayor == TRONO_SUP)
        {
            medio = TRONO_DER;
        }
        else if (menor == 6 && mayor == TRONO_IZQ)
        {
            medio = 1;
        }
        else if (menor == 7 && mayor == TRONO_SUP)
        {
            medio = 2;
        }
        else if (menor == 8 && mayor == TRONO_DER)
        {
            medio = 3;
        }
    }

    if (medio == -1)
    {
        return false;
    }

    if (!conectadas( origen, medio) || !conectadas(medio, destino))
    {
        return false;
    }

    int pos[3] = {origen, medio, destino};
    char fichas[3];

    for (int i=0; i<3; i++)
    {
        int posicion = pos[i];

        if (posicion < TAM * TAM)
        {
            fichas[i] = tablero[posicion / TAM][posicion % TAM];
        }
        else
        {
            fichas[i] = trono[posicion - TAM * TAM];
        }
    }

    if (fichas[0] != 'P' || fichas[1] != 'D' || fichas[2] != '.')
    {
        return false;
    }

    for (int i = 0; i < 3; i++)
    {
        int posicion = pos[i];
        char ficha = '.';

        if (i == 2)
        {
            ficha = 'P';
        }

        if (posicion< TAM * TAM)
        {
            tablero[posicion / TAM][posicion % TAM] = ficha;
        }
        else
        {
            trono[posicion - TAM*TAM] = ficha;
        }
    }

    return true;
}
