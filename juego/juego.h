#ifndef JUEGO_H
#define JUEGO_H
const int TAM = 5;
const int TAM_TRONO = 3;

const int TRONO_SUP = TAM * TAM;
const int TRONO_IZQ = TRONO_SUP + 1;
const int TRONO_DER = TRONO_SUP + 2;

void inicializarTablero(char tablero[TAM][TAM]);
void inicializarTrono(char trono[TAM_TRONO]);
void mostrarTablero(const char tablero[TAM][TAM], const char trono[TAM_TRONO]);
bool conectadas(int origen, int destino);
bool movValido(const char tablero[TAM][TAM], const char trono[TAM_TRONO], int origen, int destino);
bool movimiento(char tablero[TAM][TAM],  char trono[TAM_TRONO], int origen, int destino);

#endif