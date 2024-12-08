CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

# Source files
CLIENT_SRC = src/client.c
SERVER_SRC = src/server.c
COMMON_SRCS = $(filter-out $(CLIENT_SRC) $(SERVER_SRC), $(wildcard src/*.c))

# Object files
CLIENT_OBJ = $(CLIENT_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o)
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

# Targets
CLIENT = client
SERVER = server

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJS)
	$(CC) $^ -o $@

$(SERVER): $(SERVER_OBJ) $(COMMON_OBJS)
	$(CC) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(CLIENT) $(SERVER)

.PHONY: all clean