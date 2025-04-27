#ifndef SETTINGS_H
#define SETTINGS_H

#include "def.h"

class Settings {
public:
    Settings();

    void updateDropSpeed();
    void resetSettings();
    void displaySettings();
    void increaseLevel();

private:
    int boardWidth;
    int boardHeight;
    int blockSize;
    Uint32 dropSpeed;
    int level;
    int score;
    int linesCleared;
};

#endif
