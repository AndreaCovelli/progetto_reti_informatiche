#ifndef SERVER_H
#define SERVER_H

#include <sys/select.h>
#include "common.h"
#include "player.h"
#include "quiz.h"
#include "debug.h"

/**
 * Struttura per mantenere lo stato del server
 * @param server_socket Socket del server
 * @param active_fds Set di descrittori attivi
 * @param read_fds Set di descrittori in lettura
 * @param max_fd Massimo descrittore attivo
 * @param players Array di giocatori
 */
typedef struct {
    int server_socket;
    fd_set active_fds;
    fd_set read_fds;
    int max_fd;
    PlayerArray* players;
} ServerState;

/**
 * Struttura per mantenere lo stato del client
 * @param nickname Nickname del giocatore
 * @param current_quiz Quiz attualmente selezionato (1 per sport, 2 per geografia)
 * @param current_question Numero della domanda corrente
 * @param is_playing Indica se il client è attualmente in partita
 */
typedef struct {
    char nickname[MAX_NICK_LENGTH];
    int current_quiz;
    int current_question;
    bool is_playing;
} ClientData;

// Funzioni server

/**
 * Inizializza il server e le sue strutture dati
 * @param ip indirizzo ip del server    
 * @param port porta del server
 * @note Initializes file descriptor sets for select().
 * Inizializza read, write e error set
 * @return Restituisce la struttura ServerState inizializzata
 */
ServerState* init_server(const char* ip, int port);

/**
 * Pulisce le risorse allocate dal server
 * @param state ServerState* struttura del server

 * @note Chiude tutti i socket aperti
 */
void cleanup_server(ServerState* state);

/**
 * Gestisce una nuova connessione in arrivo
 * @param state ServerState* struttura del server
 */
void handle_new_connection(ServerState* state);

/**
 * Gestisce un messaggio ricevuto da un client
 * @param state ServerState* struttura del server
 * @param client_socket socket del client
 */
void process_client_message(ServerState* state, int client_socket);

/**
 * Invia un messaggio a tutti i client connessi
 * @param state ServerState* struttura del server
 * @param msg Message* messaggio da inviare
 * @note Usata per aggiornamenti di stato del server
 */
void broadcast_message(ServerState* state, Message* msg);

/**
 * Gestisce la disconnessione di un client
 * @param state ServerState* struttura del server
 * @param client_socket socket del client
 * @note Rimuove il client dai descrittori attivi
 */
void handle_disconnect(ServerState* state, int client_socket);

/**
 * Gestisce la chiusura del server
 * @note Invia un messaggio di disconnessione a tutti i client
 */
void handle_shutdown();

/**
 * Mostra lo stato del server
 * @param state ServerState* struttura del server
 */
void display_server_status(ServerState* state);

/**
 * Invia un prompt per il nickname al client
 * @param client_socket socket del client
 */
void send_nickname_prompt(int client_socket);

/**
 * Invia il messaggio con i quiz disponibili al client
 * @param client_socket socket del client
 */
void send_quiz_options(int client_socket);

/**
 * Invia una domanda ad un client
 * @param client_socket socket del client
 * @param quiz Quiz* quiz corrente
 * @param question_num numero della domanda
 */
void send_question_to_client(int client_socket, Quiz* quiz, int question_num);

/**
 * Formatta i punteggi di tutti i giocatori in una stringa
 * @param state struttura ServerState contenente l'array dei giocatori
 * @return stringa formattata contenente tutti i punteggi
 */
char* format_scores(ServerState* state);

#endif