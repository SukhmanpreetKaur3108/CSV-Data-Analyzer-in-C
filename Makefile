# ============================================================
# Makefile — CSV Data Analyzer
# Compiler: GCC (MinGW-w64) on Windows / GCC on Linux
# Build:   make
# Run:     make run
# Clean:   make clean
# ============================================================

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -g -Iinclude
LDFLAGS = -lm

# All source files
SRC = src/main.c \
      src/csv_parser.c \
      src/stats.c \
      src/display.c \
      src/utils.c

# Object files go into build/
OBJ = $(patsubst src/%.c, build/%.o, $(SRC))

# Output binary
TARGET = build/csv_analyzer

# ---- Default target ----
all: build_dir $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build successful: $(TARGET)"

# Compile each .c file to a .o inside build/
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it doesn't exist
build_dir:
	@mkdir -p build

# ---- Run with sample data ----
run: all
	./$(TARGET) data/sample.csv

# ---- Export run ----
run_export: all
	./$(TARGET) data/sample.csv -o build/summary.txt
	@echo "--- Exported output ---"
	@cat build/summary.txt

# ---- Clean build artifacts ----
clean:
	rm -f build/*.o build/csv_analyzer build/csv_analyzer.exe build/summary.txt
	@echo "Cleaned build directory"

.PHONY: all clean run run_export build_dir
