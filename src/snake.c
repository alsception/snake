//Standard libraries
#include <unistd.h>

//Our libraries
#include "lib/types/game_mode.h"
#include "lib/types/game_direction.h"
#include "lib/types/game_settings.h"
#include "lib/types/game_state.h"
#include "lib/types/game_mode.h"
#include "lib/types/colors.h"
#include "lib/utils.h"
#include "lib/engine.h"
#include "lib/rendering.h"

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
            updateSnakeModel(&gameState);
        }   

        render(&gameState);     

        usleep(SETTINGS.millis * 1000); // Sleep for defined milliseconds
    }

    cleanUp(&gameState);

    printGameOverScreen(&gameState);

    return 0;
}