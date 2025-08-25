# Define our compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

# Define the source and object files
SRCS = src/main.c src/lexer.c src/parser.c src/banner.c
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

# Define the executable name and output directory
EXECUTABLE = build/zen
BUILD_DIR = build

.PHONY: all
all: $(BUILD_DIR) $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
