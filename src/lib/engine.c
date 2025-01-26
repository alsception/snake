#include <stdio.h>
#include <stdbool.h> // Include this header to use bool, true, and false
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "engine.h"
#include "rendering.h"
#include "utils.h"
#include "types/colors.h"
#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_settings.h"

void finalizeInitialization(T_Game_State *gameState)
{
    system("clear");
    
    // Initial size print and frame
    // This is important to get rows and columns    
    setWindowSize(gameState);

    if(gameState->columns < 7 || gameState->rows < 5 )
    {
        printf("\nScreen is too small to play snake :/ \nPlease ensure at least 5 rows height and 7 columns width (found %i r, %i c).\n",gameState->rows,gameState->columns);
        exit(0);
    }

    gameState->headPositionX = gameState->columns/4;
    gameState->headPositionY = gameState->rows/2;

    initBody(gameState);
    generateFood(gameState);

    enableNonCanonicalMode();
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Set input to non-blocking mode
}

void initBody(T_Game_State *gameState)
{  
    gameState->yBody = (int *)calloc(SETTINGS.maxLength, sizeof(int));
    gameState->xBody = (int *)calloc(SETTINGS.maxLength, sizeof(int));    
}

void generateFood(T_Game_State *gameState)
{
    //Food is generated randomly
    gameState->foodX = (rand() % (gameState->columns - 4) + 2);
    gameState->foodY = (rand() % (gameState->rows - 4) + 2);
}

// Function to read keypresses including arrow keys
int readKeyPress() 
{
    int ch = fgetc(stdin);

    // Check if it's an escape sequence
    if (ch == '\x1b') { // Escape character
        if (fgetc(stdin) == '[') { // Skip the '[' character
            ch = fgetc(stdin); // Read the third character
            // We will mask arrow keys to WASD, that way we can use both
            switch (ch) {
                case 'A': return 'W'; // Up arrow
                case 'B': return 'S'; // Down arrow
                case 'C': return 'D'; // Right arrow
                case 'D': return 'A'; // Left arrow
                default: return -1; // Unknown escape sequence
            }
        }
    }
    return ch; // Return the actual character if it's not an arrow key
}

int handleKeypress(T_Game_State *gameState)
{
    // Check for keyboard input
    int ch = readKeyPress();
    if (ch != EOF)
    {
        switch (ch) 
        {
            case 'a': 
            case 'A':
                if (gameState->direction != 'R' || is_mode_active(gameState->mode, GOD))
                    gameState->direction = 'L';
                break;

            case 'd': 
            case 'D':
                if (gameState->direction != 'L' || is_mode_active(gameState->mode, GOD))
                    gameState->direction = 'R';
                break;

            case 'w': 
            case 'W':
                if (gameState->direction != 'D' || is_mode_active(gameState->mode, GOD))
                    gameState->direction = 'U';
                break;

            case 's': 
            case 'S':
                if (gameState->direction != 'U' || is_mode_active(gameState->mode, GOD))
                    gameState->direction = 'D';
                break;

            case 'p': 
            case 'P':
                gameState->pausa = !gameState->pausa;
                break;

            case 'm':
            case 'M':
                if( !is_mode_active(gameState->mode, MATRIX) )
                {
                    gameState->mode = add_mode(gameState->mode, MATRIX);
                }
                else
                {
                    gameState->mode = remove_mode(gameState->mode, MATRIX);
                }                
                break;

            case 'q': 
            case 'Q':
                return 0; // Quit the game

            default:
                break;
        }
    }
    return 1;
}

void updateHeadPosition(T_Game_State *gameState)
{
    if ( is_mode_active( gameState->mode, AUTO ) && ( gameState->cycle % 100 ))
    {
        autoPlay(gameState);
    }
    else
    {
        //TODO: EXTRACT FUNCITON
        switch (gameState->direction) {
            case 'L':
                gameState->headPositionX--;
                if (gameState->headPositionX < 1)
                    gameState->headPositionX = gameState->columns - 3;
                break;

            case 'R':
                gameState->headPositionX++;
                if (gameState->headPositionX > gameState->columns - 3)
                    gameState->headPositionX = 1;
                break;

            case 'U':
                gameState->headPositionY--;
                if (gameState->headPositionY < gameState->yOffset)
                    gameState->headPositionY = gameState->rows - 3;
                break;

            case 'D':
                gameState->headPositionY++;
                if (gameState->headPositionY > gameState->rows - gameState->yOffset - 1)
                    gameState->headPositionY = gameState->yOffset;
                break;

            default:
                break;
        }
    }
}

void updateBodyPosition(T_Game_State *gameState)
{
    //Every element (i) of the snakes body takes the x,y coordinates of the previous element (i-1)
    //And first body element takes head's position
    for (int i = gameState->length - 1; i > 0; i--)
    {
        gameState->xBody[i] = gameState->xBody[i-1];
        gameState->yBody[i] = gameState->yBody[i-1];
    }

    gameState->xBody[0] = gameState->headPositionX;
    gameState->yBody[0] = gameState->headPositionY;
}

// Return true if heads position is collided with one of the body elements
bool detectBodyCollision(T_Game_State *gameState)
{
    for (int i = 0; i < gameState->length; i++)
    {
        if (gameState->headPositionX == gameState->xBody[i] && gameState->headPositionY == gameState->yBody[i])
        {
            return true;
        }
    }
    return false;
}

bool detectEating(T_Game_State *gameState)
{
    //Why this separate function for 1 expression?
    //Because maybe in the future we would want to detect eating by some other condition
    return gameState->headPositionX == gameState->foodX && gameState->headPositionY == gameState->foodY;
}

void eat(T_Game_State *gameState)
{
    if( is_mode_active( gameState->mode, LAYING_EGGS ))
    {
        layEgg(gameState);
    }
    else
    {        
        do
        {
            generateFood(gameState);
        } 
        while ( checkBody( gameState->foodX, gameState->foodY, gameState->xBody, gameState->yBody, gameState->length) > -1);
    }

    gameState->length += SETTINGS.bodyIncrement;
    gameState->foodEaten++;
}

void layEgg(T_Game_State *gameState)
{    
    //Food is actually layed egg from snake's tail
    gameState->foodX = gameState->xBody[gameState->length-1];
    gameState->foodY = gameState->yBody[gameState->length-1];
}

void autoPlay(T_Game_State *gameState)
{
    int z = (rand() % (4) + 0);

    switch (z)
    {
    case 0:
        gameState->headPositionX--;
        if (gameState->headPositionX < 1)
            gameState->headPositionX = gameState->columns - 2;
        break;

    case 1:
        gameState->headPositionX++;
        if (gameState->headPositionX > gameState->columns - 1)
            gameState->headPositionX = 1;
        break;

    case 2:
        gameState->headPositionY--;
        if (gameState->headPositionY < 1)
            gameState->headPositionY = gameState->rows - 2;
        break;

    case 3:
        gameState->headPositionY++;
        if (gameState->headPositionY > gameState->rows - 1)
            gameState->headPositionY = 1;
        break;
    
    default:
        break;
    }
}

void updateSnakeData(T_Game_State *gameState)
{
    // Update body before head, because it must follow the heads previous position
    updateBodyPosition(gameState);

    // Update head position based on current direction
    updateHeadPosition(gameState);

    if( !is_mode_active( gameState->mode, GOD ) && detectBodyCollision(gameState) )
    {
        gameOver(gameState);        
    }    

    if ( detectEating(gameState) )
    {
        eat(gameState);
        updateBodyPosition(gameState);        
    }
}

void gameOver(T_Game_State *gameState)
{
    disableNonCanonicalMode();
    free(gameState->yBody);
    free(gameState->xBody);
    printGameOverScreen(gameState->foodEaten, gameState->length, gameState->cycle);
    exit(0);
}