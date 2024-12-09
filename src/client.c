/*
    Implementazione del client del Trivia Game
*/

#include "include/client.h"
#include "include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

ClientState* init_client() {
    ClientState* state = malloc(sizeof(ClientState));
    if (!state) return NULL;
    
    state->socket = -1;
    state->nickname[0] = '\0';
    state->current_quiz = 0;
    state->current_question = 0;
    
    return state;
}

void cleanup_client(ClientState* state) {
    if (state) {
        if (state->socket != -1) {
            close(state->socket);
        }
        free(state);
    }
}

bool connect_to_server(ClientState* state, int port) {
    printf("DEBUG: Connecting to server on port %d\n", port);
    
    int sock = setup_connection("127.0.0.1", port);
    if (sock < 0) {
        fprintf(stderr, "Failed to connect to server\n");
        return false;
    }
    
    state->socket = sock;
    printf("DEBUG: Connected with socket %d\n", state->socket);
    return true;
}

void disconnect_from_server(ClientState* state) {
    if (state->socket != -1) {
        Message msg;
        msg.type = MSG_DISCONNECT;
        msg.length = 0;
        
        if (send_message(state->socket, &msg) < 0) {
            fprintf(stderr, "Errore nell'invio del messaggio di disconnessione\n");
        }
        
        // Chiude il socket
        if (close(state->socket) < 0) {
            fprintf(stderr, "Errore nella chiusura del socket\n");
        }
        // Imposta il socket a -1 per indicare che non è più connesso
        state->socket = -1;
    }
}

int show_main_menu() {
    char input[10];
    
    printf("\nTrivia Quiz\n");
    printf("++++++++++++++++++++++++++++++++++++\n");
    printf("Menù:\n");
    printf("1 - Comincia una sessione di Trivia\n");
    printf("2 - Esci\n");
    printf("++++++++++++++++++++++++++++++++++++\n");
    printf("La tua scelta: ");
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 2;  // Exit in caso di errore
    }
    
    // Rimuove il newline e converte in intero
    input[strcspn(input, "\n")] = 0;
    return atoi(input);
}

int show_quiz_selection() {
    char input[10];
    
    printf("\nQuiz disponibili\n");
    printf("++++++++++++++++++++++++++++++\n");
    printf("1 - Sport\n");
    printf("2 - Geografia\n");
    printf("++++++++++++++++++++++++++++++\n");
    printf("La tua scelta: ");
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }
    
    input[strcspn(input, "\n")] = 0;
    return atoi(input);
}

bool validate_and_send_nickname(ClientState* state) {
    Message msg;
    printf("\nTrivia Quiz\n");
    printf("+++++++++++++++++++++++++++++++++++++++++\n");
    printf("Scegli un nickname (deve essere univoco): ");
    
    fgets(state->nickname, MAX_NICK_LENGTH, stdin);
    state->nickname[strcspn(state->nickname, "\n")] = 0; // Rimuove newline

    msg.type = MSG_LOGIN;
    msg.length = strlen(state->nickname);
    strncpy(msg.payload, state->nickname, MAX_MSG_LEN);

    // Invia il nickname al server
    if (send_message(state->socket, &msg) < 0) {
        return false;
    }

    // Attendi risposta dal server
    if (receive_message(state->socket, &msg) < 0) {
        return false;
    }

    // Se il server risponde con errore, il nickname è già preso
    if (msg.type == MSG_ERROR) {
        printf("%s\n", msg.payload);
        return false;
    }

    return true;
}

bool answer_question(ClientState* state, const char* question) {
    char answer[MAX_ANSWER_LENGTH];
    Message msg;

    printf("\nDomanda: %s\n", question);
    printf("Risposta (o 'show score' per vedere i punteggi, 'endquiz' per uscire): ");
    
    fgets(answer, MAX_ANSWER_LENGTH, stdin);
    answer[strcspn(answer, "\n")] = 0; // Rimuove newline

    // Gestisce comandi speciali
    if (strcmp(answer, "show score") == 0) {
        msg.type = MSG_SCORE;
        //msg.type = MSG_ANSWER;
        msg.length = strlen("show score");  // Set proper length
        strncpy(msg.payload, "show score", MAX_MSG_LEN);  // Include the command in payload
        
        if (send_message(state->socket, &msg) < 0) {
            return false;
        }
        
        // Wait for and process server's response
        if (receive_message(state->socket, &msg) < 0) {
            return false;
        }
        
        // Display the scores
        printf("\nPunteggi:\n%s\n", msg.payload);
        return true;  // Continue the game
        
    } else if (strcmp(answer, "endquiz") == 0) {
        msg.type = MSG_DISCONNECT;
        msg.length = strlen("endquiz");
        strncpy(msg.payload, "endquiz", MAX_MSG_LEN);
        
        if (send_message(state->socket, &msg) < 0) {
            return false;
        }

        // Chiudiamo il socket corrente
        close(state->socket);
        state->socket = -1;
        return false;  // Termina la sessione di gioco
    } else {
        msg.type = MSG_ANSWER;
        msg.length = strlen(answer);
        strncpy(msg.payload, answer, MAX_MSG_LEN);
    }

    if (send_message(state->socket, &msg) < 0) {
        return false;
    }

    return true;
}

bool play_game_session(ClientState* state) {
    Message msg;
    
    state->current_quiz = show_quiz_selection();
    if (state->current_quiz < 1 || state->current_quiz > 2) {
        return false;
    }

    // Invia scelta del quiz al server
    msg.type = MSG_QUESTION;
    msg.length = 1;
    msg.payload[0] = state->current_quiz + '0';
    
    if (send_message(state->socket, &msg) < 0) {
        return false;
    }

    state->current_question = 0;

    while (state->current_question < MAX_QUESTIONS) {
        if (receive_message(state->socket, &msg) < 0) {
            return false;
        }

        switch (msg.type) {
            case MSG_QUESTION:
                if (!answer_question(state, msg.payload)) {
                    return false;
                }
                state->current_question++;
                break;
            case MSG_SCORE:
                printf("\nPunteggi:\n%s\n", msg.payload);
                break;
            case MSG_ERROR:
                printf("\nErrore: %s\n", msg.payload);
                return false;
            case MSG_DISCONNECT:
                printf("\nIl server si è disconnesso. Il quiz è terminato.\n");
                return false;
            default:
                printf("\n%s\n", msg.payload);
                break;
        }
    }

    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <port>\n", argv[0]);
        return 1;
    }

    ClientState* state = init_client();
    if (!state) {
        fprintf(stderr, "Errore nell'inizializzazione del client\n");
        return 1;
    }

    if (!connect_to_server(state, atoi(argv[1]))) {
        cleanup_client(state);
        return 1;
    }

    int choice;
    do {
    choice = show_main_menu();
    switch (choice) {
        case 1:
            // Se il socket è chiuso, riconnettiti
            if (state->socket == -1) {
                if (!connect_to_server(state, atoi(argv[1]))) {
                    printf("Errore di connessione al server\n");
                    break;
                }
            }
            if (validate_and_send_nickname(state)) {
                play_game_session(state);
            }
            break;
        case 2:
            disconnect_from_server(state);
            printf("Grazie per aver giocato!\n");
            break;
        default:
            printf("Scelta non valida\n");
            break;
        }
    } while (choice != 2);

    cleanup_client(state);
    return 0;
}