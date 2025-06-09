# Makefile for Memory Allocator Project
# Supports Windows with MinGW/GCC

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
INCLUDES = -Isrc/includes
SRCDIR = src
COREDIR = $(SRCDIR)/core
UTILSDIR = $(SRCDIR)/utils
TESTDIR = tests
BUILDDIR = build
BINDIR = bin

# Source files
CORE_SOURCES = $(COREDIR)/memory_allocator.cpp $(COREDIR)/buddy_allocator.cpp $(COREDIR)/slab_allocator.cpp $(COREDIR)/pool_allocator.cpp $(COREDIR)/hybrid_allocator.cpp
UTILS_SOURCES = $(wildcard $(UTILSDIR)/*.cpp)
TEST_SOURCES = $(wildcard $(TESTDIR)/*.cpp)

# Object files
CORE_OBJECTS = $(CORE_SOURCES:%.cpp=$(BUILDDIR)/%.o)
UTILS_OBJECTS = $(UTILS_SOURCES:%.cpp=$(BUILDDIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:%.cpp=$(BUILDDIR)/%.o)

# Main executable
MAIN_TARGET = $(BINDIR)/memory_allocator.exe
MAIN_SOURCE = $(SRCDIR)/main.cpp
MAIN_OBJECT = $(BUILDDIR)/$(SRCDIR)/main.o

# Test executables
TEST_TARGET = $(BINDIR)/run_tests.exe

# Default target
all: directories $(MAIN_TARGET)

# Create directories
directories:
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	@if not exist "$(BUILDDIR)/$(SRCDIR)" mkdir "$(BUILDDIR)/$(SRCDIR)"
	@if not exist "$(BUILDDIR)/$(COREDIR)" mkdir "$(BUILDDIR)/$(COREDIR)"
	@if not exist "$(BUILDDIR)/$(TESTDIR)" mkdir "$(BUILDDIR)/$(TESTDIR)"
	@if not exist "$(BINDIR)" mkdir "$(BINDIR)"

# Main executable
$(MAIN_TARGET): $(CORE_OBJECTS) $(MAIN_OBJECT)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built main executable: $@"

# Test executable  
$(TEST_TARGET): $(CORE_OBJECTS) $(UTILS_OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Built test executable: $@"

# Object file rules
$(BUILDDIR)/%.o: %.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Special targets
.PHONY: all clean test demo benchmark directories run help

# Run the main program
run: $(MAIN_TARGET)
	@echo "Running Memory Allocator Demo..."
	@$(MAIN_TARGET)

# Run tests
test: $(TEST_TARGET)
	@echo "Running unit tests..."
	@$(TEST_TARGET)

# Run benchmarks
benchmark: $(MAIN_TARGET)
	@echo "Running performance benchmarks..."
	@$(MAIN_TARGET) --benchmark

# Start demo server
demo:
	@echo "Starting web demo server..."
	@cd demo && python -m http.server 8080

# Clean build files
clean:
	@if exist "$(BUILDDIR)" rmdir /s /q $(BUILDDIR)
	@if exist "$(BINDIR)" rmdir /s /q $(BINDIR)
	@echo "Cleaned build files"

# Quick build (only buddy allocator for now)
quick: directories
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(COREDIR)/memory_allocator.cpp $(COREDIR)/buddy_allocator.cpp $(SRCDIR)/main.cpp -o $(BINDIR)/quick_test.exe
	@echo "Quick build complete: $(BINDIR)/quick_test.exe"

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Build main executable"
	@echo "  test       - Build and run tests"
	@echo "  run        - Build and run main program"
	@echo "  benchmark  - Run performance benchmarks"
	@echo "  demo       - Start web demo server"
	@echo "  quick      - Quick build for testing"
	@echo "  clean      - Remove build files"
	@echo "  help       - Show this help message"

# Dependencies (simplified)
$(BUILDDIR)/$(COREDIR)/buddy_allocator.o: $(SRCDIR)/includes/buddy_allocator.h $(SRCDIR)/includes/memory_allocator.h
$(BUILDDIR)/$(COREDIR)/memory_allocator.o: $(SRCDIR)/includes/memory_allocator.h
$(BUILDDIR)/$(SRCDIR)/main.o: $(SRCDIR)/includes/memory_allocator.h $(SRCDIR)/includes/buddy_allocator.h
