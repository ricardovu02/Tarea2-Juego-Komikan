#ifndef ARBITRO_H
#define ARBITRO_H

#include "../partida/partida.h"

void* threadArbitro(void* dato);
void revisarFin(Partida &p);
void registrarMov(const Partida &p, const char *ficha, int numero, int origen, int destino);

#endif