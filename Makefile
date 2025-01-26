# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = bin/snake

# Object files
OBJS = src/snake.o src/lib/utils.o src/lib/rendering.o src/lib/types/game_mode.o src/lib/types/game_settings.o src/lib/types/game_state.o src/lib/engine.o

# Default target
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

# Compile source files into object files
src/snake.o: src/snake.c src/lib/utils.h src/lib/rendering.h src/lib/types/game_mode.h src/lib/types/game_direction.h src/lib/types/colors.h src/lib/types/game_settings.h src/lib/types/game_state.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/utils.o: src/lib/utils.c src/lib/utils.h src/lib/types/game_mode.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/rendering.o: src/lib/rendering.c src/lib/rendering.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/engine.o: src/lib/engine.c src/lib/engine.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/types/game_mode.o: src/lib/types/game_mode.c src/lib/types/game_mode.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/types/game_settings.o: src/lib/types/game_settings.c src/lib/types/game_settings.h
	$(CC) $(CFLAGS) -c $< -o $@

src/lib/types/game_state.o: src/lib/types/game_state.c src/lib/types/game_state.h
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove compiled files
clean:
	rm -f $(OBJS) $(TARGET)
