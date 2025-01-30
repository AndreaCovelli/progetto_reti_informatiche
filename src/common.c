/*
 * common.c
 * Implementazione delle funzionalità comuni per 'Trivia Quiz Multiplayer'
 * 
 * Questo file contiene l'implementazione delle funzioni utilizzate sia dal client
 * che dal server, inclusa la gestione dei socket, l'invio e la ricezione dei
 * messaggi, e la conversione dei tipi di messaggio. Fornisce le funzionalità
 * di base per la comunicazione di rete e la gestione dei protocolli condivisi
 * tra client e server.
 */

#include "include/common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int create_socket() {
    // SOCK_STREAM --> TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Errore nella creazione del socket");
        return ERR_SOCKET;
    }
    
    // Imposta le opzioni del socket per permettere
    // il riutilizzo degli indirizzi e delle porte
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Errore nell'impostazione delle opzioni del socket");
        close(sock);
        return ERR_SOCKET;
    }

    return sock;
}

int setup_connection(const char* ip, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("Indirizzo IP non valido");
        return ERR_CONNECT;
    }
    
    int sock = create_socket();
    if (sock < 0) return sock;
    
    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Errore nella connessione");
        close(sock);
        return ERR_CONNECT;
    }
    
    return sock;
}

ssize_t send_message(int sock, Message* msg) {
    if (!msg) return ERR_SEND;
    
    // Creiamo una struttura temporanea per la conversione

    // La conversione host-to-network non serve per MessageType
    // perché è un discriminatore confrontato come intero
    // non un valore usato in calcoli numerici come length
    struct {
        MessageType type;
        uint32_t length;
    } network_header;
    
    network_header.type = msg->type;           // Il type rimane invariato
    network_header.length = htonl(msg->length); // Solo length viene convertito
    
    ssize_t header_size = sizeof(network_header);
    ssize_t sent = send(sock, &network_header, header_size, MSG_NOSIGNAL);
    if (sent != header_size) {
        return ERR_SEND;
    }
    
    // Poi invia il payload sse è presente
    if (msg->length > 0) {
        sent = send(sock, msg->payload, msg->length, MSG_NOSIGNAL);
        if (sent != msg->length) {
            return ERR_SEND;
        }
    }

    /*
        Si divide l'invio del messaggio in due parti con
        l'header che viene inviato prima del payload perchè
        contiene metadati importanti che influenzano
        come il payload deve essere processato
     */
    
    DEBUG_PRINT("Inviato messaggio di tipo %s, lunghezza %d, payload (primi 10 caratteri): %.20s\n", 
           message_type_to_string(msg->type), msg->length, msg->payload);

    return sent + header_size;
}

ssize_t receive_message(int sock, Message* msg) {
    if (!msg) return ERR_RECV;
    
    struct {
        MessageType type;
        uint32_t length;
    } network_header;
    
    ssize_t header_size = sizeof(network_header);
    ssize_t received = recv(sock, &network_header, header_size, 0);
    if (received <= 0) {
        return ERR_RECV;
    }
    
    msg->type = network_header.type;
    msg->length = ntohl(network_header.length);  // Convertiamo solo length
    
    // Poi ricevo il payload sse presente
    if (msg->length > 0) {
        if (msg->length > MAX_MSG_LEN) {
            return ERR_RECV;
        }
        
        received = recv(sock, msg->payload, msg->length, 0);
        if (received <= 0) {  // Cambiato da != a <= per individuare la disconnessione
            printf("Server disconnesso\n");
            return ERR_RECV;
        }
        
        // Ensure null termination
        msg->payload[msg->length] = '\0';
    }

    DEBUG_PRINT("Ricevuto messaggio di tipo %s, lunghezza %d, payload (primi 10 caratteri): %.20s\n", 
           message_type_to_string(msg->type), msg->length, msg->payload);
    
    return received + header_size;
}

const char* message_type_to_string(MessageType type) {
    switch(type) {
        case MSG_LOGIN: return "MSG_LOGIN";
        case MSG_LOGIN_SUCCESS: return "MSG_LOGIN_SUCCESS";
        case MSG_LOGIN_ERROR: return "MSG_LOGIN_ERROR";
        case MSG_REQUEST_NICKNAME: return "MSG_REQUEST_NICKNAME";
        case MSG_NICKNAME_PROMPT: return "MSG_NICKNAME_PROMPT";
        case MSG_REQUEST_QUESTION: return "MSG_REQUEST_QUESTION";
        case MSG_QUESTION: return "MSG_QUESTION";
        case MSG_ANSWER: return "MSG_ANSWER";
        case MSG_ANSWER_RESULT: return "MSG_ANSWER_RESULT";
        case MSG_REQUEST_SCORE: return "MSG_REQUEST_SCORE";
        case MSG_SCORE: return "MSG_SCORE";
        case MSG_QUIZ_COMPLETED: return "MSG_QUIZ_COMPLETED";
        case MSG_QUIZ_AVAILABLE: return "MSG_QUIZ_AVAILABLE";
        case MSG_TRIVIA_COMPLETED: return "MSG_TRIVIA_COMPLETED";
        case MSG_END_QUIZ: return "MSG_END_QUIZ";
        case MSG_DISCONNECT: return "MSG_DISCONNECT";
        case MSG_ERROR: return "MSG_ERROR";
        default: return "UNKNOWN"; // Messaggio sconosciuto
    }
}