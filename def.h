#ifndef _DEFS__H
#define _DEFS__H

#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <iostream>

extern const int GRID_WIDTH;
extern const int SCORE_WIDTH;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int BLOCK_SIZE;
extern const int ROWS;
extern const int COLS;
extern const int BOARD_WIDTH;
extern const int BOARD_HEIGHT;
extern const char* WINDOW_TITLE;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern SDL_Texture* backgroundTexture;
extern TTF_Font* font;
extern bool running;
extern Uint32 lastDropTime;
extern Uint32 lockTimerStart;
extern Uint32 dropSpeed;
extern const Uint32 lockDelay;
extern const Uint32 DROP_DELAY;
extern bool gameOver;
extern bool showStartScreen;
extern bool onGround;
extern bool gameWin;
extern bool holdUsed;
extern bool hasHoldPiece;
extern bool holdUsedThisTurn;
extern bool paused;
extern bool musicPlayed;
extern bool showSettingsMenu;
extern bool isMuted;

extern std::vector<std::vector<SDL_Color>> grid;  // Changed from static array to vector
extern std::vector<std::vector<int>> board;       // Changed from static array to vector

extern int currentRotation, currentX, currentY;
extern int currentIndex;
extern int heldPiece;
extern int score;
extern int level;
extern int lines;
extern int linesCleared;
extern int volume;
extern int bagIndex;
extern std::vector<int> bag;

#endif
