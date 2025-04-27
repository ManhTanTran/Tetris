#include "def.h"
#include <SDL_mixer.h>


const int GRID_WIDTH = 300;
const int SCORE_WIDTH = 100;
const int SCREEN_WIDTH = GRID_WIDTH + SCORE_WIDTH + 100;
const int SCREEN_HEIGHT = 600;
const int BLOCK_SIZE = 30;
const int ROWS = SCREEN_HEIGHT / BLOCK_SIZE;
const int COLS = GRID_WIDTH / BLOCK_SIZE;
const int BOARD_WIDTH = 10;
const int BOARD_HEIGHT = 20;
const char* WINDOW_TITLE = "Tetris Game";

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* backgroundTexture = NULL;
TTF_Font* font = nullptr;
bool running = true;
Uint32 lastDropTime = 0;
Uint32 lockTimerStart = 0;
Uint32 dropSpeed = 500;
const Uint32 lockDelay = 500;
const Uint32 DROP_DELAY = 500;
bool gameOver = false;
bool showStartScreen = true;
bool onGround = false;
bool gameWin = false;
bool holdUsed = false;
bool hasHoldPiece = false;
bool holdUsedThisTurn = false;
bool paused = false;
bool musicPlayed = false;
bool showSettingsMenu = false;
bool isMuted = false;


std::vector<std::vector<SDL_Color>> grid(ROWS, std::vector<SDL_Color>(COLS, {0}));  // Initialize as a vector of vectors
std::vector<std::vector<int>> board(BOARD_HEIGHT, std::vector<int>(BOARD_WIDTH, 0));  // Initialize as a vector of vectors

int currentRotation, currentX, currentY;
int currentIndex;
int heldPiece = -1;
int score = 0;
int level = 1;
int lines = 0;
int linesCleared = 0;
int bagIndex = 0;
int volume = MIX_MAX_VOLUME;
std::vector<int> bag;
