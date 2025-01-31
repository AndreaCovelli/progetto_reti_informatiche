#ifndef QUIZ_H
#define QUIZ_H

#include "player.h"
#include "common.h"

/**
 * Struttura di una domanda
 * @param question Testo della domanda
 * @param correct_answer Risposta corretta
 */
typedef struct {
    char question[MAX_QUESTION_LENGTH];
    char correct_answer[MAX_ANSWER_LENGTH];
} Question;

/**
 * Struttura di un quiz
 * @param questions Array di tutte le domande disponibili
 * @param selected Array delle domande selezionate
 * @param total_count Numero di domande
 * @param selected_count Numero di domande selezionate
 * @param topic Nome del tema del quiz
 * @note Si utilizza un array dinamico per le domande
 * perché il numero di domande può variare
 */
typedef struct {
    Question* questions;
    Question* selected; 
    int total_count;
    int selected_count;
    char topic[50];
} Quiz;

// Funzioni di gestione quiz e file
/**
 * Carica un quiz da file
 * @param filename Nome del file
 * @return Quiz* puntatore al quiz caricato
 */
Quiz* load_quiz(const char* filename);

/**
 * Libera la memoria allocata per il quiz
 * @param quiz Quiz* da liberare
 */
void free_quiz(Quiz* quiz);

/**
 * Restituisce la domanda di un quiz dato l'indice
 * @param quiz Quiz* quiz
 * @param question_num Numero della domanda
 * @return Question* domanda
 */
Question* get_question_by_index(Quiz* quiz, int question_index);

/**
 * Controlla la risposta data dall'utente
 * @param quiz Quiz* quiz
 * @param question_num Numero della domanda
 * @param answer Risposta data dall'utente
 * @return true se la risposta è corretta, false altrimenti
 */
bool check_answer(Quiz* quiz, int question_num, const char* answer);

/**
 * Restituisce il numero di domande del quiz
 * @param quiz Quiz* quiz
 * @return int, numero di domande
 */
int get_question_count(Quiz* quiz);

/**
 * Seleziona domande casuali per il quiz
 * @param quiz Quiz* quiz
 * @note L'inizializzazione del generatore di numeri casuali
 * avviene nel main() del server
 */
bool select_random_indices(Quiz* quiz, int* indices);

#endif