# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I.
DEBUGFLAGS = -g -DDEBUG

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
CLIENT_DEBUG = client_debug
SERVER_DEBUG = server_debug

# All target
all: $(OBJ_DIR) $(CLIENT) $(SERVER)

# Debug target
debug: CFLAGS += $(DEBUGFLAGS)
debug: $(OBJ_DIR) $(CLIENT_DEBUG) $(SERVER_DEBUG)

# Documentation
docs:
	doxygen Doxyfile
	@echo "Documentation generated in docs/html"

# Create obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Link client (release)
$(CLIENT): $(CLIENT_OBJ) $(COMMON_OBJS)
	$(CC) $^ -o $@

# Link server (release)
$(SERVER): $(SERVER_OBJ) $(COMMON_OBJS)
	$(CC) $^ -o $@

# Link client (debug)
$(CLIENT_DEBUG): $(CLIENT_OBJ) $(COMMON_OBJS)
	$(CC) $(DEBUGFLAGS) $^ -o $@

# Link server (debug)
$(SERVER_DEBUG): $(SERVER_OBJ) $(COMMON_OBJS)
	$(CC) $(DEBUGFLAGS) $^ -o $@

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJ_DIR) $(CLIENT) $(SERVER) $(CLIENT_DEBUG) $(SERVER_DEBUG) docs/

# Dependencies
-include $(OBJ_DIR)/*.d

.PHONY: all clean debug docs