#include <stdio.h>
#include <stdbool.h> // Include this header to use bool, true, and false
#include "utils.h"

// Colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_HIGREEN "\e[1;92m"

//Function implementations

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
    }
    else
    {
        if( cycle % 2 )
            ANSI_COLOR_FOOD = ANSI_COLOR_RED;
        else
            ANSI_COLOR_FOOD = ANSI_COLOR_YELLOW;
    }    
    
    printf( "%s$%s", ANSI_COLOR_FOOD, ANSI_COLOR_RESET );
}

void printEmptyContent(int x, int y, int width, int depth, bool matrixMode)
{
    if (matrixMode)
    {        
        if (y == 1 || y == depth || x == 0 || x == width)
        {
            char c = getRandomChar();
            printf(ANSI_COLOR_GREEN"%c"ANSI_COLOR_RESET, c);
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

void printContent(
    int rows, int columns, int yOffset, 
    int length, int cycle, 
    int headPositionX, int headPositionY, 
    int foodX, int foodY, 
    int *xBody, int *yBody,
    bool matrixMode)
    {
    int depth = rows - yOffset;
    int width = columns - 2;
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
                hasHead = checkHead( x, y, headPositionX, headPositionY );              
                hasFood = checkFood( x, y, foodX, foodY );
                bodyIndex = checkBody( x, y, xBody, yBody, length );    

                // 2. Print appropriate element
                if ( hasHead )
                {                    
                    printSnakeHead( cycle, matrixMode );
                }                
                else if ( hasFood )
                {
                    printFood(cycle, matrixMode);
                }                
                else if ( bodyIndex >= 0 )
                {
                    printSnakeBody( bodyIndex, length, cycle, matrixMode);
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