#ifndef GAME_SETTINGS_H
#define GAME_SETTINGS_H

#include <stdbool.h>

/* FIXED SETTINGS */
typedef struct 
{   
    int millis;// = 50; //Milliseconds duration for one position change and screen refresh cycle. Choose between 1-200. Smaller is faster     
    int bodyIncrement;// = 15;
    int maxLength;// = 1000; //If length is greater then this value-> then very interesting effects is produced on the screen
    bool cursorVisible;// = false;
    //todo: add starting length
} T_Game_Settings;


/* Constant settings instance */
extern const T_Game_Settings SETTINGS;

#endif