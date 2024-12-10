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
    int choice;

    printf("\nQuiz disponibili\n");
    printf("++++++++++++++++++++++++++++++\n");
    printf("1 - Sport\n");
    printf("2 - Geografia\n");
    printf("++++++++++++++++++++++++++++++\n");

    do {
        printf("La tua scelta: ");
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }
        
        // Rimuove il newline
        input[strcspn(input, "\n")] = 0;
        
        // Verifica se l'input è numerico
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Per favore, inserisci 1 per Sport o 2 per Geografia.\n");
            continue;
        }
        
        // Verifica se il numero è valido (1 o 2)
        if (choice != 1 && choice != 2) {
            printf("Scelta non valida. Inserisci 1 per Sport o 2 per Geografia.\n");
            continue;
        }
        
        return choice;
        
    } while (1);
}

bool validate_and_send_nickname(ClientState* state) {
    Message msg;
    bool nickname_valid = false;

    while (!nickname_valid) {
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
        printf("DEBUG: Received message type: %s, length: %d, payload: %s\n",
               message_type_to_string(msg.type), msg.length, msg.payload);
        // Se il server risponde con errore, il nickname è già preso
        if (msg.type == MSG_ERROR_LOGIN) {
            printf("%s\n", msg.payload);
            continue; // Chiede di nuovo il nickname
        }

        if(msg.type == MSG_ERROR) {
            printf("Errore nel server");
            return false;
        }

        nickname_valid = true;
    }

    return true;
}

bool answer_question(ClientState* state, const char* question) {
    char answer[MAX_ANSWER_LENGTH];
    Message msg = {0};  // Inizializza a zero tutti i campi

    // Mostra la domanda e richiedi la risposta
    printf("\nDomanda: %s\n", question);
    printf("Risposta (o 'show score' per vedere i punteggi, 'endquiz' per uscire): ");
    
    // Leggi l'input dell'utente
    if (!fgets(answer, MAX_ANSWER_LENGTH, stdin)) {
        return false;
    }
    answer[strcspn(answer, "\n")] = 0; // Rimuove newline

    // Prepara il messaggio base
    if (strcmp(answer, "show score") == 0) {
        msg.type = MSG_SCORE;
    } else if (strcmp(answer, "endquiz") == 0) {
        msg.type = MSG_QUIZ_COMPLETED;
    } else {
        msg.type = MSG_ANSWER;
    }

    // Gestisci la risposta in base al tipo
    switch (msg.type) {
        case MSG_SCORE:
            msg.length = strlen(answer);
            strncpy(msg.payload, answer, MAX_MSG_LEN);
            return send_message(state->socket, &msg) >= 0;

        case MSG_QUIZ_COMPLETED:
            msg.length = strlen(answer);
            strncpy(msg.payload, answer, MAX_MSG_LEN);
            if (send_message(state->socket, &msg) < 0) {
                return false;
            }
            state->current_quiz = 0;  // Reset quiz state
            return false;  // Segnala di terminare la sessione

        case MSG_ANSWER:
            msg.length = strlen(answer);
            strncpy(msg.payload, answer, MAX_MSG_LEN);
            
            // Invia la risposta
            if (send_message(state->socket, &msg) < 0) {
                return false;
            }

            // Attendi il risultato dal server
            if (receive_message(state->socket, &msg) < 0) {
                return false;
            }

            // Gestisci la risposta del server
            switch (msg.type) {
                case MSG_ANSWER_RESULT:
                    printf("%s\n", msg.payload);
                    return true;
                case MSG_ERROR:
                    printf("Errore: %s\n", msg.payload);
                    return false;
                default:
                    printf("Messaggio inaspettato dal server (tipo: %s)\n", 
                           message_type_to_string(msg.type));
                    return false;
            }
        default:
            printf("Messaggio inaspettato dal server (tipo: %s)\n", 
                   message_type_to_string(msg.type));
            return false;
    }

    return true;
}

bool play_game_session(ClientState* state) {
    Message msg;
    
    // Se non siamo in un quiz (prima volta o quiz appena terminato)
    if (state->current_quiz == 0) {
        state->current_quiz = show_quiz_selection();

        // Invia scelta del quiz al server
        msg.type = MSG_QUESTION;
        msg.length = 1;
        msg.payload[0] = state->current_quiz + '0';
        
        if (send_message(state->socket, &msg) < 0) {
            return false;
        }
    }

    state->current_question = 0;
    char current_question[MAX_QUESTION_LENGTH] = {0}; // Buffer per salvare la domanda corrente

    while (state->current_question < MAX_QUESTIONS) {
        if (receive_message(state->socket, &msg) < 0) {
            return false;
        }

        switch (msg.type) {
            case MSG_QUESTION:
                strncpy(current_question, msg.payload, MAX_QUESTION_LENGTH - 1); // Salva la domanda per la show score
                if (!answer_question(state, msg.payload)) {
                    return false;
                }
                state->current_question++;
                break;
            case MSG_SCORE:
                printf("\n%s\n", msg.payload);
                if (!answer_question(state, current_question)) {
                    return false;
                }
                break;
            case MSG_QUIZ_AVAILABLE:
                printf("\n%s", msg.payload);
                state->current_quiz = 0;  // Reset quiz selection
                return play_game_session(state);  // Recursive call to select another quiz
            case MSG_ERROR:
                printf("\nErrore: %s\n", msg.payload);
                return false;
            case MSG_QUIZ_COMPLETED:
                printf("\n%s\n", msg.payload);
                if (strstr(msg.payload, "Hai completato tutti i quiz disponibili!")) {
                    // Il client ha completato tutti i quiz
                    // Resetta lo stato e torna al menu principale
                    state->current_quiz = 0;
                    state->current_question = 0;
                    return false;
                }
                state->current_quiz = 0;
                return play_game_session(state);
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
    
    /*
    if (!connect_to_server(state, atoi(argv[1]))) {
        cleanup_client(state);
        return 1;
    }
    */

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