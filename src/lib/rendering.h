#ifndef RENDERING_H
#define RENDERING_H

// Function declarations

int checkBody(int x, int y, int *xBody, int *yBody, int length);

bool checkFood(int x, int y, int foodX, int foodY);

bool checkHead(int x, int y, int headPositionX, int headPositionY);

void printSnakeBody(int bodyIndex, int length, int cycle, bool matrixMode);

void printSnakeHead(int cycle, bool matrixMode);

void printFood(int cycle, bool matrixMode);

void printEmptyContent(int x, int y, int width, int depth, bool matrixMode);

void printContent(
    int rows, int columns, int yOffset, 
    int length, int cycle, 
    int headPositionX, int headPositionY, 
    int foodX, int foodY, 
    int *xBody, int *yBody,
    bool matrixMode);

#endif