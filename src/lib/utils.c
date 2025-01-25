#include "utils.h"
#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_direction.h"

#include <unistd.h>
#include <stdlib.h> 
#include <termios.h>
#include <string.h>
#include <stdio.h>

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

char getRandomLatinChar()
{
    char randomletter = 'A' + (random() % 26);//56
    return randomletter;
}

char getRandomFullLatinChar()
{
    char randomletter = '!' + (random() % (126-32));//full latin alfabet is going from 32(!) to 126
    return randomletter;
}

char getRandomLatinCharOrNumber() 
{
    int randomIndex = random() % 36; // 26 letters + 10 digits
    if (randomIndex < 26) 
    {
        return 'A' + randomIndex; // Latin letters
    } 
    else 
    {
        return '0' + (randomIndex - 26); // Numbers
    }
}

char getRandomChar()
{
    return getRandomLatinCharOrNumber();
}

T_Game_Mode processArguments(int argc, char **argv, T_Game_Mode mode)
{
    char gm[] = "god-mode";
    char le[] = "lay-eggs";
    char at[] = "auto";
    char mm[] = "matrix";
    
    for (int i = 1; i < argc; ++i)
    {
        int result;
        
        result = strcmp(gm, argv[i]);
        if (result == 0) 
        {
            mode = add_mode(mode, GOD);
            printf("God mode activated: %d\n", mode);
        } 
    
        result = strcmp(le, argv[i]);
        if (result == 0) 
        {
            mode = add_mode(mode, LAYING_EGGS);
            printf("Laying eggs mode activated: %d\n", mode);
        }

        result = strcmp(at, argv[i]);
        if (result == 0) 
        {
            mode = add_mode(mode, AUTO);
            mode = add_mode(mode, GOD); //Need god-mode also, otherwise its stuck
            printf("Auto mode activated: %d\n", mode);
        }

        result = strcmp(mm, argv[i]);
        if (result == 0) 
        {
            mode = add_mode(mode, MATRIX);
            printf("Wake up Neo, Matrix mode is activated...: %d\n", mode);
        }

       usleep(1000 * 1000);
    }

    return mode;
}

void cleanUp(T_Game_State *gameState)
{
    disableNonCanonicalMode();
    free(gameState->yBody);
    free(gameState->xBody);
}