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

/**
 * Analizza una linea del file di quiz e la converte in una struttura Question.
 * Il formato atteso è "domanda | risposta". 
 * 
 * @param line stringa contenente la linea da analizzare
 * @param question puntatore alla struttura Question da popolare
 * @return true se il parsing è avvenuto con successo, false altrimenti
 * 
 * @note La funzione si aspetta che ci sia uno spazio prima e dopo il 
 *       carattere '|' nel formato "domanda | risposta"
 * @note La funzione modifica la stringa di input usando strtok()
 */
static bool parse_question_line(char* line, Question* question) {
    char* question_part = strtok(line, "|");
    char* answer_part = strtok(NULL, "|");
    
    if (!question_part || !answer_part) {
        return false;
    }

    // Rimuovi eventuali spazi iniziali e finali
    while (*question_part == ' ') question_part++;
    while (*answer_part == ' ') answer_part++;

    // Rimuovi eventuali newline dalla risposta
    char* newline = strchr(answer_part, '\n');
    if (newline) *newline = '\0';
    
    // Copia la domanda e la risposta nelle strutture
    strncpy(question->question, question_part, MAX_QUESTION_LENGTH - 1);
    strncpy(question->correct_answer, answer_part, MAX_ANSWER_LENGTH - 1);
    
    // Assicurati che le stringhe siano terminate correttamente
    question->question[MAX_QUESTION_LENGTH - 1] = '\0';
    question->correct_answer[MAX_ANSWER_LENGTH - 1] = '\0';
    
    return true;
}

void select_random_questions(Quiz* quiz) {
    if (!quiz || quiz->total_count == 0) return;
    
    // Liberiamo l'eventuale selezione precedente
    if (quiz->selected) {
        free(quiz->selected);
    }
    
    // Allochiamo memoria per le nuove domande
    quiz->selected = malloc(sizeof(Question) * QUESTIONS_PER_QUIZ);
    if (!quiz->selected) return;
    
    // Array per tenere traccia degli indici già selezionati
    bool* used = calloc(quiz->total_count, sizeof(bool));
    if (!used) {
        free(quiz->selected);
        quiz->selected = NULL;
        return;
    }
    
    quiz->selected_count = 0;

    DEBUG_PRINT("Selezionando %d domande casuali da %d disponibili",
                QUESTIONS_PER_QUIZ, quiz->total_count);

    // Seleziona QUESTIONS_PER_QUIZ domande casuali
    while (quiz->selected_count < QUESTIONS_PER_QUIZ) {
        int idx = rand() % quiz->total_count;
        // Se questa domanda non è già stata selezionata
        if (!used[idx]) {
            quiz->selected[quiz->selected_count] = quiz->questions[idx];
            used[idx] = true;
            quiz->selected_count++;
        }
    }
    
    free(used);
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

Question* get_question(Quiz* quiz, int question_num) {
    if (!quiz || !quiz->selected || question_num < 0 || 
        question_num >= quiz->selected_count) {
        return NULL;
    }
    return &quiz->selected[question_num];
}

bool check_answer(Quiz* quiz, int question_num, const char* answer) {
    if (!quiz || !quiz->selected || !answer || 
        question_num < 0 || question_num >= quiz->selected_count) {
        return false;
    }
    DEBUG_PRINT("Correct answer: %s\n", quiz->selected[question_num].correct_answer);
    // Confronta le stringhe ignorando maiuscole e minuscole
    return strcasecmp(quiz->selected[question_num].correct_answer, answer) == 0;
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