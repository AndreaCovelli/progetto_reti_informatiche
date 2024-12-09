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
 * Crea un array di giocatori vuoto
 * @param capacity capacità iniziale dell'array
 * @return PlayerArray*
 */
PlayerArray* create_player_array(int capacity);

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
 * Ottieni il numero di giocatori attivi
 * @param array PlayerArray* array di giocatori
 * @return numero di giocatori attivi
 */
/*
static int get_active_players_count(PlayerArray* array);
*/

/**
 * Ritorna true se il giocatore ha completato il quiz richiesto
 * @param player Player* giocatore
 * @param nick_name const char* nickname del giocatore
 * @param sport_quiz true se si vuole verificare se il giocatore
 * ha completato il quiz sullo sport, false per la geografia
 * @return true se il giocatore ha completato il quiz richiesto, false altrimenti
 */
bool has_completed_quiz(PlayerArray* player, const char* nick_name, bool sport_quiz);

/**
 * Segna il quiz richiesto come completato per il giocatore
 * @param array PlayerArray* array di giocatori
 * @param nickname const char* nickname del giocatore
 * @param sport_quiz true se si vuole segnare il quiz sullo sport come completato,
 * false per la geografia
 * @return void
 */
void mark_quiz_as_completed(PlayerArray* array, const char* nickname, bool sport_quiz);

#endif