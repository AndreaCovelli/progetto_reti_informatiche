#ifndef SERVER_H
#define SERVER_H

#include <sys/select.h>
#include "common.h"
#include "player.h"
#include "quiz.h"
#include "debug.h"

/**
 * Struttura per mantenere lo stato del server
 * @param server_socket Socket del server
 * @param active_fds Set di descrittori attivi
 * @param read_fds Set di descrittori in lettura
 * @param max_fd Massimo indice tra i descrittori attivi
 * @param players Array di giocatori
 */
typedef struct {
    int server_socket;
    fd_set active_fds;
    fd_set read_fds;
    int max_fd;
    PlayerArray* players;
} ServerState;

/**
 * Struttura per mantenere lo stato del client
 * @param nickname Nickname del giocatore scelto dal client
 * @param current_quiz Numero del quiz attualmente selezionato (1 per sport, 2 per geografia)
 * @param current_question Numero della domanda corrente
 * @param is_playing Indica se il client è attualmente in partita
 */
typedef struct {
    char nickname[MAX_NICK_LENGTH];
    int current_quiz;
    int current_question;
    bool is_playing;
} ClientData;

// Funzioni server

/**
 * Inizializza il socket del server
 * @param state ServerState* struttura del server
 * @param ip indirizzo ip del server
 * @param port porta del server
 * @return true se l'inizializzazione ha successo, false altrimenti
 */
bool init_server_socket(ServerState* state, const char* ip, int port);

/**
 * Inizializza il server e le sue strutture dati
 * @param ip indirizzo ip del server    
 * @param port porta del server
 * @note Initializes file descriptor sets for select().
 * Inizializza read, write e error set
 * @return Restituisce la struttura ServerState inizializzata
 */
ServerState* init_server(const char* ip, int port);

/**
 * Inizializza i dati del client
 * @param client_socket socket del client
 * @note Inizializza i dati del client con valori di default 
 * e imposta is_playing = false per indicare che il client non è in partita
 */
void init_client_data(int client_socket);

/**
 * Pulisce le risorse allocate dal server
 * @param state ServerState* struttura del server

 * @note Chiude tutti i socket aperti
 */
void cleanup_server(ServerState* state);

/**
 * Aggiorna l'indice del massimo descrittore attivo (max_fd)
 * @param state ServerState* struttura del server
 */
void update_max_fd(ServerState* state);

/**
 * Gestisce una nuova connessione in arrivo
 * @param state ServerState* struttura del server
 */
void handle_new_connection(ServerState* state);

/**
 * Gestisce un messaggio ricevuto da un client
 * @param state ServerState* struttura del server
 * @param client_socket socket del client
 */
void process_client_message(ServerState* state, int client_socket);

/**
 * Invia un messaggio a tutti i client connessi
 * @param state ServerState* struttura del server
 * @param msg Message* messaggio da inviare
 * @note Usata per aggiornamenti di stato del server
 */
void broadcast_message(ServerState* state, Message* msg);

/**
 * Gestisce la disconnessione di un client
 * @param state ServerState* struttura del server
 * @param client_socket socket del client
 * @note Rimuove il client dai descrittori attivi
 */
void handle_disconnect(ServerState* state, int client_socket);

/**
 * Gestisce la chiusura del server
 * @note Invia un messaggio di disconnessione a tutti i client
 */
void handle_shutdown();

/**
 * Mostra lo stato del server
 * @param state ServerState* struttura del server
 */
void display_server_status(ServerState* state);

/**
 * Invia un prompt per il nickname al client
 * @param client_socket socket del client
 */
void send_nickname_prompt(int client_socket);

/**
 * Invia il messaggio con i quiz disponibili al client
 * con il nickname specificato
 * @param client_socket socket del client
 * @param nickname nickname del client
 */
void send_quiz_available_message(int client_socket, const char* nickname);

/**
 * Invia una domanda ad un client
 * @param client_socket socket del client
 * @param quiz Quiz* quiz corrente
 * @param question_num numero della domanda
 */
void send_question_to_client(int client_socket, Quiz* quiz, int question_num);

/**
 * Formatta i punteggi di tutti i giocatori in una stringa
 * @param state struttura ServerState contenente l'array dei giocatori
 * @return stringa formattata contenente tutti i punteggi
 */
char* format_scores(ServerState* state);

/**
 * @brief Gestisce l'aggiunta di un nuovo giocatore al server
 *
 * @param state Puntatore allo stato del server
 * @param client_socket Socket del client da gestire
 * @param nickname Nickname del nuovo giocatore
 * 
 * @return true se il giocatore è stato aggiunto con successo, false se non è stato
 * possibile aggiungere il giocatore
 */
bool handle_new_player(ServerState* state, int client_socket, const char* nickname);

/**
 * @brief Gestisce la logica per un giocatore che si sta riconnettendo al server
 *
 * La funzione gestisce il processo di autenticazione per un giocatore esistente,
 * verificando che non sia già connesso e controllando il suo stato di completamento dei quiz.
 *
 * @param state Puntatore allo stato del server
 * @param client_socket Socket del client che si sta connettendo
 * @param player Puntatore alla struttura del giocatore esistente
 * @param nickname Nickname del giocatore che sta tentando di connettersi
 *
 * @return true se il giocatore è stato riconnesso con successo,
 *         false se il giocatore è già connesso o ha completato tutti i quiz
 *
 * @note La funzione invia messaggi appropriati al client per informarlo dello stato
 *       della connessione e dei quiz disponibili
 */
bool handle_existing_player(ServerState* state, int client_socket, Player* player, const char* nickname);

/**
 * @brief Gestisce la richiesta di login da parte di un client
 * 
 * La funzione verifica se il nickname è già in uso e gestisce il processo di login
 * per un nuovo giocatore.
 * 
 * @param state Puntatore allo stato del server
 * @param client_socket Socket del client che sta tentando di connettersi
 * @param msg Messaggio di login inviato dal client
 * 
 * @return true se il login è avvenuto con successo, false se il nickname è già in uso
 * o il giocatore ha completato tutti i quiz
 */
void handle_login_request(ServerState* state, int client_socket, Message* msg);

/**
 * @brief Gestisce il passaggio alla domanda successiva nel gioco quiz
 *
 * Questa funzione incrementa il contatore della domanda corrente per il client e:
 * - Invia la domanda successiva al client se ci sono ancora domande disponibili
 * - Gestisce il completamento del quiz se tutte le domande sono state risposte
 *
 * @param state Puntatore alla struttura dello stato del server
 * @param client_socket Descrittore del socket per la comunicazione col client
 * @param client Puntatore alla struttura dati del client
 * 
 * @note Il tipo di quiz (sport o geografia) è determinato da client->current_quiz
 */
void handle_next_question(ServerState* state, int client_socket, ClientData* client);

/**
 * @brief Gestisce il completamento di un quiz da parte di un client
 * 
 * Questa funzione si occupa di:
 * - Aggiornare lo stato del giocatore marcando il quiz come completato
 * - Inviare il messaggio con i punteggi finali se tutti i quiz sono stati completati
 * - Inviare un messaggio con i quiz ancora disponibili se ce ne sono
 *
 * @param state Puntatore alla struttura che mantiene lo stato del server
 * @param client_socket Socket del client che ha completato il quiz
 * @param client Puntatore alla struttura contenente i dati del client
 * 
 * @note La funzione gestisce automaticamente la disconnessione del client in caso di errori di comunicazione
 */
void handle_quiz_completion(ServerState* state, int client_socket, ClientData* client);

/**
 * @brief Gestisce la selezione del quiz da parte del client
 *
 * Questa funzione gestisce la richiesta di un client di iniziare un quiz specifico.
 * Verifica se il quiz selezionato è già stato completato dal giocatore e in tal caso
 * invia un messaggio di errore. Altrimenti, inizializza lo stato del client per 
 * iniziare il nuovo quiz e invia la prima domanda.
 *
 * @param state Puntatore alla struttura ServerState contenente lo stato del server
 * @param client_socket Socket del client che ha fatto la richiesta
 * @param msg Puntatore al messaggio ricevuto dal client contenente la selezione del quiz
 *
 * @note Il quiz selezionato è codificato nel primo carattere del payload del messaggio:
 *       '1' per il quiz sport, '2' per il quiz geografia
 */
void handle_quiz_selection(ServerState* state, int client_socket, Message* msg);

/**
 * @brief Gestisce la risposta di un client ad una domanda del quiz
 * 
 * La funzione elabora la risposta del client, verifica se è corretta,
 * aggiorna il punteggio del giocatore e invia il risultato al client.
 * Successivamente, procede con la domanda successiva.
 * 
 * @param state Puntatore allo stato del server
 * @param client_socket Socket del client che ha inviato la risposta
 * @param msg Puntatore al messaggio contenente la risposta del client
 * 
 * @note La funzione controlla se il client sta effettivamente giocando
 * @note In caso di risposta corretta, incrementa il punteggio appropriato (sport o geografia)
 * @note In caso di errore nell'invio del risultato, disconnette il client
 */
void handle_answer(ServerState* state, int client_socket, Message* msg);

/**
 * @brief Carica i file dei quiz dal filesystem in memoria.
 * 
 * Questa funzione carica i file dei quiz di sport e geografia dai percorsi predefiniti:
 * - res/sport_quiz.txt per il quiz sportivo
 * - res/geography_quiz.txt per il quiz geografico
 * 
 * @return true se entrambi i file sono stati caricati con successo,
 *         false se si è verificato un errore nel caricamento di almeno un file
 * 
 * @note I quiz vengono caricati nelle variabili globali sport_quiz e geography_quiz
 */
bool load_quiz_files();

#endif