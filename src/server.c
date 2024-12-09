/*
    Implementazione del server del Trivia Game
*/

#include "include/server.h"

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 2) {
        fprintf(stderr, "Utilizzo: %s <port>\n", argv[0]);
        return 1;
    }

    fd_set master;
    fd_set read_fds;

    

}