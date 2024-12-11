// Implementazione delle funzioni comuni al server e al client
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
    
    // Prima invia l'header del messaggio
    ssize_t header_size = sizeof(msg->type) + sizeof(msg->length);
    ssize_t sent = send(sock, msg, header_size, 0);
    if (sent != header_size) {
        return ERR_SEND;
    }
    
    // Poi invia il payload se presente
    if (msg->length > 0) {
        sent = send(sock, msg->payload, msg->length, 0);
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
    
    return sent + header_size;
}

ssize_t receive_message(int sock, Message* msg) {
    if (!msg) return ERR_RECV;
    
    // First receive the message header
    ssize_t header_size = sizeof(msg->type) + sizeof(msg->length);
    ssize_t received = recv(sock, msg, header_size, 0);
    if (received != header_size) {
        return ERR_RECV;
    }
    
    // Then receive the payload if there is one
    if (msg->length > 0) {
        if (msg->length > MAX_MSG_LEN) {
            return ERR_RECV;
        }
        
        received = recv(sock, msg->payload, msg->length, 0);
        if (received != msg->length) {
            return ERR_RECV;
        }
        
        // Ensure null termination
        msg->payload[msg->length] = '\0';
    }
    printf("Received message of type %s from device %d\n", message_type_to_string(msg->type), sock);
    
    return received + header_size;
}

const char* message_type_to_string(MessageType type) {
    switch(type) {
        case MSG_LOGIN: return "MSG_LOGIN";
        case MSG_LOGIN_SUCCESS: return "MSG_LOGIN_SUCCESS";
        case MSG_LOGIN_ERROR: return "MSG_LOGIN_ERROR";
        case MSG_REQUEST_QUESTION: return "MSG_REQUEST_QUESTION";
        case MSG_QUESTION: return "MSG_QUESTION";
        case MSG_ANSWER: return "MSG_ANSWER";
        case MSG_ANSWER_RESULT: return "MSG_ANSWER_RESULT";
        case MSG_SCORE: return "MSG_SCORE";
        case MSG_QUIZ_COMPLETED: return "MSG_QUIZ_COMPLETED";
        case MSG_QUIZ_AVAILABLE: return "MSG_QUIZ_AVAILABLE";
        case MSG_TRIVIA_COMPLETED: return "MSG_TRIVIA_COMPLETED";
        case MSG_END_QUIZ: return "MSG_END_QUIZ";
        case MSG_DISCONNECT: return "MSG_DISCONNECT";
        case MSG_ERROR: return "MSG_ERROR";
        case MSG_REQUEST_NICKNAME: return "MSG_REQUEST_NICKNAME";
        case MSG_NICKNAME_PROMPT: return "MSG_NICKNAME_PROMPT";
        case MSG_REQUEST_SCORE: return "MSG_REQUEST_SCORE";
        default: return "UNKNOWN";
    }
}