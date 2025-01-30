/*
 * server.c
 * Implementazione lato server del gioco 'Trivia Quiz Multiplayer'
 * 
 * Questo file implementa un server concorrente che gestisce più client
 * simultaneamente usando select() per il multiplexing I/O. Il server gestisce
 * più partite di quiz, tiene traccia dei punteggi dei giocatori, gestisce
 * le connessioni e disconnessioni dei client in modo corretto.
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

/* Variabili globali del server */
static ClientData client_data[FD_SETSIZE];
static Quiz* sport_quiz = NULL;
static Quiz* geography_quiz = NULL;
static ServerState* server_state = NULL;

/* Funzioni di inizializzazione e cleanup */

bool init_server_socket(ServerState* state, const char* ip, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    if (bind(state->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        return false;
    }

    if (listen(state->server_socket, 20) < 0) {
        return false;
    }

    return true;
}

ServerState* init_server(const char* ip, int port) {
    ServerState* state = malloc(sizeof(ServerState));
    if (!state) return NULL;

    state->server_socket = create_socket();
    if (state->server_socket < 0) {
        free(state);
        return NULL;
    }

    if (!init_server_socket(state, ip, port)) {
        close(state->server_socket);
        free(state);
        return NULL;
    }

    state->players = create_player_array(INITIAL_PLAYER_ARRAY_SIZE);
    if (!state->players) {
        close(state->server_socket);
        free(state);
        return NULL;
    }

    FD_ZERO(&state->active_fds);
    FD_SET(state->server_socket, &state->active_fds);
    state->max_fd = state->server_socket;

    return state;
}

void cleanup_server(ServerState* state) {
    if (state) {
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

/* Funzioni di gestione connessioni */

void update_max_fd(ServerState* state) {
    while (state->max_fd > 0 && !FD_ISSET(state->max_fd, &state->active_fds)) {
        state->max_fd--;
    }
}

void init_client_data(int client_socket) {
    memset(&client_data[client_socket], 0, sizeof(ClientData));
    client_data[client_socket].is_playing = false;
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

    FD_SET(client_socket, &state->active_fds);
    if (client_socket > state->max_fd) {
        state->max_fd = client_socket;
    }

    init_client_data(client_socket);

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("\nNuovo client connesso\n");
    printf("Indirizzo IP client: %s, Porta: %d\n", 
           client_ip, ntohs(client_addr.sin_port));
}

void handle_disconnect(ServerState* state, int client_socket) {
    if (strlen(client_data[client_socket].nickname) > 0) {
        // Resetta i punteggi del giocatore e lo segna come non connesso
        reset_player_connection(state->players, client_data[client_socket].nickname);
        
        // Registro la disconnessione e il reset per il debug
        DEBUG_PRINT("Player %s disconnesso - reset del punteggio per i quiz non completati\n", 
                   client_data[client_socket].nickname);
    }

    printf("\nClient disconnesso con socket %d\n", client_socket);

    // Clear client data
    memset(&client_data[client_socket], 0, sizeof(ClientData));
    
    // Clean up socket
    FD_CLR(client_socket, &state->active_fds);
    close(client_socket);

    if (client_socket == state->max_fd) {
        update_max_fd(state);
    }
}

/* Funzioni di gestione messaggi */

void send_nickname_prompt(int client_socket) {
    Message msg;
    msg.type = MSG_NICKNAME_PROMPT;
    const char* prompt = "\nTrivia Quiz\n"
                        "+++++++++++++++++++++++++++++++++++++++++\n"
                        "Scegli un nickname (deve essere univoco): ";
    msg.length = strlen(prompt);
    strncpy(msg.payload, prompt, MAX_MSG_LEN);
    send_message(client_socket, &msg);
}

void send_quiz_available_message(int client_socket, const char* nickname) {
    Message msg;
    msg.type = MSG_QUIZ_AVAILABLE;
    
    bool sport_completed = has_completed_quiz(server_state->players, nickname, true);
    bool geo_completed = has_completed_quiz(server_state->players, nickname, false);
    
    char available[MAX_MSG_LEN];
    int offset = snprintf(available, MAX_MSG_LEN,
             "Quiz disponibili\n"
             "++++++++++++++++++++++++++++++\n");
             
    if (!sport_completed) {
        offset += snprintf(available + offset, MAX_MSG_LEN - offset, "1 - Sport\n");
    }
    
    if (!geo_completed) {
        offset += snprintf(available + offset, MAX_MSG_LEN - offset, "2 - Geografia\n");
    }
    
    if (sport_completed && geo_completed) {
        snprintf(available, MAX_MSG_LEN,
                "Non ci sono più quiz disponibili.\n"
                "Hai completato tutti i quiz!\n");
    } else {
        snprintf(available + offset, MAX_MSG_LEN - offset, 
                "++++++++++++++++++++++++++++++\n");
    }

    msg.length = strlen(available);
    strncpy(msg.payload, available, MAX_MSG_LEN);
    send_message(client_socket, &msg);
}

void send_question_to_client(int client_socket, Quiz* quiz, int question_num) {
    Question* question = get_question(quiz, question_num);
    
    if (question) {
        Message msg;
        msg.type = MSG_QUESTION;
        msg.length = strlen(question->question);
        strncpy(msg.payload, question->question, MAX_MSG_LEN);
        send_message(client_socket, &msg);
    }
}

/* Funzioni di gestione login */

bool handle_existing_player(ServerState* state, int client_socket, 
                          Player* player, const char* nickname) {
    Message msg;
    
    if (player->is_connected) {
        msg.type = MSG_LOGIN_ERROR;
        strcpy(msg.payload, "Nickname già in uso da un altro giocatore");
        msg.length = strlen(msg.payload);
        send_message(client_socket, &msg);
        return false;
    }
    
    player->is_connected = true;
    strncpy(client_data[client_socket].nickname, nickname, MAX_NICK_LENGTH - 1);
    msg.type = MSG_LOGIN_SUCCESS;

    // Controlliamo prima se il giocatore ha già completato tutti i quiz
    if (player->completed_sport && player->completed_geography) {
        strcpy(msg.payload, "Hai già completato tutti i quiz disponibili! Torna presto per nuovi quiz.");
        msg.length = strlen(msg.payload);
        send_message(client_socket, &msg);
        handle_disconnect(state, client_socket);
        return false;
    }
    
    // Se non ha completato tutti i quiz, diamo il messaggio di benvenuto
    strcpy(msg.payload, "Bentornato! Inizia un nuovo quiz per mettere alla prova le tue conoscenze!");
    msg.length = strlen(msg.payload);
    send_message(client_socket, &msg);
    
    display_server_status(state);
    send_quiz_available_message(client_socket, nickname);
    return true;
}

bool handle_new_player(ServerState* state, int client_socket, const char* nickname) {
    
    // Controlla se il server ha raggiunto la massima capacità di giocatori
    if (!add_player(state->players, nickname)) {
        Message msg;
        msg.type = MSG_LOGIN_ERROR;
        strcpy(msg.payload, "Il server ha raggiunto la massima capacità di giocatori, riprova più tardi.");
        msg.length = strlen(msg.payload);
        send_message(client_socket, &msg);
        return false;
    }
    
    Player* new_player = find_player(state->players, nickname);
    new_player->is_connected = true;

    strncpy(client_data[client_socket].nickname, nickname, MAX_NICK_LENGTH - 1);
    
    Message msg;
    msg.type = MSG_LOGIN_SUCCESS;
    strcpy(msg.payload, "Login avvenuto con successo!");
    msg.length = strlen(msg.payload);
    send_message(client_socket, &msg);
    
    display_server_status(state);
    send_quiz_available_message(client_socket, nickname);
    return true;
}

void handle_login_request(ServerState* state, int client_socket, Message* msg) {
    msg->payload[msg->length] = '\0';
    Player* existing_player = find_player(state->players, msg->payload);

    if (existing_player) {
        handle_existing_player(state, client_socket, existing_player, msg->payload);
    } else {
        handle_new_player(state, client_socket, msg->payload);
    }
}

/* Funzioni di gestione quiz */

void handle_answer(ServerState* state, int client_socket, Message* msg) {
    ClientData* client = &client_data[client_socket];
    if (!client->is_playing) return;
    
    Quiz* quiz = (client->current_quiz == 1) ? sport_quiz : geography_quiz;
    msg->payload[msg->length] = '\0';
    DEBUG_PRINT("Ricevuta risposta dal giocatore %s: %s", client->nickname, msg->payload);
    
    bool correct = check_answer(quiz, client->current_question, msg->payload);
    Player* player = find_player(state->players, client->nickname);

    Message response_msg;
    response_msg.type = MSG_ANSWER_RESULT;
    strcpy(response_msg.payload, correct ? "Risposta corretta!" : "Risposta errata!");
    response_msg.length = strlen(response_msg.payload);
    
    if (send_message(client_socket, &response_msg) < 0) {
        handle_disconnect(state, client_socket);
        return;
    }
    
    if (player) {
        if (client->current_quiz == 1) {
            player->sport_score += correct ? 1 : 0;
        } else {
            player->geography_score += correct ? 1 : 0;
        }
    }

    DEBUG_PRINT("Punteggio aggiornato per il giocatore %s - Quiz: %s, Nuovo punteggio: %d", 
            client->nickname,
            client->current_quiz == 1 ? "Sport" : "Geografia",
            client->current_quiz == 1 ? player->sport_score : player->geography_score);

    handle_next_question(state, client_socket, client);
}

void handle_next_question(ServerState* state, int client_socket, ClientData* client) {
    client->current_question++;
    if (client->current_question >= QUESTIONS_PER_GAME) {
        handle_quiz_completion(state, client_socket, client);
    } else {
        Quiz* quiz = (client->current_quiz == 1) ? sport_quiz : geography_quiz;
        send_question_to_client(client_socket, quiz, client->current_question);
    }
}

void handle_quiz_completion(ServerState* state, int client_socket, ClientData* client) {
    mark_quiz_as_completed(state->players, client->nickname, client->current_quiz == 1);
    DEBUG_PRINT("Quiz completato dal giocatore %s - Quiz numero: %d", 
                client->nickname, client->current_quiz);

    client->is_playing = false;

    Message complete_msg;
    complete_msg.type = MSG_QUIZ_COMPLETED;

    bool sport_completed = has_completed_quiz(state->players, client->nickname, true);
    bool geo_completed = has_completed_quiz(state->players, client->nickname, false);
    
    if (sport_completed && geo_completed) {
        char* scores = format_scores(state);
        snprintf(complete_msg.payload, MAX_MSG_LEN, 
                "Hai completato tutti i quiz disponibili!\n\n%s", scores);
        complete_msg.type = MSG_TRIVIA_COMPLETED;
        reset_player_connection(state->players, client->nickname);
        memset(&client_data[client_socket], 0, sizeof(ClientData));
    } else {
        strcpy(complete_msg.payload, "Quiz completato!");
    }
    
    complete_msg.length = strlen(complete_msg.payload);
    if (send_message(client_socket, &complete_msg) < 0) {
        handle_disconnect(state, client_socket);
        return;
    }

    if (!sport_completed || !geo_completed) {
        send_quiz_available_message(client_socket, client->nickname);
    }
}

void handle_quiz_selection(ServerState* state, int client_socket, Message* msg) {
    ClientData* client = &client_data[client_socket];
    bool sport_completed = has_completed_quiz(state->players, client->nickname, true);
    bool geo_completed = has_completed_quiz(state->players, client->nickname, false);
    
    int selected_quiz = msg->payload[0] - '0';
    
    if ((selected_quiz == 1 && sport_completed) || 
        (selected_quiz == 2 && geo_completed)) {
        msg->type = MSG_QUIZ_AVAILABLE;
        snprintf(msg->payload, MAX_MSG_LEN, 
            "Quiz non disponibile. Seleziona un quiz dalla lista.\n");
        msg->length = strlen(msg->payload);
        send_message(client_socket, msg);
        send_quiz_available_message(client_socket, client->nickname);
        return;
    }
    
    client->current_quiz = selected_quiz;
    DEBUG_PRINT("Player %s ha selezionato il quiz %d", client->nickname, selected_quiz);

    client->current_question = 0;
    client->is_playing = true;
    
    Quiz* selected_quiz_ptr = (client->current_quiz == 1) ? sport_quiz : geography_quiz;
    select_random_questions(selected_quiz_ptr);
    send_question_to_client(client_socket, selected_quiz_ptr, client->current_question);
}

/* Funzioni di gestione server */

void broadcast_message(ServerState* state, Message* msg) {
    for (int i = 0; i <= state->max_fd; i++) {
        if (FD_ISSET(i, &state->active_fds) && i != state->server_socket) {
            send_message(i, msg);
        }
    }
}

void handle_shutdown() {
    if (server_state) {
        Message msg;
        msg.type = MSG_DISCONNECT;
        msg.length = strlen("Server shutdown");
        strncpy(msg.payload, "Server shutdown", MAX_MSG_LEN - 1);
        
        broadcast_message(server_state, &msg);
        cleanup_server(server_state);
    }
    exit(0);
}

void display_server_status(ServerState* state) {
    printf("\nStato Server Trivia Quiz\n");
    printf("++++++++++++++++++++++++++++\n");
    
    printf("Temi disponibili:\n");
    if (sport_quiz && sport_quiz->topic[0] != '\0') {
        printf("1. %s\n", sport_quiz->topic);
    }

    if (geography_quiz && geography_quiz->topic[0] != '\0') {
        printf("2. %s\n", geography_quiz->topic);
    }
    
    char* scores = format_scores(state);
    printf("%s", scores);
    printf("++++++++++++++++++++++++++++\n\n");
}

/* Funzioni di gestione messaggi del client */

void process_client_message(ServerState* state, int client_socket) {
    Message msg;
    DEBUG_PRINT("Tentativo di ricezione messaggio dal client %d\n", client_socket);
    
    ssize_t received = receive_message(client_socket, &msg);
    if (received <= 0) {
        DEBUG_PRINT("Ricezione fallita con codice %zd\n", received);
        handle_disconnect(state, client_socket);
        return;
    }
    
    switch (msg.type) {
        case MSG_LOGIN:
            send_nickname_prompt(client_socket);
            break;

        case MSG_REQUEST_NICKNAME:
            handle_login_request(state, client_socket, &msg);
            break;

        case MSG_REQUEST_QUESTION:
            handle_quiz_selection(state, client_socket, &msg);
            break;

        case MSG_ANSWER:
            handle_answer(state, client_socket, &msg);
            break;

        case MSG_REQUEST_SCORE:
            char* scores = format_scores(state);
            msg.type = MSG_SCORE;
            msg.length = strlen(scores);
            strncpy(msg.payload, scores, MAX_MSG_LEN - 1);
            msg.payload[MAX_MSG_LEN - 1] = '\0';
            send_message(client_socket, &msg);
            break;
        
        case MSG_END_QUIZ:
            {
                ClientData* client = &client_data[client_socket];
                client->is_playing = false;
                if (strlen(client->nickname) > 0) {
                    reset_player_connection(state->players, client->nickname);
                    memset(client, 0, sizeof(ClientData));
                    
                    // Invia conferma al client
                    Message response;
                    response.type = MSG_QUIZ_COMPLETED;
                    strncpy(response.payload, "Endquiz ricevuto. Ridirezione al menu principale.", MAX_MSG_LEN);
                    response.length = strlen(response.payload);
                    send_message(client_socket, &response);
                }
            }
            break;

        case MSG_DISCONNECT:
            handle_disconnect(state, client_socket);
            break;

        case MSG_ERROR:
            printf("ATTENZIONE: Ricevuto MSG_ERROR inatteso dal client\n");
            break;

        default:
            printf("ATTENZIONE: Tipo di messaggio non gestito: %d\n", msg.type);
            break;
    }
}

/* Funzioni di inizializzazione quiz */

bool load_quiz_files() {
    sport_quiz = load_quiz("res/sport_quiz.txt");
    geography_quiz = load_quiz("res/geography_quiz.txt");
    
    return sport_quiz && geography_quiz;
}

/* Main del server */

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <porta>\n", argv[0]);
        return 1;
    }

    // Inizializza il generatore di numeri casuali
    srand(time(NULL));

    // Carica i quiz
    if (!load_quiz_files()) {
        fprintf(stderr, "Errore nel caricamento dei quiz\n");
        return 1;
    }

    ServerState* state = init_server("127.0.0.1", atoi(argv[1]));
    if (!state) {
        fprintf(stderr, "Errore nell'inizializzazione del server\n");
        return 1;
    }

    DEBUG_PRINT("Server avviato sulla porta %s", argv[1]);
    
    server_state = state;  // Salva il riferimento globale
    display_server_status(state);

    // Imposta i gestori dei segnali
    signal(SIGINT, handle_shutdown);
    signal(SIGTERM, handle_shutdown);

    // Loop principale del server
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