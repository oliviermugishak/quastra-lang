# Define our compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -g

# Define the source and object files
SRCS = src/main.c src/lexer.c src/parser.c src/codegen.c
OBJS = $(patsubst src/%.c, build/%.o, $(SRCS))

# Define the executable name and output directory
EXECUTABLE = build/quastra
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
	rm -f out out.cpp
