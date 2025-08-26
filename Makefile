# Makefile for the Quastra Compiler Project (System gtest version)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g # Use C++17, enable all warnings, and include debug symbols
INCLUDES = -I./src # Corrected include path to point to the 'src' directory
LDFLAGS = -pthread # Linker flags, pthread is required by gtest
LIBS = -lgtest -lgtest_main # Link against the system-installed gtest libraries

# Directories
SRC_DIR = src/lib/frontend
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.cpp)

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
TEST_OBJECTS = $(patsubst $(TEST_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(TEST_SOURCES))

# Executable name
TEST_EXECUTABLE = $(BIN_DIR)/run_tests

# Default target
all: $(TEST_EXECUTABLE)

# Rule to build the test executable
$(TEST_EXECUTABLE): $(OBJECTS) $(TEST_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS) $(LIBS)
	@echo "✅ Test executable created at $(TEST_EXECUTABLE)"

# Rule to compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to compile test files into object files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
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
