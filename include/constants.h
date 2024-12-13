// File contenente le costanti
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Limiti di sistema
#define MAX_CLIENTS (FD_SETSIZE - 4)  // -4 per socket ascolto e stdin/stdout/stderr
// Capacità iniziale dell'array di giocatori
#define INITIAL_PLAYER_ARRAY_SIZE 10
// Numero massimo di giocatori
#define ABSOLUTE_MAX_PLAYERS (MAX_CLIENTS)

// Limiti buffer
#define BUFFER_SIZE 1024
#define LINE_BUFFER 256

// Limite lunghezza messaggio
#define MAX_MSG_LEN 512
// Limite lunghezza nickname
#define MAX_NICK_LENGTH 20
// Limite numero di domande per quiz
#define QUESTIONS_PER_GAME 5
// Limite numero di domande caricate
#define MAX_QUESTIONS 100
// Limite lunghezza domanda
#define MAX_QUESTION_LENGTH 128
// Limite lunghezza risposta
#define MAX_ANSWER_LENGTH 64

/**
 * Tipi di messaggio
 * @param MSG_LOGIN Messaggio di login (richiesta nickname)
 * @param MSG_LOGIN_SUCCESS Risposta al login con successo
 * @param MSG_LOGIN_ERROR Risposta al login con errore
 * @param MSG_REQUEST_NICKNAME Richiesta di inserimento del nickname
 * @param MSG_NICKNAME_PROMPT Indica: "il payload contiene il prompt per l'inserimento del nickname"
 * @param MSG_REQUEST_QUESTION Richiesta di una nuova domanda
 * @param MSG_QUESTION Indica: "il payload contiene una domanda"
 * @param MSG_ANSWER Indica: "il payload contiene una risposta"
 * @param MSG_ANSWER_RESULT Risultato della risposta inviato al client
 * @param MSG_REQUEST_SCORE Richiesta di invio dei punteggi
 * @param MSG_SCORE               Indica: "il payload contiene i punteggi"
 * @param MSG_QUIZ_COMPLETED      Indica: "il payload contiene un messaggio di completamento" 
 * @param MSG_QUIZ_AVAILABLE      Indica: "il payload contiene la lista dei quiz disponibili"
 * @param MSG_TRIVIA_COMPLETED    Indica: "il payload contiene il messaggio di fine gioco"
 * @param MSG_END_QUIZ            Indica: "il payload contiene il messaggio di fine quiz"
 * @param MSG_DISCONNECT          Indica: "il payload contiene il messaggio di disconnessione"
 * @param MSG_ERROR               Indica: "il payload contiene un messaggio di errore"
 */
typedef enum {
    MSG_LOGIN = 1,
    MSG_LOGIN_SUCCESS,
    MSG_LOGIN_ERROR,
    MSG_REQUEST_NICKNAME,
    MSG_NICKNAME_PROMPT,
    MSG_REQUEST_QUESTION,
    MSG_QUESTION,
    MSG_ANSWER,
    MSG_ANSWER_RESULT,
    MSG_REQUEST_SCORE,
    MSG_SCORE,
    MSG_QUIZ_COMPLETED,
    MSG_QUIZ_AVAILABLE,
    MSG_TRIVIA_COMPLETED,
    MSG_END_QUIZ,
    MSG_DISCONNECT,
    MSG_ERROR,
} MessageType;

// Codici di stato/errore
#define SUCCESS 0
#define ERR_SOCKET -1
#define ERR_CONNECT -2
#define ERR_SEND -3
#define ERR_RECV -4

#endif