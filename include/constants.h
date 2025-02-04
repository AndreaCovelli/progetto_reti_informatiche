// File contenente le costanti
#ifndef CONSTANTS_H
#define CONSTANTS_H

// Limiti di sistema
#define MAX_CLIENTS (FD_SETSIZE - 4)  // -4 per socket listener e stdin/stdout/stderr
// Capacità iniziale dell'array di giocatori
#define INITIAL_PLAYER_ARRAY_SIZE 10
// Numero massimo di giocatori
#define MAX_PLAYERS 1020

// Limiti buffer
#define BUFFER_SIZE 1024
#define LINE_BUFFER 256

// Limite lunghezza messaggio
#define MAX_MSG_LEN 512
// Limite lunghezza nickname
#define MAX_NICK_LENGTH 20 + 1
// Limite numero di domande per quiz
#define QUESTIONS_PER_QUIZ 5
// Limite numero di domande per partita
#define QUESTIONS_PER_GAME (QUESTIONS_PER_QUIZ*2)
// Limite numero massimo di possibili risposte corrette per domanda
#define MAX_CORRECT_ANSWERS 5
// Limite numero di domande caricate
#define MAX_QUESTIONS 100
// Limite lunghezza domanda
#define MAX_QUESTION_LENGTH 128
// Limite lunghezza risposta
#define MAX_ANSWER_LENGTH 64

/**
 * Tipi di messaggio
 */
typedef enum {
    MSG_LOGIN = 1,                 // Client richiede di effettuare il login
    MSG_LOGIN_SUCCESS,            // Server conferma login avvenuto con successo
    MSG_LOGIN_ERROR,             // Server notifica errore durante il login
    MSG_REQUEST_NICKNAME,        // Client richiede di impostare un nickname
    MSG_NICKNAME_PROMPT,         // Server invia prompt per inserimento nickname
    MSG_REQUEST_QUESTION,        // Client richiede una nuova domanda
    MSG_QUESTION,               // Server invia una domanda al client
    MSG_ANSWER,                 // Client invia la risposta alla domanda al server
    MSG_ANSWER_RESULT,          // Server invia il risultato della risposta del client
    MSG_REQUEST_SCORE,          // Client richiede il punteggio attuale
    MSG_SCORE,                  // Server invia i punteggi
    MSG_QUIZ_COMPLETED,         // Server notifica completamento del quiz
    MSG_QUIZ_AVAILABLE,         // Server invia lista dei quiz disponibili
    MSG_TRIVIA_COMPLETED,       // Server notifica fine del gioco completo
    MSG_END_QUIZ,              // Server notifica fine del quiz corrente
    MSG_DISCONNECT,            // Client/Server notifica disconnessione
    MSG_ERROR,                // Server notifica un errore generico
} MessageType;

// Codici di stato/errore

#define SUCCESS 0 // Operazione completata con successo
#define ERR_SOCKET -1 // Errore durante la creazione del socket
#define ERR_CONNECT -2 // Errore durante la connessione al server
#define ERR_SEND -3 // Errore durante l'invio dei dati
#define ERR_RECV -4 // Errore durante la ricezione dei dati

#endif