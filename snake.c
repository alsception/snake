#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <stdbool.h> // Include this header to use bool, true, and false

// Colors
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

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

int millis = 50;
int millisFast = 25;
int millisMedium = 75;
int millisSlow = 100;

// Allocate initial array
int *yBody;
int *xBody;
int direction = 'R'; // R for right, L for left, U for up, D for down
int foodX = 0;
int foodY = 0;
int length = 15;//15 tested max with 1115
int bodyIncrement = 15;//35
int foodEaten = 0;
int cf = 100; //Number of cycles to completely redraw the screen (Constant redrawing causes flashing, but neccessary)

bool godMode = false;
bool cursorVisible = false;

// Function to enable non-canonical mode
// https://stackoverflow.com/questions/358342/canonical-vs-non-canonical-terminal-input
void enableNonCanonicalMode()
{
    struct termios newSettings;
    tcgetattr(STDIN_FILENO, &newSettings);
    newSettings.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);
}

// Function to restore terminal to original settings
void disableNonCanonicalMode()
{
    struct termios originalSettings;
    tcgetattr(STDIN_FILENO, &originalSettings);
    originalSettings.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);
}

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
        if ((ch == 'a' || ch == 'A') && direction != 'R')
            direction = 'L';

        else if ((ch == 'd' || ch == 'D') && direction != 'L')
            direction = 'R';

        else if ((ch == 'w' || ch == 'W') && direction != 'D')
            direction = 'U';

        else if ((ch == 's' || ch == 'S') && direction != 'U')
            direction = 'D';

        else if (ch == 'q' || ch == 'Q')
            return 0; // Quit the game
    }
    return 1;
}

void getWindowSize()
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

void initialize()
{
    system("clear");
    
    // Initial size print and frame
    // This is important to get rows and columns    
    getWindowSize();

    //TODO 
    /*** PLS REALOCATE MEMORY IF WINDOW SIZE IS CHANGED ***/

    int maxLength = rows * columns;
    yBody = (int *)malloc(maxLength * sizeof(int));
    xBody = (int *)malloc(maxLength * sizeof(int));
    
    // Initialize array
    for (int i = 0; i < maxLength; i++)
    {
        yBody[i] = 0;
        xBody[i] = 0;
    }    

    foodX = (rand() % (columns - 4) + 2);
    foodY = (rand() % (rows - 4) + 2);

    enableNonCanonicalMode();
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Set input to non-blocking mode
}

void resetCursorPosition()
{
    getWindowSize();//update window size

    //TODO 
    //If window size changed -> then clear the screen
    //system("clear");

    printf("\r"); //go to start of the line
    for(int i = 0; i< rows; i++)
    {
        printf("\033[A");
    } 
}

// Upper line with informations
void printHeaderLine()
{   
    printf(
        "\r" ANSI_COLOR_BLUE "Terminal size: "
        "y:%d rows | "
        "x:%d columns | "
        "y-offset: %d | headPositionX: %d | headPositionY: %d | refresh rate: %d | foodX: %d | foodY: %d | length: %d" 
        ANSI_COLOR_RESET, rows, columns, yOffset, headPositionX, headPositionY, millis, foodX, foodY, length);
}

void printGameOverScreen()
{
    printf("\nGame Over\n");
    printf("Food eaten: %d \n", foodEaten);
    printf("Length achieved: %d \n", length);
    printf("Cycles played: %lld \n", cycle);
    printf("\e[?25h"); // Reenable cursor
}

// Return true if food is present at given x,y position
bool checkFood(int x, int y)
{
    if (x == foodX && y == foodY)
        return true;
    else
        return false;
}

// Return true if snake head is present at given x,y position
bool checkHead(int x, int y)
{
    if (x == headPositionX && y == headPositionY)
        return true;
    else
        return false;
}

// Return index of snake's body element if present at given x,y position
// Otherwise return -1
int checkBody(int x, int y)
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

void eat()
{
    // eat food and create new food
    foodX = (rand() % (columns - 4) + 2);
    foodY = (rand() % (rows - 4) + 2);
    length += bodyIncrement;
    foodEaten++;
}

void printSnakeBody(int bodyIndex)
{
    // We will add a little bit of styling to the snake, based on body's element and cycle
    if (bodyIndex > length - 10)
    {
        // tail //◘ •
        if (bodyIndex % 2)
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
            if (cycle % 2){
                printf(ANSI_COLOR_CYAN "♦" ANSI_COLOR_RESET);
            }
            else
            {
                printf(ANSI_COLOR_GREEN "♦" ANSI_COLOR_RESET);
            }
        }
    }   
}

void printSnakeHead()
{
    if ( cycle % 2 )
        printf(ANSI_COLOR_GREEN "X" ANSI_COLOR_RESET);
    else
        printf( ANSI_COLOR_CYAN "@" ANSI_COLOR_RESET );
}

void printFood()
{
    // Food will also be flashing dollar sign
    const char *ANSI_COLOR_FOOD;
    if ( cycle % 2 )
        ANSI_COLOR_FOOD = ANSI_COLOR_YELLOW;
    else
        ANSI_COLOR_FOOD = ANSI_COLOR_BLUE;
    
    printf( "%s$%s", ANSI_COLOR_FOOD, ANSI_COLOR_RESET );
}

void gameOver()
{
    disableNonCanonicalMode();
    free(yBody);
    free(xBody);
    printf("\nGame Over\n");
    printf("Length achieved: %d \n", length);
    printf("Cycles played: %lld \n", cycle);
    printf("\e[?25h"); // Reenable cursor
    exit(0);
}

void updateHeadPosition()
{
    if (direction == 'L')
    {
        headPositionX--;
        if (headPositionX < 1)
            headPositionX = columns - 2;
    }
    else if (direction == 'R')
    {
        headPositionX++;
        if (headPositionX > columns - 1)
            headPositionX = 1;
    }
    else if (direction == 'U')
    {
        headPositionY--;
        if (headPositionY < 1)
            headPositionY = rows - 2;
    }
    else if (direction == 'D')
    {
        headPositionY++;
        if (headPositionY > rows-1)
            headPositionY = 1;
    }   
    
}

/* //TODO 
void resizeBodyIfNeeded() {
    if (length >= ?) {
        n *= 2; // Double the size
        yBody = realloc(yBody, n * sizeof(int));
        xBody = realloc(xBody, n * sizeof(int));
        
        if (yBody == NULL || xBody == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(1);
        }
    }
}*/

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

void printEmptyContent( int x, int y, int width, int depth )
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

void printContent()
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
            if (y > 0)
            {
                // 1. Check if the screen cell contains head,food or body
                hasHead = checkHead( x, y );              
                hasFood = checkFood( x, y );
                bodyIndex = checkBody( x, y );    

                // 2. Print appropriate element
                if ( hasHead )
                {                    
                    printSnakeHead( bodyIndex );
                }                
                else if ( hasFood )
                {
                    printFood();
                }
                else if ( bodyIndex>=0 )
                {
                    printSnakeBody( bodyIndex );
                }
                else
                {
                    printEmptyContent( x, y, width, depth );
                }
            }
        }
        
    }    
    fflush(stdout);
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
    // (temporary) Fix:
    // Every n cycles clear the screen.
    if( cycle % cf )
        system("clear");

    resetCursorPosition();    
    printHeaderLine();    
    printContent();
    
    if(!cursorVisible) printf("\e[?25l"); // Remove cursor and flashing
}

void refreshScreen()
{
    updateSnakeData();
    render();    
}

int main()
{
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