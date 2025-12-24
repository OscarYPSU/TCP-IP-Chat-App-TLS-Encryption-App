# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Libraries
LIBS = -lssl -lcrypto

# Source files
SERVER_SRC = Server.c
CLIENT_SRC = Client.c

# Executables
SERVER_EXE = server
CLIENT_EXE = client

# Default target: build both server and client
all: $(SERVER_EXE) $(CLIENT_EXE)

# Build server
$(SERVER_EXE): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER_EXE) $(LIBS)

# Build client
$(CLIENT_EXE): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT_EXE) $(LIBS)

# Clean up executables
clean:
	rm -f $(SERVER_EXE) $(CLIENT_EXE)
