#include <stdio.h>
#include <stdbool.h> // Include this header to use bool, true, and false
#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>

#include "rendering.h"
#include "utils.h"
#include "types/colors.h"
#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_settings.h"

/* Function implementations */

void printSnakeBody(int bodyIndex, int length, int cycle, bool matrixMode)
{
    if(matrixMode)
    {
        char c = getRandomChar();
        printf(ANSI_COLOR_GREEN );
        printf("%c", c);  
        printf(ANSI_COLOR_RESET);
    }
    else
    {    
        // We will add a little bit of styling to the snake, based on body's element and cycle
        if (bodyIndex > length - 10)
        {
            // tail //◘ •
            if(bodyIndex == length-2)
                printf(ANSI_COLOR_GREEN "◘" ANSI_COLOR_RESET);
            else if (bodyIndex % 2)
                printf(ANSI_COLOR_GREEN "•" ANSI_COLOR_RESET);
            else
                printf(ANSI_COLOR_CYAN "•" ANSI_COLOR_RESET);
        }
        else
        {
            //This means alternating + and ♦ elements with flashing colors
            if (bodyIndex % 2)
            {
                if (cycle % 2)
                {
                    printf(ANSI_COLOR_GREEN "+" ANSI_COLOR_RESET);
                }
                else
                {
                    printf(ANSI_COLOR_CYAN "+" ANSI_COLOR_RESET);
                }
            }
            else
            {
                if (cycle % 2)
                {
                    printf(ANSI_COLOR_CYAN "♦" ANSI_COLOR_RESET);
                }
                else
                {
                    printf(ANSI_COLOR_GREEN "♦" ANSI_COLOR_RESET);
                }
            }
        }   
    }
}

void printSnakeHead(int cycle, bool matrixMode)
{
    if( matrixMode )
    {
        char c = getRandomChar();
        printf( ANSI_COLOR_HIGREEN );
        printf( "%c", c );  
        printf( ANSI_COLOR_RESET );
    }
    else
    {
        if ( cycle % 2 )
            printf( ANSI_COLOR_GREEN "X" ANSI_COLOR_RESET );
        else
            printf( ANSI_COLOR_CYAN "@" ANSI_COLOR_RESET );
    }    
}

void printFood(int cycle, bool matrixMode)
{
    // Food will be flashing dollar sign (everybody likes dollar sign)
    const char *ANSI_COLOR_FOOD;
    
    if( matrixMode )
    {
        ANSI_COLOR_FOOD = ANSI_COLOR_RED;
        printf( "%sΘ%s", ANSI_COLOR_FOOD,ANSI_COLOR_RESET );
    }
    else
    {
        if( cycle % 2 )
            ANSI_COLOR_FOOD = ANSI_COLOR_RED;
        else
            ANSI_COLOR_FOOD = ANSI_COLOR_YELLOW;

        printf( "%s$%s", ANSI_COLOR_FOOD, ANSI_COLOR_RESET );
    }        
}

void printEmptyContent(int x, int y, int width, int depth, bool matrixMode)
{
    if (matrixMode)
    {        
        if (y == 1 || y == depth || x == 0 || x == width)
        {
            char c = '*';
            //char c = getRandomChar();
            printf("%c", c);
        }
        else
        {
            printf(" ");
        }            
    }
    else
    {
        if (y == 1 || y == depth)
        {
            if (x == 0 || x == width)
                printf("+");
            else
                printf("=");
        }
        else
        {
            if (x == 0 || x == width)
                printf("|");
            else
                printf(" ");
        }
    }
}

// Return true if food is present at given x,y position
bool checkFood(int x, int y, int foodX, int foodY)
{
    if (x == foodX && y == foodY)
        return true;
    else
        return false;
}

// Return true if snake head is present at given x,y position
bool checkHead(int x, int y, int headPositionX, int headPositionY)
{
    if (x == headPositionX && y == headPositionY)
        return true;
    else
        return false;
}

// Return index of snake's body element if present at given x,y position
// Otherwise return -1
int checkBody(int x, int y, int *xBody, int *yBody, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (x == xBody[i] && y == yBody[i])
        {
            return i;
        }
    }
    return -1;
}

void printContent(T_Game_State *gameState, bool matrixMode)
{
    int depth = gameState->rows - gameState->yOffset;
    int width = gameState->columns - 2;
    int bodyIndex = -1;
    bool hasHead = false;
    bool hasFood = false;

    //Screen printing is done line by line starting from top
    for (int y = 0; y <= depth; y++)
    {     
        printf("\n"); // Start at new line
        for (int x = 0; x <= width; x++)
        {
            if (y > 0)  //Not sure why this condition?
            {
                // 1. Check if the screen cell contains head,food or body
                hasHead = checkHead( x, y, gameState->headPositionX, gameState->headPositionY );              
                hasFood = checkFood( x, y, gameState->foodX, gameState->foodY );
                bodyIndex = checkBody( x, y, gameState->xBody, gameState->yBody, gameState->length );    

                // 2. Print appropriate element
                if ( hasHead )
                {                    
                    printSnakeHead( gameState->cycle, matrixMode );
                }                
                else if ( hasFood )
                {
                    printFood(gameState->cycle, matrixMode);
                }                
                else if ( bodyIndex >= 0 )
                {
                    printSnakeBody( bodyIndex, gameState->length, gameState->cycle, matrixMode);
                }
                else
                {
                    printEmptyContent( x, y, width, depth, matrixMode );
                }
            }
        }
        
    }    
    fflush(stdout);
}

// Upper line with informations
void printHeaderLine(T_Game_State *gameState, T_Game_Mode mode)
{       
    if(gameState->columns < 155)
    {
        //Make it responsive :)
        printMiniHeaderLine(mode, gameState->length);
    }
    else
    {    
        //TODO: IF HEAD X OR Y EQUALS TO FOODS -> THEN COLOR IN GREEN
        printMaxiHeaderLine(mode, gameState);
    }
}

void printMaxiHeaderLine(T_Game_Mode mode, T_Game_State *gameState)
{
    //TODO: add length after food
    const char *color = is_mode_active(mode, MATRIX) ? ANSI_COLOR_GREEN : ANSI_COLOR_BLUE;
    printf(
        "\r%s"
        "Terminal size: "
        "y:%d rows | "
        "x:%d columns | "
        "headPositionX: %d | "
        "headPositionY: %d | "
        "refresh rate: %d | "
        "foodX: %d | "
        "foodY: %d | "
        "length: %s%d "
        "    " // Empty space is needed
        ANSI_COLOR_RESET,
        color,  gameState->rows,  gameState->columns,  
        gameState->headPositionX,  gameState->headPositionY, 
        SETTINGS.millis,  gameState->foodX,  gameState->foodY, ANSI_COLOR_HIGREEN, gameState->length);
}

void printMiniHeaderLine(T_Game_Mode mode, int length)
{
    //TODO: add length after food
    if (is_mode_active(mode, MATRIX))
        printf("\r" ANSI_COLOR_GREEN "$: " ANSI_COLOR_HIGREEN "%d" ANSI_COLOR_RESET, length);
    else
        printf("\r" ANSI_COLOR_YELLOW "$: " ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET, length);
} 

void printGameOverScreen(int foodEaten, int length, long long int cycle)
{
    printf("\nGame Over\n");
    printf("Food eaten: %d \n", foodEaten);
    printf("Length achieved: %d \n", length);
    printf("Cycles played: %lld \n", cycle);
    printf("\e[?25h"); // Reenable cursor
}

void setWindowSize(T_Game_State *gameState)
{
    struct winsize w;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
    {
        perror("ioctl");
        return;
    }

    gameState->rows = w.ws_row;
    gameState->columns = w.ws_col;

    if(gameState->rowsPrev != gameState->rows || gameState->columnsPrev != gameState->columns)
    {
        system("clear");
    }

    gameState->rowsPrev = gameState->rows;
    gameState->columnsPrev = gameState->columns;
}

void resetCursorPosition(T_Game_State *gameState)
{
    setWindowSize(gameState);//update window size    

    printf("\r"); //go to start of the line

    for(int i = 0; i< gameState->rows; i++)
    {
        printf("\033[A");
    } 
}

void render(T_Game_State *gameState, T_Game_Mode mode)
{
    resetCursorPosition(gameState);    
    printHeaderLine(gameState, SETTINGS.millis);        
    printContent(gameState, is_mode_active( mode, MATRIX )); 
    
    if(!SETTINGS.cursorVisible) printf("\e[?25l"); // Remove cursor and flashing
}