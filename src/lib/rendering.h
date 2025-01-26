#ifndef RENDERING_H
#define RENDERING_H

#include "types/game_mode.h"
#include "types/game_state.h"
#include "types/game_settings.h"

// Function declarations

void render(T_Game_State *gameState);

void printSnakeHead(int cycle, bool matrixMode);

void printSnakeBody(int bodyIndex, int length, int cycle, bool matrixMode);

void printFood(int cycle, bool matrixMode);

int checkBody(int x, int y, int *xBody, int *yBody, int length);

bool checkFood(int x, int y, int foodX, int foodY);

bool checkHead(int x, int y, int headPositionX, int headPositionY);

void printEmptyContent(int x, int y, int width, int depth, bool matrixMode);

void printContent(T_Game_State *gameState, bool matrixMode);

void printHeaderLine(T_Game_State *gameState);

void printMaxiHeaderLine(T_Game_State *gameState);

void printHeaderCell(char* title, int value, const char* color1);

void printMiniHeaderLine(T_Game_Mode mode, int length, int foodEaten);

void printGameOverScreen(T_Game_State *gameState);

void setWindowSize(T_Game_State *gameState);

void resetCursorPosition(T_Game_State *gameState);

#endif