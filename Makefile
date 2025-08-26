# Makefile for the Quastra Compiler Project

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
VPATH = $(shell find src -type d) tests

# --- Source Files ---
# Find all library source files
LIB_SOURCES = $(notdir $(shell find src/lib -name '*.cpp'))
# The main compiler driver source
MAIN_SOURCE = main.cpp
# All test source files
TEST_SOURCES = $(notdir $(shell find tests -name '*.cpp'))

# --- Object Files ---
LIB_OBJECTS = $(addprefix $(OBJ_DIR)/, $(LIB_SOURCES:.cpp=.o))
MAIN_OBJECT = $(addprefix $(OBJ_DIR)/, $(MAIN_SOURCE:.cpp=.o))
TEST_OBJECTS = $(addprefix $(OBJ_DIR)/, $(TEST_SOURCES:.cpp=.o))

# --- Executables ---
COMPILER_EXECUTABLE = $(BIN_DIR)/quastra-compiler
TEST_EXECUTABLE = $(BIN_DIR)/run_tests

# Default target builds the compiler.
all: $(COMPILER_EXECUTABLE)

# --- Build Rules ---

# Rule to build the main compiler executable
$(COMPILER_EXECUTABLE): $(MAIN_OBJECT) $(LIB_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Rule to build the test executable
$(TEST_EXECUTABLE): $(LIB_OBJECTS) $(TEST_OBJECTS)
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
