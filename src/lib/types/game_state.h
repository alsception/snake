#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include "game_direction.h"
#include "game_mode.h"

//STATE VARIABLES
typedef struct 
{            
    long long int cycle; // ++ every screen refresh and position change

    // Screen size
    int rows;
    int columns;
    int rowsPrev;
    int columnsPrev;

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

    T_Game_Direction direction;
    T_Game_Mode mode;
} 
T_Game_State;

void init_game_state(T_Game_State *state, int initial_length);

void cleanup_game_state(T_Game_State *state);

#endif