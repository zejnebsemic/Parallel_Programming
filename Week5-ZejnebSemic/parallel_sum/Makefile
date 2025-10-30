CC = gcc
CFLAGS = -Wall -std=c99 -O2
LDFLAGS = -lm

TARGET = parallel_sum

SOURCES = main.c sum_algorithms.c
HEADERS = sum_algorithms.h
OBJECTS = $(SOURCES:.c=.o)


all: $(TARGET)


$(TARGET): $(OBJECTS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)


%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	@echo "Running all tests (2^10 to 2^27)..."
	./$(TARGET)


test_quick: $(TARGET)
	@echo "Running quick test (2^20 to 2^24)..."
	@./$(TARGET) | grep -A 6 "2^2[0-4]"


clean:
	@echo "Cleaning up..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete!"


rebuild: clean all


help:
	@echo "Available targets:"
	@echo "  make          - Build the program (default)"
	@echo "  make run      - Build and run full test suite"
	@echo "  make clean    - Remove compiled files"
	@echo "  make rebuild  - Clean and rebuild from scratch"
	@echo "  make test_quick - Run limited tests for debugging"
	@echo ""
	@echo "Compiler:  $(CC)"
	@echo "Flags:     $(CFLAGS)"
	@echo "Linker:    $(LDFLAGS)"
	@echo "Target:    $(TARGET)"
	@echo ""
	@echo "The program tests precision and performance of"
	@echo "five summation algorithms over array sizes from"
	@echo "2^10 (1,024) to 2^27 (134,217,728) elements."

.PHONY: all clean run test_quick rebuild help
