#include "Settings.h"
#include "def.h"
#include <iostream>

// Constructor mặc định
Settings::Settings() {
    boardWidth = BOARD_WIDTH;
    boardHeight = BOARD_HEIGHT;
    blockSize = BLOCK_SIZE;
    dropSpeed = DROP_DELAY;  // Default drop speed
    level = 1;
    score = 0;
    linesCleared = 0;
}

// Cập nhật tốc độ rơi khi lên cấp
void Settings::updateDropSpeed() {
    dropSpeed = std::max(static_cast<Uint32>(100), DROP_DELAY - static_cast<Uint32>(level * 50));  // Decrease drop speed based on level
}

// Reset lại cài đặt khi bắt đầu trò chơi mới
void Settings::resetSettings() {
    score = 0;
    linesCleared = 0;
    level = 1;
    updateDropSpeed();  // Reset drop speed to the initial value
}

// Hiển thị cài đặt hiện tại
void Settings::displaySettings() {
    // Sử dụng các giá trị đã khai báo trong def.h
    std::cout << "Board Size: " << BOARD_WIDTH << "x" << BOARD_HEIGHT << std::endl;
    std::cout << "Block Size: " << BLOCK_SIZE << std::endl;
    std::cout << "Drop Speed: " << dropSpeed << "ms" << std::endl;
    std::cout << "Level: " << level << std::endl;
    std::cout << "Score: " << score << std::endl;
    std::cout << "Lines Cleared: " << linesCleared << std::endl;
}

// Cập nhật cấp độ dựa trên số dòng đã xóa
void Settings::increaseLevel() {
    if (linesCleared >= level * 10) {  // Every 10 lines cleared, increase the level
        level++;
        updateDropSpeed();  // Update the drop speed when the level increases
    }
}
