#include "include/quiz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool parse_question_line(char* line, Question* question) {
    char* question_part = strtok(line, "|");
    char* answer_part = strtok(NULL, "|");
    
    if (!question_part || !answer_part) {
        return false;
    }

    // Rimuovi eventuali spazi iniziali e finali
    while (*question_part == ' ') question_part++;
    while (*answer_part == ' ') answer_part++;
    
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
    
    // Alloca memoria per le domande selezionate
    quiz->selected = malloc(sizeof(Question) * QUESTIONS_PER_GAME);
    if (!quiz->selected) return;
    
    // Array per tenere traccia degli indici già selezionati
    bool* used = calloc(quiz->total_count, sizeof(bool));
    if (!used) {
        free(quiz->selected);
        return;
    }
    
    quiz->selected_count = 0;
    
    // Seleziona QUESTIONS_PER_GAME domande casuali
    while (quiz->selected_count < QUESTIONS_PER_GAME) {
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
    
    // Seleziona domande casuali per questa partita
    select_random_questions(quiz);
    
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