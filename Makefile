# Define our compiler and flags
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -g

# Define the source and object files
SRCS_C = src/lexer.c src/codegen.c
SRCS_CPP = src/main.cpp src/parser.cpp

OBJS_C = $(patsubst src/%.c, build/%.o, $(SRCS_C))
OBJS_CPP = $(patsubst src/%.cpp, build/%.o, $(SRCS_CPP))

OBJS = $(OBJS_C) $(OBJS_CPP)

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

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -f out out.cpp
