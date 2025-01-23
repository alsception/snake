#ifndef GAME_MODE_H
#define GAME_MODE_H

typedef enum {
    NORMAL          = 0x00,        // 0b0000 This is default mode (normal) that cannot be combined with others
    AUTO            = 0x01,        // 0b0001
    GOD             = 0x02,        // 0b0010
    MATRIX          = 0x04,        // 0b0100
    LAYING_EGGS     = 0x08         // ob1ooo  
} T_Game_Mode;

// Function prototypes 
T_Game_Mode add_mode(T_Game_Mode current, T_Game_Mode new_mode);

T_Game_Mode remove_mode(T_Game_Mode current, T_Game_Mode mode_to_remove);

int is_mode_active(T_Game_Mode current, T_Game_Mode mode);

#endif