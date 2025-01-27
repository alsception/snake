//Standard libraries
#include <unistd.h>

//Our libraries
#include "lib/engine.h"
#include "lib/rendering.h"
#include "lib/utils.h"

int main(int argc, char **argv)
{   
    T_Game_State gameState;
    
    init_game_state(&gameState, SETTINGS.initialLength);

    if(argc > 0)
    {
        gameState.mode = processArguments(argc, argv, gameState.mode);    
    }

    finalizeInitialization(&gameState);
    
    while (1)
    {
        gameState.cycle++;

        if( !handleKeypress(&gameState) )
        {
            break;
        }
            
        if( !gameState.pausa )
        {
            updateGameState(&gameState); //Only snake model calculation will be paused 
        }   

        render(&gameState);//Render must go on even when paused     

        usleep(SETTINGS.millis * 1000); // Sleep for defined milliseconds
    }

    cleanUp(&gameState);

    printGameOverScreen(&gameState);

    return 0;
}