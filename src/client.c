/*
    Implementazione del client del Trivia Game
*/

#include "include/client.h"
#include "include/common.h"

int main(int argc, char *argv[]){
    if (argc != 3)
    {
        fprintf(stderr, "Utilizzo: %s <server IP address> <port>\n", argv[0]);
        return 1;
    }

    
}