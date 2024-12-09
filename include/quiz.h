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
 * @param questions Array di domande
 * @param count Numero di domande
 * @param topic Nome del tema del quiz
 * @note Si utilizza un array dinamico per le domande
 */
typedef struct {
    Question* questions;
    int count;
    char topic[50];
} Quiz;

// Funzioni di gestione quiz e file
/**
 * Carica un quiz da file
 * @param filename Nome del file
 */
Quiz* load_quiz(const char* filename);

/**
 * Libera la memoria allocata per il quiz
 * @param quiz Quiz* da liberare
 */
void free_quiz(Quiz* quiz);

/**
 * Restituisce una domanda del quiz
 * @param quiz Quiz* quiz
 * @param question_num Numero della domanda
 * @return Question* domanda
 */
Question* get_question(Quiz* quiz, int question_num);

/**
 * Controlla la risposta data dall'utente
 * @param quiz Quiz* quiz
 * @param question_num Numero della domanda
 * @param answer Risposta data dall'utente
 * @return true se la risposta è corretta, false altrimenti
 */
bool check_answer(Quiz* quiz, int question_num, const char* answer);

#endif