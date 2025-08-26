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
# This is corrected to search all subdirectories of src/lib AND tests.
VPATH = $(shell find src/lib tests -type d)

# Source files are now found automatically in all subdirectories of src/lib.
SOURCES = $(notdir $(shell find src/lib -name '*.cpp'))
TEST_SOURCES = $(notdir $(shell find tests -name '*.cpp'))

# Object files
OBJECTS = $(addprefix $(OBJ_DIR)/, $(SOURCES:.cpp=.o))
TEST_OBJECTS = $(addprefix $(OBJ_DIR)/, $(TEST_SOURCES:.cpp=.o))

# Executable name
TEST_EXECUTABLE = $(BIN_DIR)/run_tests

# Default target
all: $(TEST_EXECUTABLE)

# Rule to build the test executable
$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "✅ Test executable created at $(TEST_EXECUTABLE)"

# Generic rule to compile any .cpp file into an object file.
# The VPATH directive will ensure 'make' finds the source file.
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

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
