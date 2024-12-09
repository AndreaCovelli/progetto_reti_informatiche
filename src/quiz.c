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

Quiz* load_quiz(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    Quiz* quiz = (Quiz*)malloc(sizeof(Quiz));
    if (!quiz) {
        fclose(file);
        return NULL;
    }

    // Inizializza l'array di domande
    quiz->questions = (Question*)malloc(sizeof(Question) * MAX_QUESTIONS);
    if (!quiz->questions) {
        free(quiz);
        fclose(file);
        return NULL;
    }

    quiz->count = 0;

    // Estrai il nome del tema dalla prima riga
    char line[LINE_BUFFER];
    if (fgets(line, sizeof(line), file)) {
        // Rimuovi il newline se presente
        char* newline = strchr(line, '\n');
        if (newline) 
            *newline = '\0';
        
        strncpy(quiz->topic, line, sizeof(quiz->topic) - 1);
        quiz->topic[sizeof(quiz->topic) - 1] = '\0';
    }

    // Leggi le domande
    while (fgets(line, sizeof(line), file) && quiz->count < MAX_QUESTIONS) {
        // Rimuovi il newline se presente
        char* newline = strchr(line, '\n');
        if (newline) *newline = '\0';
        
        if (parse_question_line(line, &quiz->questions[quiz->count])) {
            quiz->count++;
        }
    }

    fclose(file);

    if (quiz->count == 0) {
        free(quiz->questions);
        free(quiz);
        return NULL;
    }

    return quiz;
}

void free_quiz(Quiz* quiz) {
    if (quiz) {
        free(quiz->questions);
        free(quiz);
    }
}

Question* get_question(Quiz* quiz, int question_num) {
    if (!quiz || question_num < 0 || question_num >= quiz->count) {
        return NULL;
    }
    return &quiz->questions[question_num];
}

bool check_answer(Quiz* quiz, int question_num, const char* answer) {
    if (!quiz || !answer || question_num < 0 || question_num >= quiz->count) {
        return false;
    }

    // Case insensitive comparison
    return strcasecmp(quiz->questions[question_num].correct_answer, answer) == 0;
}

int get_question_count(Quiz* quiz) {
    return quiz ? quiz->count : 0;
}