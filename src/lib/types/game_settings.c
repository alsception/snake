#include "game_settings.h"

/* Define the constant settings */
const T_Game_Settings GAME_SETTINGS = 
{
    .millis = 50, //Milliseconds duration for one position change and screen refresh cycle. Choose between 1-200. Smaller is faster     
    .bodyIncrement = 15,
    .maxLength = 1000, //If length is greater then this value-> then very interesting effects is produced on the screen
    .cursorVisible = false
};
