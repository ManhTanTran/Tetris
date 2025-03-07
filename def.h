#ifndef _DEFS__H
#define _DEFS__H
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <iostream>

const int GRID_WIDTH = 300;         // Kích thước lưới chơi (chiều rộng)
const int SCORE_WIDTH = 100;        // Kích thước cột điểm
const int SCREEN_WIDTH = GRID_WIDTH + SCORE_WIDTH+100; // Tổng chiều rộng cửa sổ
const int SCREEN_HEIGHT = 600;
const int BLOCK_SIZE = 30;
const int ROWS = SCREEN_HEIGHT / BLOCK_SIZE;
const int COLS = GRID_WIDTH / BLOCK_SIZE; // Số cột dựa trên GRID_WIDTH

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr; // Font dùng để hiển thị điểm
bool running = true;
Uint32 lastDropTime = 0;
const Uint32 DROP_DELAY = 500;
bool gameOver = false;
bool showStartScreen = true;

// Mảng lưu màu sắc của từng ô lưới; ô trống có alpha = 0
SDL_Color grid[ROWS][COLS] = {0};
int score = 0;
int level = 1;  // Mặc định bắt đầu từ level 1
int lines = 0;  // Số dòng đã xóa


#endif
