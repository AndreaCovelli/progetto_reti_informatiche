#ifndef SCORE_H
#define SCORE_H

#include "server.h"

/**
 * Formatta i punteggi di tutti i giocatori in una stringa
 * @param state struttura ServerState contenente i giocatori
 * @return puntatore a stringa formattata con i punteggi
 */
char* format_scores(ServerState* state);

#endif