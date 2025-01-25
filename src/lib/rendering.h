#ifndef RENDERING_H
#define RENDERING_H

#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_settings.h"

// Function declarations

void render(T_Game_State *gameState, T_Game_Mode mode);

int checkBody(int x, int y, int *xBody, int *yBody, int length);

bool checkFood(int x, int y, int foodX, int foodY);

bool checkHead(int x, int y, int headPositionX, int headPositionY);

void printSnakeBody(int bodyIndex, int length, int cycle, bool matrixMode);

void printSnakeHead(int cycle, bool matrixMode);

void printFood(int cycle, bool matrixMode);

void printEmptyContent(int x, int y, int width, int depth, bool matrixMode);

void printContent(T_Game_State *gameState, bool matrixMode);

void printHeaderLine(T_Game_State *gameState, T_Game_Mode mode);

void printMaxiHeaderLine(T_Game_Mode mode, T_Game_State *gameState);

void printMiniHeaderLine(T_Game_Mode mode, int length);

void printGameOverScreen(int foodEaten, int length, long long int cycle);

void setWindowSize(T_Game_State *gameState);

void resetCursorPosition(T_Game_State *gameState);

#endif