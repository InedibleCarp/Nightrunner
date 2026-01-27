# ========================================================
#  Simple raylib Makefile for macOS + Homebrew (Apple Silicon / Intel)
# ========================================================

# Name of the final executable
TARGET := game

# Source files (add more .cpp files here later if needed)
SOURCES := dasher.cpp

# Compiler — clang++ is the macOS default and works great with raylib
CXX := clang++
# or use g++ if you prefer → CXX := g++

# Build mode: change to DEBUG if you want symbols + no optimization
BUILD_MODE ?= RELEASE

# raylib install paths from Homebrew (Apple Silicon)
RAYLIB_INCLUDE := /opt/homebrew/include
RAYLIB_LIB     := /opt/homebrew/lib

# Compiler flags
CXXFLAGS := -Wall -std=c++14 -I$(RAYLIB_INCLUDE)

ifeq ($(BUILD_MODE),DEBUG)
    CXXFLAGS += -g -O0
else
    CXXFLAGS += -O2 -DNDEBUG
endif

# Linker flags & libraries
LDFLAGS := -L$(RAYLIB_LIB) -lraylib \
           -framework OpenGL \
           -framework IOKit \
           -framework CoreVideo \
           -framework Cocoa

# Object files (automatically from sources)
OBJECTS := $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

# Compile each .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(TARGET) *.o

# Phony targets
.PHONY: all clean

# Quick help
help:
	@echo "Usage:"
	@echo "  make              → build release version"
	@echo "  make BUILD_MODE=DEBUG → build debug version"
	@echo "  make clean        → remove build files"
