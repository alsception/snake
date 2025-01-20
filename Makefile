# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = bin/snake

# Object files
OBJS = src/snake.o src/lib/utils.o src/lib/rendering.o

# Default target
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

# Compile source files into object files
src/snake.o: src/snake.c src/lib/utils.h src/lib/rendering.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/utils.o: src/lib/utils.c src/lib/utils.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/rendering.o: src/lib/rendering.c src/lib/rendering.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove compiled files
clean:
	rm -f $(OBJS) $(TARGET)
