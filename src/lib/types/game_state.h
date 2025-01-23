#ifndef GAME_STATE
#define GAME_STATE

#include <stdbool.h>

//STATE VARIABLES
typedef struct 
{            
    long long int cycle; // ++ every screen refresh and position change

    // Screen size
    int rows;
    int columns;

    // Positioning    
    int xOffset;
    int yOffset;
    
    int headPositionX;
    int headPositionY;    
    
    // Snake tail position
    int *xBody;
    int *yBody;

    int foodX;
    int foodY;
    
    int length;    // tested max with 1115    
    int foodEaten;
    
    bool pausa;
} T_Game_State;

#endif