#ifndef UTILS_H
#define UTILS_H

#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_direction.h"

// Function declarations
void enableNonCanonicalMode();
void disableNonCanonicalMode();
char getRandomChar();
char getRandomLatinChar();
char getRandomFullLatinChar();
char getRandomLatinCharOrNumber();
T_Game_Mode processArguments(int argc, char **argv, T_Game_Mode mode);
void cleanUp(T_Game_State *gameState);

#endif