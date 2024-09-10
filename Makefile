CC = gcc
CFLAGS = -Wall -Iinclude -g
LDFLAGS = -lX11 -lpthread
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))
BIN = bin/xlog

.PHONY: all clean

all: $(BIN)

# Link object files to create the final binary
$(BIN): $(OBJ)
	mkdir -p bin
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files into object files in the build directory
build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

# Clean command to remove binaries and object files
clean:
	rm -rf bin/ build/
