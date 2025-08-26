# Makefile for the Quastra Compiler Project (System gtest version)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
INCLUDES = -I./src
LDFLAGS = -pthread
LIBS = -lgtest -lgtest_main

# Directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# VPATH tells 'make' where to look for source files.
# Corrected to include the root src directory for main.cpp
VPATH = $(shell find src src/lib tests src/lib/semantic -type d)

# --- Source Files ---
# Find all library and main source files automatically.
SOURCES = $(notdir $(shell find src/lib src/lib/semantic -name '*.cpp'))
MAIN_SOURCE = main.cpp

# Explicitly list all test source files for a more robust build.
TEST_SOURCES = \
    test_lexer.cpp \
    test_parser.cpp \
    test_interpreter.cpp \
    test_codegen.cpp \
    test_type_checker.cpp

# --- Object Files ---
OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))
MAIN_OBJECT = $(addprefix $(OBJ_DIR)/, $(MAIN_SOURCE:.cpp=.o))
TEST_OBJECTS = $(addprefix $(OBJ_DIR)/, $(TEST_SOURCES:.cpp=.o))

# --- Executables ---
COMPILER_EXECUTABLE = $(BIN_DIR)/quastra-compiler
TEST_EXECUTABLE = $(BIN_DIR)/run_tests

# Default target builds the compiler.
all: $(COMPILER_EXECUTABLE)

# --- Build Rules ---

# Rule to build the main compiler executable
$(COMPILER_EXECUTABLE): $(MAIN_OBJECT) $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Rule to build the test executable
$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)

# Generic rule to compile any .cpp file into an object file.
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# --- Utility Targets ---

# Target to run the tests
test: $(TEST_EXECUTABLE)
	@echo "🚀 Running tests..."
	@$(TEST_EXECUTABLE)

# Target to clean up build artifacts
clean:
	@echo "🧹 Cleaning up build files..."
	@rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all test clean
