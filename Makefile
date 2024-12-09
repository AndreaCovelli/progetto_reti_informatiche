# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I.
# oppure CFLAGS = -Wall -Wextra  # Se modifichiamo i file sorgente

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

# Source files
CLIENT_SRC = $(SRC_DIR)/client.c
SERVER_SRC = $(SRC_DIR)/server.c
COMMON_SRCS = $(filter-out $(CLIENT_SRC) $(SERVER_SRC), $(wildcard $(SRC_DIR)/*.c))

# Object files
CLIENT_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(CLIENT_SRC))
SERVER_OBJ = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SERVER_SRC))
COMMON_OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(COMMON_SRCS))

# Final executables
CLIENT = client
SERVER = server

# All target
all: $(OBJ_DIR) $(CLIENT) $(SERVER)

# Create obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link client
$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJS)
	$(CC) $^ -o $@

# Link server
$(SERVER): $(SERVER_OBJ) $(COMMON_OBJS)
	$(CC) $^ -o $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(CLIENT) $(SERVER)

# Dependencies
-include $(OBJ_DIR)/*.d

.PHONY: all clean