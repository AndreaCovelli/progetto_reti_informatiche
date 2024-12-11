// File contenente le costanti
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Limiti di sistema
#define MAX_CLIENTS (FD_SETSIZE - 4)  // -4 per socket ascolto e stdin/stdout/stderr
#define ABSOLUTE_MAX_PLAYERS (MAX_CLIENTS)

// Limiti buffer
#define BUFFER_SIZE 1024
#define LINE_BUFFER 256

// Limiti messaggi
#define MAX_MSG_LEN 512
#define MAX_NICK_LENGTH 20
#define QUESTIONS_PER_GAME 5
#define MAX_QUESTIONS 100
#define MAX_QUESTION_LENGTH 128
#define MAX_ANSWER_LENGTH 64

/**
 * Tipi di messaggio
 * @param MSG_LOGIN Messaggio di login (richiesta nickname)
 * @param MSG_QUESTION Indica: questa è una domanda
 * @param MSG_ANSWER Indica: questa è una risposta
 * @param MSG_ANSWER_RESULT Risultato della risposta inviato al client
 */
typedef enum {
    MSG_LOGIN = 1,
    MSG_LOGIN_SUCCESS,
    MSG_LOGIN_ERROR,
    MSG_REQUEST_QUESTION,
    MSG_QUESTION,
    MSG_ANSWER,
    MSG_ANSWER_RESULT,
    MSG_SCORE,
    MSG_QUIZ_COMPLETED,
    MSG_QUIZ_AVAILABLE,
    MSG_TRIVIA_COMPLETED,
    MSG_END_QUIZ,
    MSG_DISCONNECT,
    MSG_ERROR,
    MSG_REQUEST_NICKNAME,
    MSG_NICKNAME_PROMPT,
    MSG_REQUEST_SCORE
} MessageType;

// Capacità iniziale dell'array di giocatori
#define INITIAL_PLAYER_ARRAY_SIZE 10

// Codici di stato/errore
#define SUCCESS 0
#define ERR_SOCKET -1
#define ERR_CONNECT -2
#define ERR_SEND -3
#define ERR_RECV -4

#endif