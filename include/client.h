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
 * @note Invia un messaggio di disconnessione al server e chiude il socket
 */
void disconnect_from_server(ClientState* state);


// Funzioni di gestione del menu e dell'interfaccia

/**
 * Stampa il banner del gioco
 * @note Mostra il titolo del gioco e una linea decorativa
 */
void print_banner();

/**
 * Stampa le opzioni del menu principale
 * @note Mostra le opzioni disponibili per l'utente (iniziare una partita o uscire)
 */
void print_menu_options();

/**
 * Ottiene la scelta dell'utente dal menu principale
 * @return int numero dell'opzione selezionata dall'utente
 * @note Gestisce l'input dell'utente e lo converte in un intero
 */
int get_menu_choice();

/**
 * Mostra il menu principale
 * @return int, scelta dell'utente
 */
int show_main_menu();

/**
 * Gestisce la selezione del quiz da parte dell'utente
 * @param state struttura ClientState
 * @return true se la selezione è valida, false altrimenti
 * @note Permette all'utente di scegliere tra quiz sport (1) o geografia (2)
 */
bool handle_quiz_selection(ClientState* state);

/**
 * Gestisce l'input del nickname, l'invio, e la validazione
 * da parte del server
 * @param state struttura ClientState
 * @return true se il nickname è valido e accettato dal server
 */
bool validate_and_send_nickname(ClientState* state);


/* Funzioni di gestione delle risposte */

/**
 * Gestisce la risposta del server alla quiz-risposta inviata dal client
 * @param msg puntatore al messaggio ricevuto dal server
 * @return true se la quiz-risposta è stata gestita correttamente, false altrimenti
 */
bool handle_answer_result(Message* msg);

/**
 * Gestisce i comandi speciali durante il gioco
 * @param state struttura ClientState
 * @param answer comando inserito dall'utente
 * @return true se il comando è stato gestito, false se non è un comando speciale
 * @note Gestisce comandi come 'show score' e 'endquiz'
 */
bool handle_special_commands(ClientState* state, const char* answer);

/**
 * Invia una risposta al server
 * @param state struttura ClientState
 * @param answer risposta da inviare
 * @return true se l'invio ha avuto successo, false altrimenti
 * @note Prepara e invia un messaggio di tipo MSG_ANSWER
 */
bool send_answer(ClientState* state, const char* answer);

/**
 * Gestisce la fase in cui il client risponde alle domande
 * @param state struttura ClientState
 * @param question testo della domanda
 * @return true se la risposta è stata inviata con successo
 * @note Se la handle_special_commands() restituisce true, non invia la risposta. 
 * @note La funzione chiamante di answer_question() avrà come ritorno true per indicare
 * che il comando speciale è stato gestito.
 */
bool answer_question(ClientState* state, const char* question);


// Funzioni di gestione del gioco

/**
 * Gestisce i messaggi ricevuti durante la partita
 * @param state struttura ClientState
 * @param msg puntatore al messaggio ricevuto dal server
 * @param current_question puntatore alla stringa contenente la domanda corrente
 * @return true se il messaggio è stato gestito correttamente, false altrimenti
 * @note Elabora vari tipi di messaggi come domande, punteggi e risultati
 */
bool handle_game_message(ClientState* state, Message* msg, char* current_question);

/**
 * Gestisce una sessione di gioco attiva
 * @param state struttura ClientState
 * @return true se la sessione termina normalmente, false in caso di errori
 * @note Gestisce il ciclo principale di gioco, inclusa la ricezione delle domande
 */
bool play_game_session(ClientState* state);

/**
 * Gestisce una sessione di gioco completa
 * @param state struttura ClientState
 * @return true se la sessione termina normalmente, false in caso di errori
 * @note Gestisce l'autenticazione e la partita, inclusa la disconnessione in caso di errori
 */
bool handle_game_session(ClientState* state);

#endif