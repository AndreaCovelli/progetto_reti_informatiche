#include "include/score.h"
#include <stdio.h>
#include <string.h>

char* format_scores(ServerState* state) {
    static char score_buffer[MAX_MSG_LEN];
    int offset = 0;

    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nPunteggi attuali:\n");

    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nPunteggio Curiosità sulla tecnologia:\n");
    sort_players_by_score(state->players, true);
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->sport_score > 0) {
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s: %d\n", p->nickname, p->sport_score);
        }
    }

    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nPunteggio Cultura Generale:\n");
    sort_players_by_score(state->players, false);
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->geography_score > 0) {
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s: %d\n", p->nickname, p->geography_score);
        }
    }

    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nQuiz Curiosità sulla tecnologia completato da:\n");
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->completed_sport) {
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s\n", p->nickname);
        }
    }

    offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                      "\nQuiz Cultura Generale completato da:\n");
    for (int i = 0; i < state->players->count; i++) {
        Player* p = &state->players->players[i];
        if (p->completed_geography) {
            offset += snprintf(score_buffer + offset, MAX_MSG_LEN - offset, 
                             "- %s\n", p->nickname);
        }
    }

    return score_buffer;
}