#include "game_settings.h"

/* Define the constant settings */
const T_Game_Settings SETTINGS = 
{
    .millis = 50, 
        //Milliseconds duration for one position change and screen refresh cycle. Choose between 1-200. Smaller is faster     
        // Standard: 50
        // Fast: 20
        // Superfast <= 10
        // Slow 100
    .bodyIncrement = 15,
    .maxLength = 1000, //If length is greater then this value-> then very interesting effects is produced on the screen
    .cursorVisible = false
};
