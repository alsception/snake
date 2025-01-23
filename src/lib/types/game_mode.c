#include "game_mode.h"

// Function to add a mode
T_Game_Mode add_mode(T_Game_Mode current, T_Game_Mode new_mode) {
    if (current == NORMAL) {
        return new_mode; // Override if current mode is NORMAL
    }
    if (new_mode == NORMAL) {
        return NORMAL; // Reset if new mode is NORMAL
    }
    return current | new_mode; // Combine modes
}

// Function to remove a mode
T_Game_Mode remove_mode(T_Game_Mode current, T_Game_Mode mode_to_remove) {
    return current & ~mode_to_remove;
}

// Function to check if a mode is active
int is_mode_active(T_Game_Mode current, T_Game_Mode mode) {
    return (current & mode) != 0;
}
