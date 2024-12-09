// Strutture e funzioni di rete usate sia dal server che dai client
#ifndef COMMON_H
#define COMMON_H

// Include necessari sia per il server che per i client
#include "constants.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/**
 * Struttura per rappresentare un messaggio
 * @param type tipo del messaggio
 * @param length lunghezza del payload
 * @param payload contenuto del messaggio
 * @see constants.h per i tipi di messaggio
 * @note COMMAND|LENGTH|PAYLOAD dove
 * COMMAND e LENGHT fanno parte dell'header
 */
typedef struct {
    int type;
    int length;
    char payload[MAX_MSG_LEN];
} Message;

// Funzioni di utilità rete

/**
 * Crea un nuovo socket TCP
 * @return file descriptor o -1 in caso di errore
 */
int create_socket();

/**
 * Configura una connessione socket
 * @param ip indirizzo ip
 * @param port porta
 * @return file descriptor del socket o -1 in caso di errore
 */
int setup_connection(const char* ip, int port);

/**
 * Invia un messaggio al socket
 * @param sock file descriptor del socket
 * @param msg messaggio da inviare
 * @return numero di byte inviati o ERR_SEND in caso di errore
 */
ssize_t send_message(int sock, Message* msg);

/**
 * Riceve un messaggio dal socket
 * @param sock file descriptor del socket
 * @param msg puntatore al messaggio da ricevere
 * @return numero di byte ricevuti o -1 in caso di errore
 */
ssize_t receive_message(int sock, Message* msg);

#endif