#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include <stdbool.h>

/**
 * Struttura che rappresenta lo stato del client
 * @param socket Socket di connessione al server
 * @param nickname Nickname del giocatore
 * @param current_quiz Quiz attualmente selezionato (1 per sport, 2 per geografia)
 * @param current_question Numero della domanda corrente
 */
typedef struct {
    int socket;
    char nickname[MAX_NICK_LENGTH];
    int current_quiz;
    int current_question;
} ClientState;

// Funzioni di inizializzazione e gestione del client
/**
 * Inizializza la struttura ClientState
 * @return ClientState* struttura inizializzata o NULL in caso di errore
 */
ClientState* init_client();

/**
 * Libera le risorse allocate per il client
 * @param state struttura ClientState da liberare
 */
void cleanup_client(ClientState* state);

/**
 * Connette il client al server
 * @param state struttura ClientState
 * @param port porta del server
 * @return true se la connessione ha successo, false altrimenti
 */
bool connect_to_server(ClientState* state, int port);

/**
 * Disconnette il client dal server
 * @param state struttura ClientState
*/
void disconnect_from_server(ClientState* state);

// Funzioni di gestione del menu e dell'interfaccia
/**
 * Mostra il menu principale
 * @return scelta dell'utente
 */
int show_main_menu();

/**
 * Mostra il menu di selezione del quiz
 * @return scelta dell'utente
 */
int show_quiz_selection();

/**
 * Gestisce l'input del nickname
 * @param state struttura ClientState
 * @return true se il nickname è valido e accettato dal server
 */
bool validate_and_send_nickname(ClientState* state);

// Funzioni di gestione del gioco
/**
 * Gestisce una sessione di gioco
 * @param state struttura ClientState
 * @return true se la sessione termina normalmente
 */
bool play_game_session(ClientState* state);

/**
 * Gestisce una singola domanda
 * @param state struttura ClientState
 * @param question testo della domanda
 * @return true se la risposta è stata inviata con successo
 */
bool answer_question(ClientState* state, const char* question);

#endif