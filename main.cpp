#include <iostream>
#include <string>
#include <sstream>

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

    return 0;

}