#ifndef PLAYER_H
#define PLAYER_H
#include "constants.h"
#include <stdbool.h>

/**
 * Dati di un giocatore
 * @param nickname Nickname del giocatore
 * @param sport_score Punteggio del giocatore nel quiz sullo sport
 * @param geography_score Punteggio del giocatore nel quiz sulla geografia
 * @param completed_sport true se il giocatore ha completato il quiz sullo sport
 * @param completed_geography true se il giocatore ha completato il quiz sulla geografia
 */
typedef struct {
    char nickname[MAX_NICK_LENGTH]; // Nickname del giocatore
    int sport_score; 
    int geography_score;
    bool completed_sport;
    bool completed_geography;
} Player;

/**
 * Array dinamico di giocatori
 * @param players Array di giocatori
 * @param count Numero di giocatori attualmente presenti nell'array
 * @param capacity Capacità massima dell'array
 */
typedef struct {
    Player* players;
    int count;
    int capacity;
} PlayerArray;

/**
 * Crea un array di giocatori 
 * @return PlayerArray*
 */
PlayerArray* create_player_array(int initial_capacity);

/**
 * Libera la memoria allocata per l'array
 * @param array PlayerArray* da liberare
 * @return void
 */
void free_player_array(PlayerArray* array);

/**
 * Aggiunge un giocatore all'array
 * @param array PlayerArray* in cui aggiungere il giocatore
 * @param nickname const char* nickname del giocatore
 * @return true se il giocatore è stato aggiunto, false altrimenti
 */
bool add_player(PlayerArray* array, const char* nickname);

/**
 * Rimuove un giocatore dall'array
 * @param array PlayerArray* da cui rimuovere il giocatore
 * @param nickname const char* nickname
 * @return true se il giocatore è stato rimosso, false altrimenti
 */
bool remove_player(PlayerArray* array, const char* nickname);

/**
 * Trova un giocatore nell'array
 * @param array PlayerArray* in cui cercare il giocatore
 * @param nickname const char* nickname del giocatore da cercare
 * @return Player* al giocatore se trovato, NULL altrimenti 
 */
Player* find_player(PlayerArray* array, const char* nickname);

/**
 * Ordina i giocatori in base al punteggio
 * @param array PlayerArray* array di giocatori
 * @param sport_quiz true se si vuole ordinare i giocatori 
 * per il punteggio del quiz sullo sport, false per il quiz sulla geografia
 * @return void
 */
void sort_players_by_score(PlayerArray* array, bool sport_quiz);

/**
 * Confronta due giocatori in base al punteggio
 * @param a const void* puntatore al primo giocatore
 * @param b const void* puntatore al secondo giocatore
 * @param sport_quiz true se si vuole ordinare i giocatori 
 * per il punteggio del quiz sullo sport, false per il quiz sulla geografia
 * @return int risultato del confronto per qsort
 * (valore negativo se a < b, 0 se a == b, valore positivo se a > b)
 */
static int compare_players_by_score(const void* a, const void* b, bool sport_quiz);

#endif