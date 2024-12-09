#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "player.h"
#include <sys/select.h>

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
 * @return void
 * @note Chiude tutti i socket aperti
 */
void cleanup_server(ServerState* state);

/**
 * Gestisce una nuova connessione in arrivo
 * @param state ServerState* struttura del server
 * @return void
 */
void handle_new_connection(ServerState* state);

/**
 * Gestisce un messaggio ricevuto da un client
 * @param state ServerState* struttura del server
 * @param client_socket int socket del client
 * @return void
 */
void process_client_message(ServerState* state, int client_socket);

/**
 * Invia un messaggio a tutti i client connessi
 * @param state ServerState* struttura del server
 * @param msg Message* messaggio da inviare
 * @return void
 * @note Usata per aggiornamenti di stato o punteggi
 */
void broadcast_message(ServerState* state, Message* msg);

/**
 * Gestisce la disconnessione di un client
 * @param state ServerState* struttura del server
 * @param client_socket int socket del client
 * @return void
 * @note Rimuove il client dai descrittori attivi
 */
void handle_disconnect(ServerState* state, int client_socket);

/**
 * Invia una domanda ad un client
 * @param client_socket int socket del client
 * @param quiz Quiz* quiz corrente
 * @param question_num int numero della domanda
 * @return void
 */
void send_question_to_client(int client_socket, Quiz* quiz, int question_num);

/**
 * Formatta i punteggi di tutti i giocatori in una stringa
 * @param state struttura ServerState contenente l'array dei giocatori
 * @return stringa formattata contenente tutti i punteggi
 */
char* format_scores(ServerState* state);

#endif