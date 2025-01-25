//Standard libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h> // Include this header to use boolean, true, and false
#include <string.h>

//Our libraries
#include "lib/types/game_mode.h"
#include "lib/types/game_direction.h"
#include "lib/types/game_settings.h"
#include "lib/types/game_state.h"
#include "lib/types/game_mode.h"
#include "lib/types/colors.h"
#include "lib/utils.h"
#include "lib/rendering.h"

// Variables
long long int cycle = 0;
int rows = 0;
int columns = 0;
int rowsPrev = 0;
int columnsPrev = 0;

int yOffset = 2;
int xOffset = 0;

int *yBody;
int *xBody;

int length = 14; //tested max with 1115
int foodEaten = 0;

bool pausa = false;

//TODO: INCORPORATE IN GAME_STATE
T_Game_Mode mode = NORMAL;
T_Game_Direction direction = RIGHT; 

void cleanUp()
{
    disableNonCanonicalMode();
    free(yBody);
    free(xBody);
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

int handleKeypress()
{
    // Check for keyboard input
    int ch = readKeyPress();
    if (ch != EOF)
    {
        switch (ch) 
        {
            case 'a': 
            case 'A':
                if (direction != 'R' || is_mode_active(mode, GOD))
                    direction = 'L';
                break;

            case 'd': 
            case 'D':
                if (direction != 'L' || is_mode_active(mode, GOD))
                    direction = 'R';
                break;

            case 'w': 
            case 'W':
                if (direction != 'D' || is_mode_active(mode, GOD))
                    direction = 'U';
                break;

            case 's': 
            case 'S':
                if (direction != 'U' || is_mode_active(mode, GOD))
                    direction = 'D';
                break;

            case 'p': 
            case 'P':
                pausa = !pausa;
                break;

            case 'm':
            case 'M':
                if( !is_mode_active(mode, MATRIX) )
                {
                    mode = add_mode(mode, MATRIX);
                }
                else
                {
                    mode = remove_mode(mode, MATRIX);
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

void setWindowSize()
{
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
        perror("ioctl");
        return;
    }
    rows = w.ws_row;
    columns = w.ws_col;

    if(rowsPrev != rows || columnsPrev != columns){
        system("clear");
    }
    rowsPrev = rows;
    columnsPrev = columns;
}

void generateFood(T_Game_State *gameState)
{
    //Food is generated randomly
    gameState->foodX = (rand() % (columns - 4) + 2);
    gameState->foodY = (rand() % (rows - 4) + 2);
}

void initBody()
{  
    yBody = (int *)calloc(SETTINGS.maxLength, sizeof(int));
    xBody = (int *)calloc(SETTINGS.maxLength, sizeof(int));
    
    // Initialize array
    for (int i = 0; i < SETTINGS.maxLength; i++)
    {
        yBody[i] = 0;
        xBody[i] = 0;
    }    
}

void initialize(T_Game_State *gameState)
{
    
/*     initState();
 */
    system("clear");
    
    // Initial size print and frame
    // This is important to get rows and columns    
    setWindowSize();

    if(columns < 7 || rows < 5 )
    {
        printf("\nScreen is too small to play snake :/ \nPlease ensure at least 5 rows height and 7 columns width (found %i r, %i c).\n",rows,columns);
        exit(0);
    }

    gameState->headPositionX = columns/4;
    gameState->headPositionY = rows/2;

    initBody();
    generateFood(gameState);

    enableNonCanonicalMode();
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Set input to non-blocking mode
}

void resetCursorPosition()
{
    setWindowSize();//update window size    

    printf("\r"); //go to start of the line

    for(int i = 0; i< rows; i++)
    {
        printf("\033[A");
    } 
}

// Return true if heads position is collided with one of the body elements
bool detectBodyCollision(T_Game_State *gameState)
{
    for (int i = 0; i < length; i++)
    {
        if (gameState->headPositionX == xBody[i] && gameState->headPositionY == yBody[i])
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

void layEgg(T_Game_State *gameState)
{    
    //Food is actually layed egg from snake's tail
    gameState->foodX = xBody[length-1];
    gameState->foodY = yBody[length-1];
}

void eat(T_Game_State *gameState)
{
    if( is_mode_active( mode, LAYING_EGGS ))
    {
        layEgg(gameState);
    }
    else
    {        
        do
        {
            generateFood(gameState);
        } 
        while ( checkBody( gameState->foodX, gameState->foodY, xBody, yBody, length) > -1);
    }

    length += SETTINGS.bodyIncrement;
    foodEaten++;
}

void gameOver()
{
    disableNonCanonicalMode();
    free(yBody);
    free(xBody);
    printGameOverScreen(foodEaten, length, cycle);
    exit(0);
}

void autoPlay(T_Game_State *gameState)
{
    int z = (rand() % (4) + 0);

    switch (z)
    {
    case 0:
        gameState->headPositionX--;
        if (gameState->headPositionX < 1)
            gameState->headPositionX = columns - 2;
        break;

    case 1:
        gameState->headPositionX++;
        if (gameState->headPositionX > columns - 1)
            gameState->headPositionX = 1;
        break;

    case 2:
        gameState->headPositionY--;
        if (gameState->headPositionY < 1)
            gameState->headPositionY = rows - 2;
        break;

    case 3:
        gameState->headPositionY++;
        if (gameState->headPositionY > rows - 1)
            gameState->headPositionY = 1;
        break;
    
    default:
        break;
    }
}

void updateHeadPosition(T_Game_State *gameState)
{
    if ( is_mode_active( mode, AUTO ) && ( cycle % 100 ))
    {
        autoPlay(gameState);
    }
    else
    {
        //TODO: EXTRACT FUNCITON
        switch (direction) {
            case 'L':
                gameState->headPositionX--;
                if (gameState->headPositionX < 1)
                    gameState->headPositionX = columns - 3;
                break;

            case 'R':
                gameState->headPositionX++;
                if (gameState->headPositionX > columns - 3)
                    gameState->headPositionX = 1;
                break;

            case 'U':
                gameState->headPositionY--;
                if (gameState->headPositionY < yOffset)
                    gameState->headPositionY = rows - 3;
                break;

            case 'D':
                gameState->headPositionY++;
                if (gameState->headPositionY > rows - yOffset - 1)
                    gameState->headPositionY = yOffset;
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
    for (int i = length - 1; i > 0; i--)
    {
        xBody[i] = xBody[i-1];
        yBody[i] = yBody[i-1];
    }

    xBody[0] = gameState->headPositionX;
    yBody[0] = gameState->headPositionY;
}

void updateSnakeData(T_Game_State *gameState)
{
    // Update body before head, because it must follow the heads previous position
    updateBodyPosition(gameState);

    // Update head position based on current direction
    updateHeadPosition(gameState);

    if( !is_mode_active( mode, GOD ) && detectBodyCollision(gameState) )
    {
        gameOver();        
    }    

    if ( detectEating(gameState) )
    {
        eat(gameState);
        updateBodyPosition(gameState);        
    }
}

void render(T_Game_State *gameState)
{
    resetCursorPosition();    
    printHeaderLine(columns, mode, length, rows, gameState->headPositionX, gameState->headPositionY, SETTINGS.millis, gameState->foodX, gameState->foodY);    
    
    printContent(rows, columns, yOffset, length, cycle, gameState->headPositionX, gameState->headPositionY, gameState->foodX, gameState->foodY, xBody, yBody, is_mode_active( mode, MATRIX )); //New problem: Too many arguments?
    
    if(!SETTINGS.cursorVisible) printf("\e[?25l"); // Remove cursor and flashing
}

void refreshScreen(T_Game_State *gameState)
{
    if(!pausa)
    {
        updateSnakeData(gameState);
    }   
    render(gameState);  
}


int main(int argc, char **argv)
{
    T_Game_State gameState;

    // Initialize with a default length of 14
    initialize_game_state(&gameState, 14);


    if(argc > 0)
        mode = processArguments(argc,argv, mode);    
    

    initialize(&gameState);
        
    while (1)
    {
        cycle++;
        if(!handleKeypress())
            break;
        refreshScreen(&gameState);        
        usleep(SETTINGS.millis * 1000); // Sleep for defined milliseconds
    }

    cleanUp();
    printGameOverScreen(foodEaten, length, cycle);

    return 0;
}