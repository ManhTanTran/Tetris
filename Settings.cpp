#include "Settings.h"
#include "def.h"
#include <iostream>

Settings::Settings() {
    boardWidth = BOARD_WIDTH;
    boardHeight = BOARD_HEIGHT;
    blockSize = BLOCK_SIZE;
    dropSpeed = DROP_DELAY;
    level = 1;
    score = 0;
    linesCleared = 0;
}

void Settings::updateDropSpeed() {
    dropSpeed = std::max(static_cast<Uint32>(100), DROP_DELAY - static_cast<Uint32>(level * 50));  // Decrease drop speed based on level
}

void Settings::resetSettings() {
    score = 0;
    linesCleared = 0;
    level = 1;
    updateDropSpeed();
}


void Settings::displaySettings() {

    std::cout << "Board Size: " << BOARD_WIDTH << "x" << BOARD_HEIGHT << std::endl;
    std::cout << "Block Size: " << BLOCK_SIZE << std::endl;
    std::cout << "Drop Speed: " << dropSpeed << "ms" << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "Lines Cleared: " << linesCleared << std::endl;
}

void Settings::increaseLevel() {
    if (linesCleared >= level * 10) {
        level++;
        updateDropSpeed();
    }
}
