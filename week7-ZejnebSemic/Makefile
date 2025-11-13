CC = gcc
CFLAGS = -fopenmp -Wall -std=c11 -march=native -O2
LDFLAGS = -lm

# Stencil source files (main programs)
SOURCES = example_06.c stencil_opt4.c stencil_opt6.c

# Generate executable names
EXECUTABLES = $(SOURCES:.c=)

# Utility object files
UTILS = timer.o malloc2D.o

# Default target: build all executables
all: $(EXECUTABLES)
	@echo "All stencil executables built successfully!"

# Generic rule: compile any stencil .c file into executable
%: %.c $(UTILS)
	@echo "Building $@..."
	$(CC) $(CFLAGS) $< $(UTILS) -o $@ $(LDFLAGS)

# Compile timer utility
timer.o: timer.c timer.h
	@echo "Compiling timer..."
	$(CC) $(CFLAGS) -c timer.c -o $@

# Compile malloc2D utility
malloc2D.o: malloc2D.c malloc2D.h
	@echo "Compiling malloc2D..."
	$(CC) $(CFLAGS) -c malloc2D.c -o $@

# Run all programs
run: all
	@echo "Running example_06..."
	./example_06
	@echo "Running stencil_opt4..."
	./stencil_opt4
	@echo "Running stencil_opt6..."
	./stencil_opt6

# Clean up
clean:
	@echo "Cleaning up..."
	rm -f $(EXECUTABLES) $(UTILS)

.PHONY: all clean run
