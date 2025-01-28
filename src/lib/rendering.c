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

void render(T_Game_State *gameState)
{
    resetCursorPosition(gameState);    
    printHeaderLine(gameState);        
    printContent(gameState, is_mode_active( gameState->mode, MATRIX )); 
    
    if(!SETTINGS.cursorVisible) printf("\e[?25l"); // Remove cursor and flashing
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
                    printSnakeBody( bodyIndex, gameState->length, gameState->cycle, matrixMode, gameState);
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

void printSnakeHead(int cycle, bool matrixMode)
{
    if( matrixMode )
    {
        printRainDrop();
    }
    else
    {
        if ( cycle % 2 )
        {
            printf( ANSI_COLOR_HIGREEN "X" ANSI_COLOR_RESET );
        }
        else
        {
            printf( ANSI_COLOR_CYAN "@" ANSI_COLOR_RESET );
        }
    }
}

void printRainDrop()
{
    char c = getRandomChar();
    printf(ANSI_COLOR_HIGREEN);
    printf("%c", c);
    printf(ANSI_COLOR_RESET);
}

void printSnakeBody(int bodyIndex, int length, int cycle, bool matrixMode, T_Game_State *gameState)
{
    if(matrixMode)
    {
        printRandomGreenChar();
    }
    else
    {    
        // We will add a little bit of styling to the snake, based on body's element and cycle
        if (bodyIndex > length - 10)
        {
            printEndTail(bodyIndex, length);
        }
        else
        {
            if(gameState->timeToFlash > 0)
            {
                /////////start flashing part
                flash(bodyIndex, cycle);
                /////////end flashing part
            }
            else
            {
                //Snake will be stripped
                printNormalBodySegment(bodyIndex);
            }
            
        }   
    }
}

void printRandomGreenChar()
{
    char c = getRandomChar();
    printf(ANSI_COLOR_GREEN);
    printf("%c", c);
    printf(ANSI_COLOR_RESET);
}

void printNormalBodySegment(int bodyIndex)
{
    if (bodyIndex % 3)
    {
        printf(ANSI_COLOR_CYAN "▓" ANSI_COLOR_RESET);
    }
    else
    {
        printf(ANSI_COLOR_HIGREEN "░" ANSI_COLOR_RESET);
    }
}

void printEndTail(int bodyIndex, int length)
{
    if (bodyIndex == length - 2)
    {
        printf(ANSI_COLOR_GREEN "◘" ANSI_COLOR_RESET);
    }
    else if (bodyIndex % 3)
    {
        printf(ANSI_COLOR_GREEN "•" ANSI_COLOR_RESET);
    }
    else
    {
        printf(ANSI_COLOR_CYAN "•" ANSI_COLOR_RESET);
    }
}

void flash(int bodyIndex, int cycle)
{
    if (bodyIndex % 3)
    {
        if (cycle % 2)
        {
            printf(ANSI_COLOR_HIGREEN "+" ANSI_COLOR_RESET);
        }
        else
        {
            printf(ANSI_COLOR_BLUE "+" ANSI_COLOR_RESET);
        }
    }
    else
    {
        if (cycle % 2)
        {
            printf(ANSI_COLOR_YELLOW "▓" ANSI_COLOR_RESET);
        }
        else
        {
            printf(ANSI_COLOR_RED "♦" ANSI_COLOR_RESET);
        }
    }
}

void printFood(int cycle, bool matrixMode)
{
    
    const char *ANSI_COLOR_FOOD;
    
    if( matrixMode )
    {
        //Food is red pill
        ANSI_COLOR_FOOD = ANSI_COLOR_RED;
        printf( "%sΘ%s", ANSI_COLOR_FOOD,ANSI_COLOR_RESET );
    }
    else
    {
        // Food will be dollar sign (everybody likes dollar sign)
        ANSI_COLOR_FOOD = ANSI_COLOR_RED;       
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
            {
                printf("+");
            }
            else
            {
                printf("=");
            }
                
        }
        else
        {
            if (x == 0 || x == width)
            {
                printf("|");
            }
            else
            {
                printf(" ");
            }
        }
    }
}

// Return true if food is present at given x,y position
bool checkFood(int x, int y, int foodX, int foodY)
{
    if (x == foodX && y == foodY)
    {
        return true;
    }
    else
    { 
        return false;
    }
}

// Return true if snake head is present at given x,y position
bool checkHead(int x, int y, int headPositionX, int headPositionY)
{
    if (x == headPositionX && y == headPositionY)
    {
        return true;
    }
    else
    {
        return false;
    }
        
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

// Upper line with informations
void printHeaderLine(T_Game_State *gameState)
{       
    if(gameState->columns < 155)
    {
        //Make it responsive :)
        printMiniHeaderLine(gameState->mode, gameState->length, gameState->foodEaten);
    }
    else
    {   
        printMaxiHeaderLine(gameState);
    }
}

void printMaxiHeaderLine(T_Game_State *gameState)
{
    const char *color = is_mode_active(gameState->mode, MATRIX) ? ANSI_COLOR_GREEN : ANSI_COLOR_BLUE;

    //TODO: IF HEAD X OR Y EQUALS TO FOODS -> THEN COLOR IN GREEN

    printf("\r");
    printHeaderCell("Rows(X)", gameState->rows, color);
    printHeaderCell("Columns(Y)",gameState->columns, color);
    printHeaderCell("HeadX",gameState->headPositionX, color);
    printHeaderCell("HeadY",gameState->headPositionY, color);
    printHeaderCell("FoodX",gameState->foodX, color);
    printHeaderCell("FoodY",gameState->foodY, color);
    printHeaderCell("Millis",SETTINGS.millis, color);
    printHeaderCell("Food",gameState->foodEaten, color);
    printHeaderCell("Length",gameState->length, color);    
    printf("    ");// Empty space is needed
}

void printHeaderCell(char* title, int value, const char* color1)
{
    printf( "%s%s: " ANSI_COLOR_RESET ANSI_COLOR_HIGREEN "%d " ANSI_COLOR_RESET, color1, title, value);
}

void printMiniHeaderLine(T_Game_Mode mode, int length, int foodEaten)
{
    if (is_mode_active(mode, MATRIX))
    {
        printf(
            "\r" 
            ANSI_COLOR_GREEN "Θ: "  
            ANSI_COLOR_HIGREEN "%d " 
            ANSI_COLOR_RESET
            ANSI_COLOR_GREEN "$: "  
            ANSI_COLOR_HIGREEN "%d " 
            ANSI_COLOR_RESET,
            foodEaten, length);
    }
    else
    {
        printf(
            "\r" 
            ANSI_COLOR_BLUE "Θ: "  
            ANSI_COLOR_HIGREEN "%d " 
            ANSI_COLOR_RESET
            ANSI_COLOR_BLUE "$: "  
            ANSI_COLOR_HIGREEN "%d " 
            ANSI_COLOR_RESET,
            foodEaten, length);
    }        
} 

void printGameOverScreen(T_Game_State *gameState)
{
    printf("\nGame Over\n");
    printf("Food eaten: %d \n", gameState->foodEaten);
    printf("Length achieved: %d \n", gameState->length);
    printf("Cycles played: %lld \n", gameState->cycle);
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
        printf("\033[2J");
        //Clear screen
        /*Alternatively use system("clear");*/
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