#include "utils.h"

#include <unistd.h>
#include <stdlib.h> 
#include <termios.h>

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