#ifndef COMMON_H
#define COMMON_H
#include <stdbool.h>
#include "include/constants.h"

/**
 * Player infos
 */
typedef struct {
    char nickname[MAX_NICK_LENGTH];
    int tech_score;
    int general_score;
    bool completed_tech;
    bool completed_general;
} Player;

/**
 * Dynamic array to store and manage multiple players
 */
typedef struct {
    Player* players;
    int count;
    int capacity;
} PlayerArray;

/**
 * Create a new player array
 * @return PlayerArray*
 */
PlayerArray* create_player_array(int initial_capacity);

/**
 * Free a player array
 * @param array PlayerArray* to free
 * @return void
 */
void free_player_array(PlayerArray* array);

/**
 * Add a player to the array
 * @param array PlayerArray* to add the player to
 * @param nickname const char* nickname of the player
 * @return bool true if the player was added, false otherwise
 */
bool add_player(PlayerArray* array, const char* nickname);

/**
 * Remove a player from the array
 * @param array PlayerArray* to remove the player from
 * @param nickname const char* nickname of the player
 * @return bool true if the player was removed, false otherwise
 */
bool remove_player(PlayerArray* array, const char* nickname);

/**
 * Find a player in the array
 * @param array PlayerArray* to search in
 * @param nickname const char* nickname of the player
 * @return Player* pointer to the player if found, NULL otherwise
 */
Player* find_player(PlayerArray* array, const char* nickname);

/**
 * Sort the players in the array by score
 * @param array PlayerArray* to sort
 * @param tech_quiz bool true if the players are sorted by tech score, false if they are sorted by general score
 * @return void
 */
void sort_players_by_score(PlayerArray* array, bool tech_quiz);

#endif