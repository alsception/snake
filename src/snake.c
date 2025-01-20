//Standard libraries
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h> // Include this header to use bool, true, and false
#include <string.h>

//Our library
#include "lib/utils.h"
#include "lib/rendering.h"

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

// Variables
long long int cycle = 0;
int rows = 0;
int rowsPrevious = 0;
int columns = 0;
int columnsPrevious = 0;
int yOffset = 2;
int xOffset = 0;
int headPositionX = 21;
int headPositionY = 22;

int millis = 50;//50;
int millisFast = 25;
int millisMedium = 75;
int millisSlow = 100;

int *yBody;
int *xBody;
int direction = 'R'; // R for right, L for left, U for up, D for down
int foodX = 0;
int foodY = 0;
int length = 14;//15 tested max with 1115
int bodyIncrement = 15;//35
int foodEaten = 0;
int cf = 1000; //Number of cycles to completely redraw the screen (Constant redrawing causes flashing, but neccessary)
int wallSize = 0;
int maxLength = 1000;

bool godMode = false;
bool autoMode = false;
bool matrixMode = false;
bool cursorVisible = false;
bool generateFoodRandomly = true;
bool pausa = false;

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
            case 'a': case 'A':
                if (direction != 'R' || godMode)
                    direction = 'L';
                break;

            case 'd': case 'D':
                if (direction != 'L' || godMode)
                    direction = 'R';
                break;

            case 'w': case 'W':
                if (direction != 'D' || godMode)
                    direction = 'U';
                break;

            case 's': case 'S':
                if (direction != 'U' || godMode)
                    direction = 'D';
                break;

            case 'p': case 'P':
                pausa = !pausa;
                break;

            case 'q': case 'Q':
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
}

void generateFood()
{
    //Food is generated randomly
    foodX = (rand() % (columns - 4) + 2);
    foodY = (rand() % (rows - 4) + 2);
}

void initBody()
{  
    yBody = (int *)malloc(maxLength * sizeof(int));
    xBody = (int *)malloc(maxLength * sizeof(int));
    
    // Initialize array
    for (int i = 0; i < maxLength; i++)
    {
        yBody[i] = 0;
        xBody[i] = 0;
    }    
}

void initialize()
{
    system("clear");
    
    // Initial size print and frame
    // This is important to get rows and columns    
    setWindowSize();

    if(columns < 7 || rows < 5 )
    {
        printf("\nScreen is too small to play snake :/ \nPlease ensure at least 5 rows height and 7 columns width (found %i r, %i c).\n",rows,columns);
        exit(0);
    }

    headPositionX = columns/4;
    headPositionY = rows/2;

    initBody();
    generateFood();

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
bool detectBodyCollision()
{
    for (int i = 0; i < length; i++)
    {
        if (headPositionX == xBody[i] && headPositionY == yBody[i])
        {
            return true;
        }
    }
    return false;
}

bool detectEating()
{
    //Why this separate function for 1 expression?
    //Because maybe in the future we would want to detect eating by some other condition
    return headPositionX == foodX && headPositionY == foodY;
}

void layEgg()
{    
    //Food is actually layed egg from snake's tail
    foodX = xBody[length-1];
    foodY = yBody[length-1];
}

void eat()
{
    if (generateFoodRandomly)
    {
        do
        {
            generateFood();
        } 
        while (checkBody(foodX, foodY, xBody, yBody, length) > -1);
    }
    else
    {
        layEgg();
    }

    length += bodyIncrement;
    foodEaten++;
}

// Upper line with informations
void printHeaderLine()
{   
    if(columns<155)
    {
        //Make it responsive :)        
        if(matrixMode)
            printf( "\r" ANSI_COLOR_GREEN "$: " ANSI_COLOR_HIGREEN "%d" ANSI_COLOR_RESET, length);
        else
            printf( "\r" ANSI_COLOR_YELLOW "$: " ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET, length);
    }
    else
    {    
        const char* color = matrixMode ? ANSI_COLOR_GREEN : ANSI_COLOR_BLUE;
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
            color, rows, columns, headPositionX, headPositionY, millis, foodX, foodY, ANSI_COLOR_HIGREEN, length);
    }
}

void printGameOverScreen()
{
    printf("\nGame Over\n");
    printf("Food eaten: %d \n", foodEaten);
    printf("Length achieved: %d \n", length);
    printf("Cycles played: %lld \n", cycle);
    printf("\e[?25h"); // Reenable cursor
}


void gameOver()
{
    disableNonCanonicalMode();
    free(yBody);
    free(xBody);
    printGameOverScreen();    
    exit(0);
}

void autoPlay()
{
    int z = (rand() % (4) + 0);

    switch (z)
    {
    case 0:
        headPositionX--;
        if (headPositionX < 1)
            headPositionX = columns - 2;
        break;

    case 1:
        headPositionX++;
        if (headPositionX > columns - 1)
            headPositionX = 1;
        break;

    case 2:
        headPositionY--;
        if (headPositionY < 1)
            headPositionY = rows - 2;
        break;

    case 3:
        headPositionY++;
        if (headPositionY > rows - 1)
            headPositionY = 1;
        break;
    
    default:
        break;
    }
}

void updateHeadPosition()
{
    if (autoMode && (cycle%100))
    {
        autoPlay();
    }
    else
    {
        switch (direction) {
            case 'L':
                headPositionX--;
                if (headPositionX < 1)
                    headPositionX = columns - 3;
                break;

            case 'R':
                headPositionX++;
                if (headPositionX > columns - 3)
                    headPositionX = 1;
                break;

            case 'U':
                headPositionY--;
                if (headPositionY < yOffset)
                    headPositionY = rows - 3;
                break;

            case 'D':
                headPositionY++;
                if (headPositionY > rows - yOffset - 1)
                    headPositionY = yOffset;
                break;

            default:
                break;
        }

    }
}

void updateBodyPosition()
{
    //Every element (i) of the snakes body takes the x,y coordinates of the previous element (i-1)
    //And first body element takes head's position
    for (int i = length - 1; i > 0; i--)
    {
        xBody[i] = xBody[i-1];
        yBody[i] = yBody[i-1];
    }

    xBody[0] = headPositionX;
    yBody[0] = headPositionY;
}

void updateSnakeData()
{
    // Update body before head, because it must follow the heads previous position
    updateBodyPosition();

    // Update head position based on current direction
    updateHeadPosition();

    if(!godMode && detectBodyCollision())
    {
        gameOver();        
    }    

    if (detectEating())
    {
        eat();
        updateBodyPosition();        
    }
}

void render()
{
    resetCursorPosition();    
    printHeaderLine();    
    printContent(rows, columns, yOffset, length, cycle, headPositionX, headPositionY, foodX, foodY, xBody, yBody, matrixMode);
    
    if(!cursorVisible) printf("\e[?25l"); // Remove cursor and flashing
}

void refreshScreen()
{
    if(!pausa)
    {
        updateSnakeData();
    }   
    render();  
}

void processArguments(int argc, char **argv)
{
    char gm[] = "god-mode";
    char le[] = "lay-eggs";
    char at[] = "auto";
    char mm[] = "matrix-mode";
    
    for (int i = 1; i < argc; ++i)
    {
        int result;
        
        result = strcmp(gm, argv[i]);
        if (result == 0) 
        {
            godMode = true;
            printf("God mode activated\n");
        } 
    
        result = strcmp(le, argv[i]);
        if (result == 0) 
        {
            generateFoodRandomly=false;
            printf("Laying eggs mode activated.\n");
        }

        result = strcmp(at, argv[i]);
        if (result == 0) 
        {
            autoMode = true;
            godMode = true; //need to have godmode also otherwise it crashes
            printf("Auto mode activated.\n");
        }

        result = strcmp(mm, argv[i]);
        if (result == 0) 
        {
            matrixMode=true;
            printf("Wake up Neo, Matrix mode is activated...\n");
        }

       usleep(1000 * 1000);
    }
}

int main(int argc, char **argv)
{
    if(argc > 0)
        processArguments(argc,argv);    

    initialize();
        
    while (1)
    {
        cycle++;
        if(!handleKeypress())
            break;
        refreshScreen();        
        usleep(millis * 1000); // Sleep for defined milliseconds
    }

    cleanUp();
    printGameOverScreen();

    return 0;
}