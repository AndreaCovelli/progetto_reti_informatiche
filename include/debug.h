/**
 * @file debug.h
 * @brief Fornisce funzionalità di debug configurabili tramite macro
 * 
 * Questo header fornisce strumenti per il debugging del codice attraverso
 * macro che possono essere abilitate o disabilitate in fase di compilazione.
 * 
 * Per abilitare il debug è sufficiente compilare con:
 * @code
 * make debug
 * @endcode
 * 
 * Questo comando definirà automaticamente la macro DEBUG che abilita i messaggi di debug.
 * Per la compilazione normale senza debug usare semplicemente 'make'.
 * 
 * @note I messaggi di debug vengono inviati su stderr
 * 
 * Esempio di utilizzo:
 * @code
 * DEBUG_PRINT("Valore della variabile x: %d", x);
 * @endcode
 * 
 * Se DEBUG non è definito, le macro si espandono a nulla (no-op),
 * quindi non c'è overhead in release mode (make classico).
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)
#else
    // Se DEBUG è definito, abilita i messaggi di debug.
    // Altrimenti, le macro si espandono a nulla.
    #define DEBUG_PRINT(fmt, ...) ((void)0)
#endif

#endif