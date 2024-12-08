#include <stdlib.h>
#include <string.h>
#include "include/common.h"

PlayerArray* create_player_array(int initial_capacity) {
    PlayerArray* array = (PlayerArray*)malloc(sizeof(PlayerArray));
    if (!array) return NULL;

    array->players = (Player*)malloc(sizeof(Player) * initial_capacity);
    if (!array->players) {
        free(array);
        return NULL;
    }

    array->count = 0;
    array->capacity = initial_capacity;
    return array;
}

void free_player_array(PlayerArray* array) {
    if (array) {
        free(array->players);
        free(array);
    }
}

bool add_player(PlayerArray* array, const char* nickname) {
    if (!array || !nickname) return false;

    if (find_player(array, nickname) != NULL) return false;

    if (array->count >= array->capacity) {
        int new_capacity = array->capacity * 2;
        Player* new_players = (Player*)realloc(array->players, 
                                             sizeof(Player) * new_capacity);
        if (!new_players) return false;
        
        array->players = new_players;
        array->capacity = new_capacity;
    }

    Player* new_player = &array->players[array->count];
    strncpy(new_player->nickname, nickname, MAX_NICK_LENGTH - 1);
    new_player->nickname[MAX_NICK_LENGTH - 1] = '\0';
    new_player->tech_score = 0;
    new_player->general_score = 0;
    new_player->completed_tech = false;
    new_player->completed_general = false;

    array->count++;
    return true;
}

bool remove_player(PlayerArray* array, const char* nickname) {
    if (!array || !nickname) return false;

    for (int i = 0; i < array->count; i++) {
        if (strcmp(array->players[i].nickname, nickname) == 0) {
            if (i < array->count - 1) {
                array->players[i] = array->players[array->count - 1];
            }
            array->count--;
            return true;
        }
    }
    return false;
}

Player* find_player(PlayerArray* array, const char* nickname) {
    if (!array || !nickname) return NULL;

    for (int i = 0; i < array->count; i++) {
        if (strcmp(array->players[i].nickname, nickname) == 0) {
            return &array->players[i];
        }
    }
    return NULL;
}

static int compare_players_by_score(const void* a, const void* b, bool tech_quiz) {
    const Player* player_a = (const Player*)a;
    const Player* player_b = (const Player*)b;
    
    int score_a = tech_quiz ? player_a->tech_score : player_a->general_score;
    int score_b = tech_quiz ? player_b->tech_score : player_b->general_score;
    
    return score_b - score_a;
}

static int compare_tech_score(const void* a, const void* b) {
    return compare_players_by_score(a, b, true);
}

static int compare_general_score(const void* a, const void* b) {
    return compare_players_by_score(a, b, false);
}

void sort_players_by_score(PlayerArray* array, bool tech_quiz) {
    if (!array || array->count <= 1) return;

    qsort(array->players, array->count, sizeof(Player), 
          tech_quiz ? compare_tech_score : compare_general_score);
}