#ifndef JUEGO_H
#define JUEGO_H
const int TAM = 5;
const int TAM_TRONO = 3;

void inicializarTablero(char tablero[TAM][TAM]);
void inicializarTrono(char trono[TAM_TRONO]);
void mostrarTablero(const char tablero[TAM][TAM], const char trono[TAM_TRONO]);

#endif