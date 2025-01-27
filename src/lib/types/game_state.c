#include "game_state.h"

#include <stdio.h>
#include <stdlib.h>

// Function to initialize the game state
void init_game_state(T_Game_State *gameState, int initial_length) 
{
    gameState->cycle = 0;

    // Screen size (default values, can be set later)
    gameState->rows = 0;
    gameState->columns = 0;
    gameState->rowsPrev = 0;
    gameState->columnsPrev = 0;

    // Positioning
    gameState->xOffset = 0;
    gameState->yOffset = 2;

    gameState->headPositionX = 21;
    gameState->headPositionY = 22;

    // Snake tail position
    gameState->length = initial_length; // Initial snake length
    gameState->xBody = (int *)calloc(gameState->length, sizeof(int));
    gameState->yBody = (int *)calloc(gameState->length, sizeof(int));

    if (!gameState->xBody || !gameState->yBody) 
    {
        fprintf(stderr, "Error: Memory allocation failed!\n");
        exit(EXIT_FAILURE);
    }

    // Initialize tail positions (example: line from head backward)
    for (int i = 0; i < gameState->length; ++i) {
        gameState->xBody[i] = gameState->headPositionX - i; // Horizontal snake body
        gameState->yBody[i] = gameState->headPositionY;    // Same row
    }

    // Food position
    gameState->foodX = 0;
    gameState->foodY = 0;

    // Other game gameState variables
    gameState->foodEaten = 0;
    gameState->pausa = false;

    gameState->direction = RIGHT;
    gameState->mode = NORMAL;

    gameState->timeToFlash = 0;
}

// Function to free allocated memory
void cleanup_game_state(T_Game_State *gameState) 
{
    free(gameState->xBody);
    free(gameState->yBody);
    gameState->xBody = NULL;
    gameState->yBody = NULL;
}