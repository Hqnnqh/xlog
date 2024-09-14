CC = gcc
CFLAGS = -Wall -Iinclude -g
LDFLAGS = -lX11 -lpthread

# Update the paths to source files
HOST_SRC = $(wildcard src/host/*.c)
CLIENT_SRC = $(wildcard src/client/*.c)

HOST_OBJ = $(patsubst src/host/%.c, build/host/%.o, $(HOST_SRC))
CLIENT_OBJ = $(patsubst src/client/%.c, build/client/%.o, $(CLIENT_SRC))

HOST_BIN = bin/xlog_host
CLIENT_BIN = bin/xlog_client

.PHONY: all clean host client

# Default target: compile both binaries
all: $(HOST_BIN) $(CLIENT_BIN)

# Compile host binary
$(HOST_BIN): $(HOST_OBJ)
	mkdir -p bin
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile client binary
$(CLIENT_BIN): $(CLIENT_OBJ)
	mkdir -p bin
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files into object files for host
build/host/%.o: src/host/%.c
	mkdir -p build/host
	$(CC) $(CFLAGS) -o $@ -c $<

# Compile source files into object files for client
build/client/%.o: src/client/%.c
	mkdir -p build/client
	$(CC) $(CFLAGS) -o $@ -c $<

# Clean command to remove binaries and object files
clean:
	rm -rf bin/ build/

# Targets to build only host or client
host: $(HOST_BIN)
client: $(CLIENT_BIN)
