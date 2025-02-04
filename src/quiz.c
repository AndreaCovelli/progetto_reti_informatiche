/*
 * quiz.c
 * Implementazione della gestione dei quiz per 'Trivia Quiz Multiplayer'
 * 
 * Questo file contiene l'implementazione delle funzioni per gestire i quiz,
 * incluso il caricamento delle domande da file, la selezione casuale delle
 * domande, la verifica delle risposte e la gestione della memoria associata
 * alle strutture dei quiz.
 */

#include "include/quiz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/**
 * Normalizza una stringa eliminando tutti i caratteri di spaziatura.
 * @param input La stringa in ingresso.
 * @param output Il buffer in cui salvare la stringa normalizzata.
 *               Si assume che output abbia sufficiente spazio.
 */
static void normalize_string(const char* input, char* output) {
    while (*input) {
        if (!isspace((unsigned char)*input)) {
            *output = *input;
            output++;
        }
        input++;
    }
    *output = '\0';
}

/**
 * Analizza una linea del file di quiz e la converte in una struttura Question.
 * Il formato atteso è "domanda | risposta". 
 * 
 * @param line stringa contenente la linea da analizzare
 * @param question puntatore alla struttura Question da popolare
 * @return true se il parsing è avvenuto con successo, false altrimenti
 * 
 * @note La funzione si aspetta che ci sia uno spazio prima e dopo il 
 *       carattere '|' nel formato "domanda | risposte corrette", con le risposte
 *       separate da ciascuna da una virgola.
 * @note La funzione modifica la stringa di input usando strtok()
 */
static bool parse_question_line(char* line, Question* question) {
    // Suddivide la riga in due parti: domanda e risposte
    char* question_part = strtok(line, "|");
    char* answers_part = strtok(NULL, "|");
    
    if (!question_part || !answers_part) {
        return false;
    }

    // Rimuove i leading spaces
    while (*question_part == ' ') question_part++;
    while (*answers_part == ' ') answers_part++;
    
    // Rimuove eventuali newline nelle risposte
    char* newline = strchr(answers_part, '\n');
    if (newline) *newline = '\0';
    
    strncpy(question->question, question_part, MAX_QUESTION_LENGTH - 1);
    question->question[MAX_QUESTION_LENGTH - 1] = '\0';

    question->num_correct = 0;

    // Suddividiamo le risposte utilizzando la virgola come delimitatore
    char* token = strtok(answers_part, ",");
    while (token != NULL && question->num_correct < MAX_CORRECT_ANSWERS) {
        char normalized[MAX_ANSWER_LENGTH];
        normalize_string(token, normalized);
        
        strncpy(question->correct_answers[question->num_correct], normalized, MAX_ANSWER_LENGTH - 1);
        question->correct_answers[question->num_correct][MAX_ANSWER_LENGTH - 1] = '\0';

        question->num_correct++;
        token = strtok(NULL, ",");
    }

    // Se non è stata trovata alcuna risposta, il parsing fallisce
    if (question->num_correct == 0) {
        return false;
    }

    return true;
}

bool select_random_indices(Quiz* quiz, int* indices) {
    if (!quiz || !indices || quiz->total_count == 0) {
        return false;
    }
    
    // Liberiamo eventuali selezioni precedenti
    memset(indices, 0, sizeof(int) * QUESTIONS_PER_QUIZ);
    
    // Array per tenere traccia degli indici già selezionati
    bool* used = calloc(quiz->total_count, sizeof(bool));
    if (!used) {
        return false;
    }
    
    int selected_count = 0;
    
    // Selezioniamo QUESTIONS_PER_QUIZ domande casuali
    while (selected_count < QUESTIONS_PER_QUIZ) {
        int idx = rand() % quiz->total_count;
        if (!used[idx]) {
            indices[selected_count++] = idx;
            used[idx] = true;
        }
    }
    
    free(used);
    return true;
}

Quiz* load_quiz(const char* filename) {
    FILE* file = fopen(filename, "r");
    DEBUG_PRINT("Caricamento del quiz dal file: %s", filename);
    if (!file) return NULL;
    
    Quiz* quiz = malloc(sizeof(Quiz));
    if (!quiz) {
        fclose(file);
        return NULL;
    }
    
    // Inizializza con valori di default
    quiz->questions = malloc(sizeof(Question) * MAX_QUESTIONS);
    quiz->selected = NULL;
    quiz->total_count = 0;
    quiz->selected_count = 0;
    
    if (!quiz->questions) {
        free(quiz);
        fclose(file);
        return NULL;
    }
    
    // Leggi il tema
    char line[LINE_BUFFER];
    if (fgets(line, sizeof(line), file)) {
        char* newline = strchr(line, '\n');
        if (newline) *newline = '\0';
        strncpy(quiz->topic, line, sizeof(quiz->topic) - 1);
        quiz->topic[sizeof(quiz->topic) - 1] = '\0';
    }
    
    // Leggi tutte le domande disponibili
    while (fgets(line, sizeof(line), file) && quiz->total_count < MAX_QUESTIONS) {
        if (parse_question_line(line, &quiz->questions[quiz->total_count])) {
            quiz->total_count++;
        }
    }
    
    fclose(file);
    
    if (quiz->total_count == 0) {
        free(quiz->questions);
        free(quiz);
        return NULL;
    }
    
    DEBUG_PRINT("Caricate %d domande dal quiz: %s", quiz->total_count, quiz->topic);

    return quiz;
}

Question* get_question_by_index(Quiz* quiz, int question_index) {
    if (!quiz || question_index < 0 || question_index >= quiz->total_count) {
        return NULL;
    }
    return &quiz->questions[question_index];
}

bool check_answer(Quiz* quiz, int question_index, const char* answer) {
    if (!quiz || !answer || question_index < 0 || question_index >= quiz->total_count) {
        return false;
    }
    
    char normalized_user[MAX_ANSWER_LENGTH];
    normalize_string(answer, normalized_user);
    
    Question* q = &quiz->questions[question_index];
    for (int i = 0; i < q->num_correct; i++) {
        DEBUG_PRINT("Confronto: utente '%s' con corretta '%s'\n", normalized_user, q->correct_answers[i]);
        
        if (strcasecmp(normalized_user, q->correct_answers[i]) == 0) {
            return true;
        }
    }

    // Se neanche una corrisponde, restituiamo false
    return false;
}

int get_question_count(Quiz* quiz) {
    return quiz ? quiz->selected_count : 0;
}

void free_quiz(Quiz* quiz) {
    if (quiz) {
        free(quiz->questions);
        free(quiz->selected);
        free(quiz);
    }
}