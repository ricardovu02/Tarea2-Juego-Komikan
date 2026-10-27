#include <iostream>
#include <string>
#include <sstream>
#include "juego/juego.h"

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

    if (argc == 4)
    {
        cout << "Archivo de configuracion: " << argv[3] << endl;
    }

    char tablero[TAM][TAM];
    char trono[TAM_TRONO];

    inicializarTablero(tablero);
    inicializarTrono(trono);

    trono[0] = 'P';
    tablero[2][0] = 'D';
    tablero[2][4] = 'D';

    for(int columna = 0; columna< TAM; columna++)
    {
        tablero[3][columna] = 'D';
        tablero[4][columna] = 'D';
    }

    

    cout<<endl;
    cout<< "TABLERO INICIAL: " << endl;
    mostrarTablero(tablero, trono);
    


    return 0;

}