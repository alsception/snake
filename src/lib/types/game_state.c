#include "game_state.h"

#include <stdio.h>
#include <stdlib.h>

// Function to initialize the game state
void initialize_game_state(T_Game_State *state, int initial_length) 
{
    state->cycle = 0;

    // Screen size (default values, can be set later)
    state->rows = 0;
    state->columns = 0;
    state->rowsPrev = 0;
    state->columnsPrev = 0;

    // Positioning
    state->xOffset = 0;
    state->yOffset = 2;

    state->headPositionX = 21;
    state->headPositionY = 22;

    // Snake tail position
    state->length = initial_length; // Initial snake length
    state->xBody = (int *)calloc(state->length, sizeof(int));
    state->yBody = (int *)calloc(state->length, sizeof(int));
    if (!state->xBody || !state->yBody) {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    // Initialize tail positions (example: line from head backward)
    for (int i = 0; i < state->length; ++i) {
        state->xBody[i] = state->headPositionX - i; // Horizontal snake body
        state->yBody[i] = state->headPositionY;    // Same row
    }

    // Food position
    state->foodX = 0;
    state->foodY = 0;

    // Other game state variables
    state->foodEaten = 0;
    state->pausa = false;
}

// Function to free allocated memory
void cleanup_game_state(T_Game_State *state) 
{
    free(state->xBody);
    free(state->yBody);
    state->xBody = NULL;
    state->yBody = NULL;
}