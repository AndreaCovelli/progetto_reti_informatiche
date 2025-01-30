/*
 * client.c
 * Implementazione lato client del gioco 'Trivia Quiz Multiplayer'
 * 
 * Questo file contiene l'implementazione di un'applicazione client per il gioco
 * 'Trivia Quiz Multiplayer'. Il client si connette a un server, permette agli utenti di
 * partecipare a quiz su diversi argomenti, gestisce l'interazione utente e lo
 * stato del gioco.
 */

#include "include/debug.h" 
#include "include/client.h"
#include "include/common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Funzioni di gestione della connessione */

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
    DEBUG_PRINT("Connessione al server sulla porta %d\n", port);
    
    int sock = setup_connection("127.0.0.1", port);
    if (sock < 0) {
        fprintf(stderr, "Impossibile connettersi al server\n");
        return false;
    }
    
    state->socket = sock;
    DEBUG_PRINT("Connesso con socket %d\n", state->socket);
    return true;
}

/* Funzioni di gestione della UI */

void print_banner() {
    printf("\nTrivia Quiz\n");
    printf("++++++++++++++++++++++++++++++++++++\n");
}

void print_menu_options() {
    printf("Menù:\n");
    printf("1 - Comincia una sessione di Trivia\n");
    printf("2 - Esci\n");
    printf("++++++++++++++++++++++++++++++++++++\n");
}

int get_menu_choice() {
    char input[10];
    printf("La tua scelta: ");
    
    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 2;  // Uscita in caso di errore
    }
    
    input[strcspn(input, "\n")] = 0;
    return atoi(input);
}

int show_main_menu() {
    print_banner();
    print_menu_options();
    return get_menu_choice();
}

/* Funzioni di gestione del gioco */

bool handle_quiz_selection(ClientState* state) {
    char input[10];
    int old_quiz = state->current_quiz;
    
    while (1) {
        printf("La tua scelta: ");
        
        if (!fgets(input, sizeof(input), stdin)) {
            return false;
        }
        
        input[strcspn(input, "\n")] = 0;
        
        int choice;
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Per favore, inserisci 1 per Sport o 2 per Geografia.\n");
            continue;
        }
        
        if (choice != 1 && choice != 2) {
            printf("Scelta non valida. Inserisci 1 per Sport o 2 per Geografia.\n");
            continue;
        }

        if(old_quiz == choice){
            printf("Hai già completato questo quiz. Scegli un altro quiz.\n");
            continue;
        }
        
        state->current_quiz = choice;
        return true;
    }
}

bool handle_nickname_response(Message* msg) {
    switch (msg->type) {
        case MSG_LOGIN_SUCCESS:
            printf("%s\n", msg->payload);
            return true;

        case MSG_LOGIN_ERROR:
            printf("%s\n", msg->payload);
            return false;

        default:
            printf("Messaggio inaspettato dal server\n");
            return false;
    }
}

bool send_nickname_request(ClientState* state) {
    Message msg;
    msg.type = MSG_REQUEST_NICKNAME;
    msg.length = strlen(state->nickname);
    strncpy(msg.payload, state->nickname, MAX_MSG_LEN);

    return send_message(state->socket, &msg) >= 0;
}

bool validate_and_send_nickname(ClientState* state) {
    Message msg;
    bool nickname_valid = false;

    msg.type = MSG_LOGIN;
    msg.length = 0;
    strncpy(msg.payload, "", MAX_MSG_LEN);
    
    if (send_message(state->socket, &msg) < 0) {
        return false;
    }

    while (!nickname_valid) {
        if (receive_message(state->socket, &msg) < 0) {
            printf("Server disconnesso durante la scelta del nickname\n");
            return false;
        }

        if (msg.type == MSG_NICKNAME_PROMPT) {
            printf("\n%s", msg.payload);
            
            fgets(state->nickname, MAX_NICK_LENGTH, stdin);
            state->nickname[strcspn(state->nickname, "\n")] = 0;
            DEBUG_PRINT("Nickname inserito: %s", state->nickname);

            if (!send_nickname_request(state)) {
                return false;
            }

            if (receive_message(state->socket, &msg) < 0) {
                return false;
            }

            nickname_valid = handle_nickname_response(&msg);
            DEBUG_PRINT("Risultato validazione nickname: %s", nickname_valid ? "Successo" : "Fallimento");
        } else {
            printf("Messaggio inaspettato dal server\n");
            return false;
        }
    }

    return true;
}

/* Funzioni di gestione delle risposte */

bool handle_answer_result(Message* msg) {
    switch (msg->type) {
        case MSG_ANSWER_RESULT:
            printf("%s\n", msg->payload);
            return true;
        case MSG_ERROR:
            printf("Errore: %s\n", msg->payload);
            return false;
        default:
            printf("Messaggio inaspettato dal server (tipo: %s)\n", 
                   message_type_to_string(msg->type));
            return false;
    }
}

bool send_answer(ClientState* state, const char* answer) {
    Message msg;
    msg.type = MSG_ANSWER;
    msg.length = strlen(answer);
    strncpy(msg.payload, answer, MAX_MSG_LEN);
    
    if (send_message(state->socket, &msg) < 0) {
        return false;
    }

    if (receive_message(state->socket, &msg) < 0) {
        return false;
    }

    return handle_answer_result(&msg);
}

bool handle_special_commands(ClientState* state, const char* answer) {
    Message msg;
    
    if (strcmp(answer, "show score") == 0) {
        msg.type = MSG_REQUEST_SCORE;
        msg.length = strlen(answer);
        strncpy(msg.payload, answer, MAX_MSG_LEN);
        return send_message(state->socket, &msg) >= 0;
    } 
    
    if (strcmp(answer, "endquiz") == 0) {
        msg.type = MSG_END_QUIZ;
        msg.length = strlen(answer);
        strncpy(msg.payload, answer, MAX_MSG_LEN);
        
        if (send_message(state->socket, &msg) < 0) {
            return false;
        }
        
        state->current_quiz = 0;  // Reset dello stato del quiz
        return false;  // Segnala al chiamante di terminare il quiz
    }
    
    return false;  // Non è un comando speciale
}

bool answer_question(ClientState* state, const char* question) {
    char answer[MAX_ANSWER_LENGTH];

    printf("\nDomanda: %s\n", question);
    printf("Risposta (o 'show score' per vedere i punteggi, 'endquiz' per uscire): ");
    
    if (!fgets(answer, MAX_ANSWER_LENGTH, stdin)) {
        return false;
    }
    answer[strcspn(answer, "\n")] = 0;

    if (handle_special_commands(state, answer)) {
        return true;
    }

    return send_answer(state, answer);
}

/* Funzioni di gestione della sessione di gioco */

bool handle_game_message(ClientState* state, Message* msg, char* current_question) {
    DEBUG_PRINT("Ricevuto game message di tipo: %s", message_type_to_string(msg->type));

    switch (msg->type) {
        case MSG_QUESTION:
            strncpy(current_question, msg->payload, MAX_QUESTION_LENGTH - 1);
            if (!answer_question(state, msg->payload)) {
                return false;
            }
            state->current_question++;
            break;
            
        case MSG_SCORE:
            printf("\n%s\n", msg->payload);
            if (!answer_question(state, current_question)) {
                return false;
            }
            break;
            
        case MSG_QUIZ_AVAILABLE:
            printf("\n%s", msg->payload);
            if (!handle_quiz_selection(state)) {
                return false;
            }
            
            Message quiz_msg;
            quiz_msg.type = MSG_REQUEST_QUESTION;
            quiz_msg.length = 1;
            quiz_msg.payload[0] = state->current_quiz + '0';
            quiz_msg.payload[1] = '\0';
            
            if (send_message(state->socket, &quiz_msg) < 0) {
                printf("Server disconnesso durante l'invio della scelta del quiz\n");
                return false;
            }
            break;
            
        case MSG_ERROR:
            printf("\nErrore: %s\n", msg->payload);
            return false;
            
        case MSG_QUIZ_COMPLETED:
            printf("\n%s\n", msg->payload);
            break;
            
        case MSG_TRIVIA_COMPLETED:
            printf("\n%s\n", msg->payload);
            state->current_quiz = 0;
            state->current_question = 0;
            return false;
            
        case MSG_DISCONNECT:
            printf("\nIl server si è disconnesso. Il quiz è terminato.\n");
            return false;
            
        default:
            printf("\n%s\n", msg->payload);
            break;
    }
    
    return true;
}

bool play_game_session(ClientState* state) {
    Message msg;
    state->current_question = 0;
    DEBUG_PRINT("Avviando una nuova sessione di gioco per il giocatore: %s", state->nickname);
    char current_question[MAX_QUESTION_LENGTH] = {0};

    while (state->current_question < MAX_QUESTIONS) {
        if (receive_message(state->socket, &msg) < 0) {
            printf("Server disconnesso durante la sessione di gioco\n");
            return false;
        }

        if (!handle_game_message(state, &msg, current_question)) {
            return false;
        }
    }

    DEBUG_PRINT("Sessione di gioco terminata per il giocatore: %s", state->nickname);

    return true;
}

/* Main e funzioni di gestione disconnessione */

void disconnect_from_server(ClientState* state) {
    if (state->socket != -1) {
        Message msg;
        msg.type = MSG_DISCONNECT;
        msg.length = 0;
        strncpy(msg.payload, "", MAX_MSG_LEN);
        
        send_message(state->socket, &msg);
        
        close(state->socket);
        state->socket = -1;
    }
}

bool handle_game_session(ClientState* state) {
    if (!validate_and_send_nickname(state)) {
        if (state->socket != -1) {
            disconnect_from_server(state);
        }
        return false;
    }
    
    if (!play_game_session(state)) {
        if (state->socket != -1) {
            disconnect_from_server(state);
        }
        return false;
    }
    
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <porta>\n", argv[0]);
        return 1;
    }

    ClientState* state = init_client();
    if (!state) {
        fprintf(stderr, "Errore nell'inizializzazione del client\n");
        return 1;
    }

    int choice;
    do {
        choice = show_main_menu();
        switch (choice) {
            case 1:
                if (state->socket == -1) {
                    if (!connect_to_server(state, atoi(argv[1])))
                        break;
                }
                handle_game_session(state);
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