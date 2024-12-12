/*
    Implementazione del server del Trivia Game
*/

#include "include/server.h"
#include "include/quiz.h"
#include "include/score.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

// Array per tenere traccia dei client connessi
static ClientData client_data[FD_SETSIZE];
static Quiz* sport_quiz = NULL;
static Quiz* geography_quiz = NULL;

static ServerState* server_state = NULL;

ServerState* init_server(const char* ip, int port) {
    ServerState* state = malloc(sizeof(ServerState));
    if (!state) return NULL;

    // Inizializza il socket del server
    state->server_socket = create_socket();
    if (state->server_socket < 0) {
        free(state);
        return NULL;
    }

    // Configura l'indirizzo del server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    // Bind del socket
    if (bind(state->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        close(state->server_socket);
        free(state);
        return NULL;
    }

    // Metti il socket in ascolto
    if (listen(state->server_socket, 20) < 0) {
        close(state->server_socket);
        free(state);
        return NULL;
    }

    // Inizializza il PlayerArray
    state->players = create_player_array(INITIAL_PLAYER_ARRAY_SIZE);
    if (!state->players) {
        close(state->server_socket);
        free(state);
        return NULL;
    }

    // Inizializza i set di file descriptor
    FD_ZERO(&state->active_fds);
    FD_SET(state->server_socket, &state->active_fds);
    state->max_fd = state->server_socket;

    return state;
}

void cleanup_server(ServerState* state) {
    if (state) {
        // Chiudi tutti i socket
        for (int i = 0; i <= state->max_fd; i++) {
            if (FD_ISSET(i, &state->active_fds)) {
                close(i);
            }
        }
        free_player_array(state->players);
        free(state);
    }
    if (sport_quiz) free_quiz(sport_quiz);
    if (geography_quiz) free_quiz(geography_quiz);
}

void handle_new_connection(ServerState* state) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    int client_socket = accept(state->server_socket, 
                             (struct sockaddr*)&client_addr, 
                             &client_len);
    
    if (client_socket < 0) {
        perror("Errore nell'accept");
        return;
    }

    printf("DEBUG: New client socket: %d\n", client_socket);

    // Aggiungi il nuovo socket al set
    FD_SET(client_socket, &state->active_fds);
    if (client_socket > state->max_fd) {
        state->max_fd = client_socket;
    }

    // Inizializza i dati del client
    memset(&client_data[client_socket], 0, sizeof(ClientData));
    client_data[client_socket].is_playing = false;

    printf("Nuovo client connesso\n");
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Indirizzo IP client: %s, Porta: %d\n", 
           client_ip, ntohs(client_addr.sin_port));
}

void display_server_status(ServerState* state) {
    printf("\nStato del Server Trivia Quiz\n");
    printf("++++++++++++++++++++++++++++\n");
    
    // Mostra i temi disponibili
    printf("Temi disponibili:\n");
    if (sport_quiz && sport_quiz->topic[0] != '\0') {
        printf("1. %s\n", sport_quiz->topic);
    }
    if (geography_quiz && geography_quiz->topic[0] != '\0') {
        printf("2. %s\n", geography_quiz->topic);
    }
    
    // Mostra il numero e i nomi dei client connessi
    printf("\nClient connessi (%d):\n", state->players->count);
    
    // Se ci sono client connessi, mostra i loro nickname
    if (state->players->count > 0) {
        for (int i = 0; i < state->players->count; i++) {
            printf("- %s\n", state->players->players[i].nickname);
        }
    } else {
        printf("Nessun client connesso\n");
    }
    printf("++++++++++++++++++++++++++++\n\n");
}

void send_nickname_prompt(int client_socket) {
    Message msg;
    msg.type = MSG_NICKNAME_PROMPT;
    const char* prompt = "\nTrivia Quiz\n"
                        "+++++++++++++++++++++++++++++++++++++++++\n"
                        "Scegli un nickname (deve essere univoco): ";
    msg.length = strlen(prompt);
    strncpy(msg.payload, prompt, MAX_MSG_LEN);
    send_message(client_socket, &msg);
    //receive_message(client_socket, &msg);
}

void show_quiz_selection(){
    printf("Seleziona il quiz:\n"
           "1 - Sport\n"
           "2 - Geografia\n");
}

void send_quiz_options(int client_socket) {
    Message msg;
    msg.type = MSG_QUIZ_AVAILABLE;
    
    // Get the client's nickname from the client_data array
    const char* nickname = client_data[client_socket].nickname;
    
    // Check which quizzes the client has completed
    bool sport_completed = has_completed_quiz(server_state->players, nickname, true);
    bool geo_completed = has_completed_quiz(server_state->players, nickname, false);
    
    // Build the message with only available quizzes
    char available[MAX_MSG_LEN];
    int offset = snprintf(available, MAX_MSG_LEN,
             "Quiz disponibili\n"
             "++++++++++++++++++++++++++++++\n");
             
    // Add sport quiz option only if not completed
    if (!sport_completed) {
        offset += snprintf(available + offset, MAX_MSG_LEN - offset, "1 - Sport\n");
    }
    
    // Add geography quiz option only if not completed 
    if (!geo_completed) {
        offset += snprintf(available + offset, MAX_MSG_LEN - offset, "2 - Geografia\n");
    }
    
    // If all quizzes are completed, show a different message
    if (sport_completed && geo_completed) {
        snprintf(available, MAX_MSG_LEN,
                "Non ci sono più quiz disponibili.\n"
                "Hai completato tutti i quiz!\n");
    } else {
        // Add the closing border only if there are available quizzes
        snprintf(available + offset, MAX_MSG_LEN - offset, 
                "++++++++++++++++++++++++++++++\n");
    }

    msg.length = strlen(available);
    strncpy(msg.payload, available, MAX_MSG_LEN);
    send_message(client_socket, &msg);
}

void send_question_to_client(int client_socket, Quiz* quiz, int question_num) {
    Message msg;
    Question* question = get_question(quiz, question_num);
    
    if (question) {
        msg.type = MSG_QUESTION;
        msg.length = strlen(question->question);
        strncpy(msg.payload, question->question, MAX_MSG_LEN);
        send_message(client_socket, &msg);
    }
}

void process_client_message(ServerState* state, int client_socket) {
    Message msg;
    printf("DEBUG: Attempting to receive message from client %d\n", client_socket);
    
    ssize_t received = receive_message(client_socket, &msg);
    if (received <= 0) {
        printf("DEBUG: Receive failed with code %zd\n", received);
        // Client disconnesso
        handle_disconnect(state, client_socket);
        return;
    }
    
    printf("DEBUG: Successfully received message type: %s, length: %d, payload: %s\n",
           message_type_to_string(msg.type), msg.length, msg.payload);
    
    ClientData* client = &client_data[client_socket];
    
    switch (msg.type) {
        case MSG_LOGIN:
            // Invia il prompt per il nickname
            send_nickname_prompt(client_socket);
            break;
        case MSG_REQUEST_NICKNAME:
            // Process login...
            msg.payload[msg.length] = '\0';
            if (add_player(state->players, msg.payload)) {
                strncpy(client->nickname, msg.payload, MAX_NICK_LENGTH - 1);
                msg.type = MSG_LOGIN_SUCCESS;
                strcpy(msg.payload, "Login avvenuto con successo!");
                msg.length = strlen(msg.payload);
                send_message(client_socket, &msg);
                
                // After successful login, send quiz options
                send_quiz_options(client_socket);
            } else {
                msg.type = MSG_LOGIN_ERROR;
                strcpy(msg.payload, "Nickname già preso, scegline un altro");
                msg.length = strlen(msg.payload);
                send_message(client_socket, &msg);
            }
            break;

        // il client chiede una domanda di un certo quiz
        case MSG_REQUEST_QUESTION:

            // Verifica se il quiz selezionato è già stato completato
            bool sport_completed = has_completed_quiz(state->players, 
                                                    client->nickname, true);
            bool geo_completed = has_completed_quiz(state->players, 
                                                client->nickname, false);
            
            int selected_quiz = msg.payload[0] - '0';
            
            // Verifica se il quiz selezionato è già stato completato
            if ((selected_quiz == 1 && sport_completed) || 
                (selected_quiz == 2 && geo_completed)) {
                msg.type = MSG_QUIZ_AVAILABLE;
                printf("DEBUG: Quiz %d already completed\n", selected_quiz);
                snprintf(msg.payload, MAX_MSG_LEN, 
                    "Quiz non disponibile. Seleziona un quiz dalla lista.\n");
                msg.length = strlen(msg.payload);
                send_message(client_socket, &msg);
                send_quiz_options(client_socket);
                break;
            }
            
            client->current_quiz = selected_quiz;
            client->current_question = 0;
            client->is_playing = true;
            
            Quiz* selected_quiz_ptr = (client->current_quiz == 1) ? 
                                    sport_quiz : geography_quiz;
            select_random_questions(selected_quiz_ptr);
            send_question_to_client(client_socket, selected_quiz_ptr, 
                                client->current_question);
            break;
        // Il client risponde a una domanda di un certo quiz
        case MSG_ANSWER:
            // Gestione risposta
            if (!client->is_playing) break;
            
            Quiz* quiz = (client->current_quiz == 1) ? sport_quiz : geography_quiz;
            msg.payload[msg.length] = '\0';
            
            bool correct = check_answer(quiz, client->current_question, msg.payload);
            printf("DEBUG: Answer is %s\n", correct ? "correct" : "incorrect");
            Player* player = find_player(state->players, client->nickname);

            // Invia il messaggio di risposta corretta/errata al client
            Message response_msg;
            response_msg.type = MSG_ANSWER_RESULT;
            if (correct) {
                strcpy(response_msg.payload, "Risposta corretta!");
            } else {
                strcpy(response_msg.payload, "Risposta errata!");
            }
            response_msg.length = strlen(response_msg.payload);
            if (send_message(client_socket, &response_msg) < 0) {
                handle_disconnect(state, client_socket);
                break;
            }
            
            if (player) {
                if (client->current_quiz == 1) {
                    player->sport_score += correct ? 1 : 0;
                } else {
                    player->geography_score += correct ? 1 : 0;
                }
            }

            client->current_question++;
            if (client->current_question >= QUESTIONS_PER_GAME) {
                mark_quiz_as_completed(state->players, client->nickname, 
                                    client->current_quiz == 1);
                client->is_playing = false;

                Message complete_msg;
                complete_msg.type = MSG_QUIZ_COMPLETED;

                // Verifica se ci sono altri quiz disponibili
                bool sport_completed = has_completed_quiz(state->players, 
                                                        client->nickname, true);
                bool geo_completed = has_completed_quiz(state->players, 
                                                    client->nickname, false);
                
                if (sport_completed && geo_completed) {
                    // Il client ha completato entrambi i quiz
                    char* scores = format_scores(state);
                    printf("%s", scores);
                    snprintf(complete_msg.payload, MAX_MSG_LEN, 
                            "Hai completato tutti i quiz disponibili!\n\nPunteggi finali:\n%s", 
                            scores);
                    complete_msg.type = MSG_TRIVIA_COMPLETED;
                    // Rimuovi il giocatore
                    remove_player(state->players, client->nickname);
                    // Resetta i dati del client
                    memset(&client_data[client_socket], 0, sizeof(ClientData));
                } else {
                    strcpy(complete_msg.payload, "Quiz completato!");
                }
                
                complete_msg.length = strlen(complete_msg.payload);
                if (send_message(client_socket, &complete_msg) < 0) {
                    handle_disconnect(state, client_socket);
                    break;
                }

                if (!sport_completed || !geo_completed) {
                    // Send quiz options only if there are still available quizzes
                    send_quiz_options(client_socket);
                }
            } else {
                // Se ci sono altre domande, invia la prossima
                send_question_to_client(client_socket, quiz, client->current_question);
            }
            break;

        case MSG_REQUEST_SCORE:
            // Gestione richiesta punteggi
            char* scores = format_scores(state);
            printf("%s", scores);
            msg.type = MSG_SCORE;
            msg.length = strlen(scores);
            strncpy(msg.payload, scores, MAX_MSG_LEN - 1);
            msg.payload[MAX_MSG_LEN - 1] = '\0';
            send_message(client_socket, &msg);
            break;
        
        case MSG_END_QUIZ:
            // Gestione quiz terminato con endquiz
            client->is_playing = false;

            // Rimuovi il giocatore dalla lista
            if (strlen(client_data[client_socket].nickname) > 0) {
                remove_player(state->players, client_data[client_socket].nickname);
                // Resetta completamente i dati del client
                memset(&client_data[client_socket], 0, sizeof(ClientData));
            }

            break;

        case MSG_DISCONNECT:
            handle_disconnect(state, client_socket);
            break;

        case MSG_ERROR:
            // I messaggi di errore vengono inviati solo dal server al client
            printf("WARNING: Received unexpected MSG_ERROR from client\n");
            break;

        default:
            printf("WARNING: Unhandled message type: %d\n", msg.type);
            break;
    }
}

void handle_disconnect(ServerState* state, int client_socket) {
    // Rimuovi il giocatore dalla lista
    if (strlen(client_data[client_socket].nickname) > 0) {
        remove_player(state->players, client_data[client_socket].nickname);
    }

    printf("Client disconnesso\n");

    // Rimuovi il socket dal set
    FD_CLR(client_socket, &state->active_fds);
    close(client_socket);

    // Se necessario aggiorna max_fd
    if (client_socket == state->max_fd) {
        while (state->max_fd > 0 && !FD_ISSET(state->max_fd, &state->active_fds)) {
            state->max_fd--;
        }
    }
}

void broadcast_message(ServerState* state, Message* msg) {
    for (int i = 0; i <= state->max_fd; i++) {
        if (FD_ISSET(i, &state->active_fds) && i != state->server_socket) {
            send_message(i, msg);
        }
    }
}

void handle_shutdown() {
    if (server_state) {
        // Prepara e invia il messaggio di chiusura a tutti i client
        Message msg;
        msg.type = MSG_DISCONNECT;
        msg.length = strlen("Server shutdown");
        strncpy(msg.payload, "Server shutdown", MAX_MSG_LEN - 1);
        msg.payload[MAX_MSG_LEN - 1] = '\0';
        
        // Invia il messaggio di disconnessione a tutti i client
        broadcast_message(server_state, &msg);
        
        // Pulisci le risorse
        cleanup_server(server_state);
    }
    
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <porta>\n", argv[0]);
        return 1;
    }

    // Inizializza il generatore di numeri casuali
    srand(time(NULL));

    // Carica i quiz
    sport_quiz = load_quiz("res/sport_quiz.txt");
    geography_quiz = load_quiz("res/geography_quiz.txt");
    
    if (!sport_quiz || !geography_quiz) {
        fprintf(stderr, "Errore nel caricamento dei quiz\n");
        return 1;
    }

    ServerState* state = init_server("127.0.0.1", atoi(argv[1]));
    if (!state) {
        fprintf(stderr, "Errore nell'inizializzazione del server\n");
        return 1;
    }

    printf("Server avviato sulla porta %s\n", argv[1]);
    
    server_state = state;  // Salva il riferimento globale
    display_server_status(state);

    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    while (1) {
        state->read_fds = state->active_fds;
        
        if (select(state->max_fd + 1, &state->read_fds, NULL, NULL, NULL) < 0) {
            perror("Errore nella select");
            break;
        }

        for (int i = 0; i <= state->max_fd; i++) {
            if (FD_ISSET(i, &state->read_fds)) {
                if (i == state->server_socket) {
                    handle_new_connection(state);
                } else {
                    process_client_message(state, i);
                }
            }
        }
    }

    cleanup_server(state);
    return 0;
}