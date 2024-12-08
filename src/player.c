#include <stdlib.h>
#include <string.h>
#include "include/player.h"

PlayerArray* create_player_array(int initial_capacity) {
    PlayerArray* array = (PlayerArray*)malloc(sizeof(PlayerArray));
    if (!array) return NULL;

    array->players = (Player*)malloc(sizeof(Player) * initial_capacity);
    if (!array->players) {
        // Allocazione fallita, deallocazione e restituzione NULL
        free(array);
        return NULL;
    }

    array->count = 0;
    array->capacity = initial_capacity;
    return array;
}

void free_player_array(PlayerArray* array) {
    // Verifica che l'array non sia NULL
    if (array) {
        free(array->players);
        free(array);
    }
}

bool add_player(PlayerArray* array, const char* nickname) {
    if (!array || !nickname) return false;

    // Verifica che il giocatore non sia già presente
    if (find_player(array, nickname) != NULL) return false;

    if (array->count >= array->capacity) {
        int new_capacity = array->capacity * 2; // Raddoppia la capacità
        Player* new_players = (Player*)realloc(array->players, 
                                             sizeof(Player) * new_capacity);
        // Verifica che la riallocazione sia andata a buon fine
        if (!new_players) return false;
        
        array->players = new_players;
        array->capacity = new_capacity;
    }

    Player* new_player = &array->players[array->count];
    // MAX_NICK_LENGTH - 1 per garantire che ci sia spazio per il carattere null terminatore '\0'
    strncpy(new_player->nickname, nickname, MAX_NICK_LENGTH - 1);
    new_player->nickname[MAX_NICK_LENGTH - 1] = '\0';
    new_player->sport_score = 0;
    new_player->geography_score = 0;
    new_player->completed_sport = false;
    new_player->completed_geography = false;

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

static int compare_players_by_score(const void* a, const void* b, bool sport_quiz) {
    const Player* player_a = (const Player*)a;
    const Player* player_b = (const Player*)b;
    
    int score_a = sport_quiz ? player_a->sport_score : player_a->geography_score;
    int score_b = sport_quiz ? player_b->sport_score : player_b->geography_score;
    
    return score_b - score_a;
}

static int compare_sport_score(const void* a, const void* b) {
    return compare_players_by_score(a, b, true);
}

static int compare_geography_score(const void* a, const void* b) {
    return compare_players_by_score(a, b, false);
}

void sort_players_by_score(PlayerArray* array, bool sport_quiz) {
    if (!array || array->count <= 1) return;

    // Use qsort to sort the players
    qsort(array->players, array->count, sizeof(Player), 
          sport_quiz ? compare_sport_score : compare_geography_score);
}