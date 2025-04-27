#ifndef SETTINGS_H
#define SETTINGS_H

#include "def.h"

class Settings {
public:
    Settings();  // Constructor

    void updateDropSpeed();   // Update the drop speed when the level increases
    void resetSettings();     // Reset all settings when starting a new game
    void displaySettings();   // Display current settings in the console
    void increaseLevel();     // Increase level based on the number of lines cleared

private:
    int boardWidth;
    int boardHeight;
    int blockSize;
    Uint32 dropSpeed;         // Speed of the blocks falling (in milliseconds)
    int level;                // Current level
    int score;                // Current score
    int linesCleared;         // Number of lines cleared
};

#endif
