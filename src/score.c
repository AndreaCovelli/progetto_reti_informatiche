#include "include/score.h"
#include <stdio.h>
#include <string.h>

char* format_scores(ServerState* state) {
    static char score_buffer[MAX_MSG_LEN];
    int offset = 0;

    // Sezione Partecipanti
    offset += snprintf(score_buffer, MAX_MSG_LEN, 
                      "\nPartecipanti:\n");
    for (int i = 0; i < state->players->count; i++) {
        offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                         "- %s\n", state->players->players[i].nickname);
    }

    // Sezione Sport
    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nPunteggio Sport:\n");
    sort_players_by_score(state->players, true);
    bool has_sport_scores = false;
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->sport_score >= 0) {  // Mostra anche punteggi 0
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s: %d\n", p->nickname, p->sport_score);
            has_sport_scores = true;
        }
    }
    if (!has_sport_scores) {
        offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                         "Nessun giocatore ha ancora partecipato\n");
    }

    // Sezione Geografia
    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nPunteggio Geografia:\n");
    sort_players_by_score(state->players, false);
    bool has_geo_scores = false;
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->geography_score >= 0) {  // Mostra anche punteggi 0
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s: %d\n", p->nickname, p->geography_score);
            has_geo_scores = true;
        }
    }
    if (!has_geo_scores) {
        offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                         "Nessun giocatore ha ancora partecipato\n");
    }

    // Quiz completati Sport
    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nQuiz Sport completato da:\n");
    bool has_completed_sport = false;
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->completed_sport) {
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s\n", p->nickname);
            has_completed_sport = true;
        }
    }
    if (!has_completed_sport) {
        offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                         "Nessun giocatore ha completato questo quiz\n");
    }

    // Quiz completati Geografia
    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nQuiz Geografia completato da:\n");
    bool has_completed_geo = false;
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->completed_geography) {
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s\n", p->nickname);
            has_completed_geo = true;
        }
    }
    if (!has_completed_geo) {
        offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                         "Nessun giocatore ha completato questo quiz\n");
    }

    return score_buffer;
}