#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include "player.h"
#include <sys/select.h>

typedef struct {
    int socket;
    struct sockaddr_in addr;
} WaitingClient;

typedef struct {
    int server_socket;
    fd_set active_fds;
    fd_set read_fds;
    int max_fd;
    PlayerArray* players;
    WaitingClient waiting_queue[MAX_WAITING_CLIENTS];
    int waiting_count;
} ServerState;

// Funzioni server

/**
 * Inizializza il server e le sue strutture dati
 * @param ip indirizzo ip del server    
 * @param port porta del server
 * @note Initializes file descriptor sets for select().
 * Sets up read, write and error file descriptor sets for monitoring
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
void handle_client_message(ServerState* state, int client_socket);

/**
 * Gestisce le connessioni attive
 * @param state ServerState* struttura del server
 * @return void
 * @note Rimuove eventuali client inattivi
 */
void manage_connections(ServerState* state);

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

// Funzioni quiz
/**
 * Invia una domanda ad un client
 */
void send_question(int client_socket, int question_num);

#endif



