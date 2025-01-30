/*
 * score.c
 * Implementazione della gestione dei punteggi per 'Trivia Quiz Multiplayer'
 * 
 * Questo file contiene l'implementazione delle funzioni per la formattazione
 * e la visualizzazione dei punteggi dei giocatori. Gestisce la creazione
 * di report dettagliati che includono i punteggi per ogni categoria di quiz
 * e lo stato di completamento dei quiz per ogni giocatore.
 */

#include "include/score.h"
#include <stdio.h>
#include <string.h>

/**
 * Formatta la sezione dei partecipanti in una stringa
 * @param state puntatore allo stato del server
 * @param buffer buffer di output
 * @param offset offset attuale del buffer
 * @note Aggiunge i nomi dei partecipanti al buffer.
 * @note Se non ci sono partecipanti, aggiunge un messaggio di avviso.
 */
static void format_participants_section(const ServerState* state, char* buffer, int* offset) {
    *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                       "\nPartecipanti (%d):\n", state->players->count);
    
    if(state->players->count == 0) {
        *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                         "Nessun giocatore presente\n");
        return;
    }
    
    for (int i = 0; i < state->players->count; i++) {
        *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                        "- %s\n", state->players->players[i].nickname);
    }
}

/**
 * Ordina i giocatori in base al punteggio
 * @param players array di giocatori
 * @param buffer buffer di output
 * @param offset offset attuale del buffer
 * @param is_sport_quiz true se si vuole ordinare i giocatori
 * per il punteggio del quiz sullo sport, false per il quiz sulla geografia
 * @note Utilizza qsort per ordinare i giocatori in base al punteggio.
 */
static void format_quiz_scores(const ServerState* state, char* buffer, int* offset, 
                             bool is_sport_quiz) {
    const char* quiz_name = is_sport_quiz ? "Sport" : "Geografia";
    *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                      "\nPunteggio %s:\n", quiz_name);

    sort_players_by_score(state->players, is_sport_quiz);
    bool has_scores = false;

    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        int score = is_sport_quiz ? p->sport_score : p->geography_score;
        
        if (score >= 0) {
            *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                             "- %s: %d\n", p->nickname, score);
            has_scores = true;
        }
    }

    if (!has_scores) {
        *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                         "Nessun giocatore ha ancora partecipato\n");
    }
}

/**
 * Formatta la sezione dei quiz completati in una stringa
 * @param state puntatore allo stato del server
 * @param buffer buffer di output
 * @param offset offset attuale del buffer
 * @param is_sport_quiz true se si vuole formattare la sezione per il quiz sullo sport
 * @note Aggiunge i nomi dei giocatori che hanno completato il quiz al buffer.
 * @note Se nessun giocatore ha completato il quiz, aggiunge una semplice nota.
 */
static void format_completed_quiz_section(const ServerState* state, char* buffer, 
                                        int* offset, bool is_sport_quiz) {
    const char* quiz_name = is_sport_quiz ? "Sport" : "Geografia";
    *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                      "\nQuiz %s completato da:\n", quiz_name);

    bool has_completed = false;

    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if ((is_sport_quiz && p->completed_sport) || 
            (!is_sport_quiz && p->completed_geography)) {
            *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                             "- %s\n", p->nickname);
            has_completed = true;
        }
    }

    if (!has_completed) {
        *offset += snprintf(buffer + *offset, MAX_MSG_LEN - *offset, 
                         "Nessun giocatore ha completato questo quiz\n");
    }
}

char* format_scores(ServerState* state) {
    static char score_buffer[MAX_MSG_LEN];
    int offset = 0;

    format_participants_section(state, score_buffer, &offset);
    format_quiz_scores(state, score_buffer, &offset, true);  // Sport
    format_quiz_scores(state, score_buffer, &offset, false); // Geografia
    format_completed_quiz_section(state, score_buffer, &offset, true);  // Sport
    format_completed_quiz_section(state, score_buffer, &offset, false); // Geografia

    return score_buffer;
}