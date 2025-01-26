#ifndef ENGINE_H
#define ENGINE_H

#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_settings.h"
#include "types/game_direction.h"

void finalizeInitialization(T_Game_State *gameState);

void initBody(T_Game_State *gameState);

void generateFood(T_Game_State *gameState);

int readKeyPress(); 

int handleKeypress(T_Game_State *gameState);

void updateHeadPosition(T_Game_State *gameState);

void updateBodyPosition(T_Game_State *gameState);

bool detectBodyCollision(T_Game_State *gameState);

bool detectEating(T_Game_State *gameState);

void eat(T_Game_State *gameState);

void layEgg(T_Game_State *gameState);

void autoPlay(T_Game_State *gameState);

void updateSnakeData(T_Game_State *gameState);

void gameOver(T_Game_State *gameState);

#endif