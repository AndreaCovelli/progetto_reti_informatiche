/*
    Implementazione del server del Trivia Game
*/

#include "include/server.h"
#include "include/common.h"


int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        fprintf(stderr, "Utilizzo: %s <IP address> <port>\n", argv[0]);
        return 1;
    }

}