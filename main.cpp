#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <random>
#include "def.h"
#include "graphic.h"
#include "TextRenderer.h"
#include "mixer.h"
#include "Settings.h"
using namespace std;

std::mt19937 g(std::time(nullptr));

struct Block {
    int x, y;
};

struct Tetromino {
    vector<Block> blocks;
    SDL_Color color;
    Block pivot;
    int shape[4][4][4];
    void draw(SDL_Renderer* renderer, int offsetX, int offsetY, int blockSize);

    bool isValidMove(const vector<Block>& newBlocks) {
        for (const auto &b : newBlocks) {
            if (b.x < 0 || b.x >= COLS || b.y >= ROWS)
                return false;

            if (b.y >= 0 && grid[b.y][b.x].a != 0)
                return false;
        }
        return true;
    }
    bool isColliding(const Tetromino& piece, int dx, int dy) {
        for (const auto &block : piece.blocks) {
            int newX = block.x + dx;
            int newY = block.y + dy;
            if (newX < 0 || newX >= COLS || newY >= ROWS)
                return true;
            if (newY >= 0 && grid[newY][newX].a != 0)
                return true;
        }
        return false;
    }

    void storePiece(const Tetromino& piece);


    void move(int dx, int dy) {
        vector<Block> newBlocks = blocks;
        for (auto &b : newBlocks) {
            b.x += dx;
            b.y += dy;
        }
        if (isValidMove(newBlocks)) {
            blocks = newBlocks;
        }
    }


    void drop() {
        while (!isColliding(*this, 0, 1)) {
            move(0, 1);
        }
        storePiece(*this);
    }

    void rotate() {
    if (blocks.empty()) return;

    vector<Block> newBlocks;
    for (const auto &b : blocks) {
        int newX = pivot.x - (b.y - pivot.y);
        int newY = pivot.y + (b.x - pivot.x);
        newBlocks.push_back({newX, newY});
    }

    if (isValidMove(newBlocks)) {
        blocks = newBlocks;
        return;
    }
    std::cout << "Rotation failed without Wall Kick\n";
    return;

    vector<pair<int, int>> wallKicks = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1},
        {1, -1}, {-1, -1}, {1, 1}, {-1, 1}
    };
    if (!isValidMove(newBlocks)) {
        std::cout << "Trying Wall Kick adjustments...\n";
        for (const auto &[dx, dy] : wallKicks) {
            vector<Block> adjustedBlocks = newBlocks;
            for (auto &b : adjustedBlocks) {
                b.x += dx;
                b.y += dy;
            }
            if (isValidMove(adjustedBlocks)) {
                blocks = adjustedBlocks;
                return;
            }
        }
        std::cout << "All Wall Kick adjustments failed.\n";
    }
}


};

int tetrominoShapes[7][4][4][4] = {
    // I
    {{{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
     {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}},
     {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
     {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}},
    // J
    {{{1,0,0},{1,1,1},{0,0,0}},
     {{0,1,1},{0,1,0},{0,1,0}},
     {{0,0,0},{1,1,1},{0,0,1}},
     {{0,1,0},{0,1,0},{1,1,0}}},
    // L
    {{{0,0,1},{1,1,1},{0,0,0}},
     {{0,1,0},{0,1,0},{0,1,1}},
     {{0,0,0},{1,1,1},{1,0,0}},
     {{1,1,0},{0,1,0},{0,1,0}}},
    // O
    {{{1,1},{1,1}},
     {{1,1},{1,1}},
     {{1,1},{1,1}},
     {{1,1},{1,1}}},
    // S
    {{{0,1,1},{1,1,0},{0,0,0}},
     {{0,1,0},{0,1,1},{0,0,1}},
     {{0,0,0},{0,1,1},{1,1,0}},
     {{1,0,0},{1,1,0},{0,1,0}}},
    // T
    {{{0,1,0},{1,1,1},{0,0,0}},
     {{0,1,0},{0,1,1},{0,1,0}},
     {{0,0,0},{1,1,1},{0,1,0}},
     {{0,1,0},{1,1,0},{0,1,0}}},
    // Z
    {{{1,1,0},{0,1,1},{0,0,0}},
     {{0,0,1},{0,1,1},{0,1,0}},
     {{0,0,0},{1,1,0},{0,1,1}},
     {{0,1,0},{1,1,0},{1,0,0}}},
};

SDL_Color tetrominoColors[7] = {
    {0, 255, 255, 255},
    {0, 0, 255, 255},
    {255, 165, 0, 255},
    {255, 255, 0, 255},
    {0, 255, 0, 255},
    {160, 32, 240, 255},
    {255, 0, 0, 255}
};

void shuffleBag() {
    bag.clear();
    for (int i = 0; i < 7; ++i) {
        bag.push_back(i);
    }
    std::shuffle(bag.begin(), bag.end(), g);
    bagIndex = 0;
}

Tetromino createTetrominoFromId(int id) {
    Tetromino t;
    t.color = tetrominoColors[id];
    std::memcpy(t.shape, tetrominoShapes[id], sizeof(t.shape));


    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (t.shape[0][y][x]) {
                t.blocks.push_back({x + 3, y});
            }
        }
    }

    if (!t.blocks.empty()) {
        t.pivot = t.blocks[0];
    }

    return t;
}

Tetromino getNextPieceFromBag() {
    if (bagIndex >= bag.size()) {
        shuffleBag();
    }
    return createTetrominoFromId(bag[bagIndex++]);
}

Tetromino currentPiece;
Tetromino nextPiece;
Tetromino holdPiece;
TextRenderer text(renderer);
Mix_Music* currentMusic = nullptr;

bool checkCollision(int x, int y, int rotation) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece.shape[rotation][i][j]) {
                int newX = x + j;
                int newY = y + i;
                if (newX < 0 || newX >= COLS || newY >= ROWS)
                    return true;
                if (newY >= 0 && grid[newY][newX].a != 0)
                    return true;
            }
        }
    }
    return false;
}

void clearLines() {
    int linesCleared = 0;

    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool fullLine = true;

        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 0) {
                fullLine = false;
                break;
            }
        }

        if (fullLine) {
            for (int yy = y; yy > 0; yy--) {
                for (int xx = 0; xx < BOARD_WIDTH; xx++) {
                    board[yy][xx] = board[yy - 1][xx];
                }
            }

            for (int xx = 0; xx < BOARD_WIDTH; xx++) {
                board[0][xx] = 0;
            }

            linesCleared++;
            y++;

        }
    }
}



void spawnNewPiece() {
    currentPiece = nextPiece;
    nextPiece = getNextPieceFromBag();

    currentPiece.pivot = {5, 1};

    currentX = 3;
    currentY = 0;
    currentRotation = 0;
    holdUsedThisTurn = false;

    if (checkCollision(currentX, currentY, currentRotation)) {
        gameOver = true;
    }
}



void swapHold() {
    if (holdUsedThisTurn) return;

    if (!hasHoldPiece) {
        holdPiece = currentPiece;
        hasHoldPiece = true;
        spawnNewPiece();
    } else {
        std::swap(currentPiece, holdPiece);
        currentX = 3;
        currentY = 0;
        currentRotation = 0;

        currentPiece.blocks.clear();
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 4; x++) {
                if (currentPiece.shape[0][y][x]) {
                    currentPiece.blocks.push_back({x + currentX, y + currentY});
                }
            }
        }
    }

    holdUsedThisTurn = true;
}

void draw3DBlock(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color) {

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Viền sáng (trên & trái)
    SDL_SetRenderDrawColor(renderer,
        std::min(color.r + 60, 255),
        std::min(color.g + 60, 255),
        std::min(color.b + 60, 255), 255);

    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y);
    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h - 1);

    // Bóng tối (dưới & phải)
    SDL_SetRenderDrawColor(renderer,
        std::max(color.r - 60, 0),
        std::max(color.g - 60, 0),
        std::max(color.b - 60, 0), 255);

    SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h - 1, rect.x + rect.w - 1, rect.y + rect.h - 1);
    SDL_RenderDrawLine(renderer, rect.x + rect.w - 1, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1);
}


void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    for (int i = 0; i <= COLS; i++) {
        int x = i * BLOCK_SIZE;
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }

    for (int i = 0; i <= ROWS; i++) {
        int y = i * BLOCK_SIZE;
        SDL_RenderDrawLine(renderer, 0, y, COLS * BLOCK_SIZE, y);
    }

    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {

            SDL_Rect rect;
            rect.x = col * BLOCK_SIZE;
            rect.y = row * BLOCK_SIZE;
            rect.w = BLOCK_SIZE;
            rect.h = BLOCK_SIZE;

            SDL_Color blockColor = grid[row][col];
            draw3DBlock(renderer, rect, blockColor);
        }
    }
}

void drawTetromino(const Tetromino& piece, SDL_Renderer* renderer) {

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece.shape[currentRotation][i][j]) {
                int x = (currentX + j) * BLOCK_SIZE;
                int y = (currentY + i) * BLOCK_SIZE;

                SDL_Rect rect;
                rect.x = x;
                rect.y = y;
                rect.w = BLOCK_SIZE;
                rect.h = BLOCK_SIZE;

                draw3DBlock(renderer, rect, piece.color);
            }
        }
    }
}



void drawMiniTetromino(const Tetromino& piece, int offsetX, int offsetY) {
    int miniBlockSize = static_cast<int>(BLOCK_SIZE / 1.5);

    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (piece.shape[0][y][x]) {
                SDL_Rect rect = {
                    offsetX + x * miniBlockSize,
                    offsetY + y * miniBlockSize,
                    miniBlockSize,
                    miniBlockSize
                };
                draw3DBlock(renderer, rect, piece.color);
            }
        }
    }
}



bool isGameOver() {
    for (const auto &b : currentPiece.blocks) {
        if (b.y >= 0 && grid[b.y][b.x].a != 0)
            return true;
    }
    return false;
}

void clearFullRows() {
    int newRow = ROWS - 1;
    int linesCleared = 0;

    for (int y = ROWS - 1; y >= 0; y--) {
        bool full = true;
        for (int x = 0; x < COLS; x++) {
            if (grid[y][x].a == 0) {
                full = false;
                break;
            }
        }
        if (!full) {
            for (int x = 0; x < COLS; x++) {
                grid[newRow][x] = grid[y][x];
            }
            newRow--;
        } else {
            linesCleared++;
        }
    }

    for (int y = newRow; y >= 0; y--) {
        for (int x = 0; x < COLS; x++) {
            grid[y][x].a = 0;
        }
    }

    lines += linesCleared;
    score += linesCleared * 100;
    level = 1 + lines / 10;
}


void Tetromino::storePiece(const Tetromino& piece) {
   for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (currentPiece.shape[currentRotation][i][j]) {
                int x = currentX + j;
                int y = currentY + i;
                if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
                    grid[y][x] = currentPiece.color;
            }

    clearFullRows();
    spawnNewPiece();
    currentX = COLS / 2 - 2;
    currentY = 0;
    currentRotation = 0;

    if (isGameOver()) gameOver = true;
}


SDL_Texture* loadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Không thể load ảnh %s! Lỗi: %s\n", path, IMG_GetError());
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
    }
    return newTexture;
}

void renderBackground(SDL_Renderer* renderer, SDL_Texture* background) {
    SDL_RenderCopy(renderer, background, NULL, NULL);
}


void drawStartScreen() {

    SDL_Color white = {255, 255 , 255, 255};

    int alpha = (SDL_GetTicks() / 500) % 2 == 0 ? 255 : 50;

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "TETRIS", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_SetTextureAlphaMod(titleTexture, alpha); // Thay đổi độ trong suốt


    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 100, 50, 200, 50};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Let's Play", white);
    SDL_Texture* playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_SetTextureAlphaMod(playTexture, alpha);

    SDL_Rect playRect = {SCREEN_WIDTH / 2 - 150, 200, 300, 50};
    SDL_RenderCopy(renderer, playTexture, NULL, &playRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
}

void startScreenLoop() {
    bool inStartScreen = true;
    SDL_Event e;

    while (inStartScreen) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = false;
                return;
            }
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                inStartScreen = false;
            }
        }

        SDL_RenderClear(renderer);
        renderBackground(renderer, backgroundTexture);
        drawStartScreen();
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }
}

void resetGame() {
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = {0, 0, 0};

    score = 0;
    level = 1;
    linesCleared = 0;
    gameOver = false;

    bag.clear();
    shuffleBag();
    holdUsedThisTurn = false;
    hasHoldPiece = false;

    int index = bag.back(); bag.pop_back();
    currentPiece = createTetrominoFromId(index);
    currentIndex = index;

    nextPiece = createTetrominoFromId(bag.back());
    bag.pop_back();

    currentX = 3;
    currentY = 0;
    currentRotation = 0;
}


void handleInput(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        running = false;
        return;
    }

    if (e.type == SDL_KEYDOWN) {
        if (showStartScreen) {
            if (e.key.keysym.sym == SDLK_RETURN) {
                showStartScreen = false;
            }
        }
        else if (showSettingsMenu) {

            switch (e.key.keysym.sym) {
                case SDLK_s:
                    showSettingsMenu = false;
                    paused = false;
                    break;
                case SDLK_EQUALS:
                    Mix_VolumeMusic(MIX_MAX_VOLUME);
                    break;
                case SDLK_MINUS:
                    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
                    break;
                case SDLK_m:
                    if (isMuted) {
                        isMuted = false;
                        Mix_VolumeMusic(MIX_MAX_VOLUME);
                    } else {
                        isMuted = true;
                        Mix_VolumeMusic(0);
                    }
                    break;
            }
        }
        else {
            if (e.key.keysym.sym == SDLK_s) {
                showSettingsMenu = true;
                paused = true;
                return;
            }

            if (e.key.keysym.sym == SDLK_r && gameOver) {
                resetGame();
                return;
            }

            if (e.key.keysym.sym == SDLK_p && !gameOver && !showStartScreen) {
                paused = !paused;
                return;
            }

            switch (e.key.keysym.sym) {
                case SDLK_LEFT:
                    if (!checkCollision(currentX - 1, currentY, currentRotation))
                        currentX--;
                    break;
                case SDLK_RIGHT:
                    if (!checkCollision(currentX + 1, currentY, currentRotation))
                        currentX++;
                    break;
                case SDLK_DOWN:
                    if (!checkCollision(currentX, currentY + 1, currentRotation))
                        currentY++;
                    break;
                case SDLK_UP: {
                    int newRotation = (currentRotation + 1) % 4;
                    if (!checkCollision(currentX, currentY, newRotation))
                        currentRotation = newRotation;
                    if(onGround){
                        lockTimerStart = SDL_GetTicks();
                    }
                    break;
                }
                case SDLK_c:
                    if (!holdUsedThisTurn) {
                        if (!hasHoldPiece) {
                            holdPiece = currentPiece;
                            hasHoldPiece = true;
                            spawnNewPiece();
                        } else {
                            std::swap(currentPiece, holdPiece);
                            currentX = 3;
                            currentY = 0;
                            currentRotation = 0;
                        }
                        holdUsedThisTurn = true;
                    }
                    break;
                case SDLK_SPACE:
                    while (!checkCollision(currentX, currentY + 1, currentRotation)) {
                        currentY++;
                    }
                    break;
            }
        }
    }
}



void update() {
    if (paused) return;

    if (gameOver && !musicPlayed) {
        Mixer::haltMusic();
        Mixer::playMusic("win_theme.mp3");
        musicPlayed = true;
    }

    if (level >= 5 && !gameWin) {
        gameWin = true;
        Mixer::haltMusic();
        Mixer::playMusic("win_theme.mp3");
        musicPlayed = true;
        return;
    }

    if (gameOver || gameWin) return;

    Uint32 currentTime = SDL_GetTicks();

    if (checkCollision(currentX, currentY + 1, currentRotation)) {
        if (!onGround) {
            onGround = true;
            lockTimerStart = currentTime;
        } else if (currentTime - lockTimerStart >= lockDelay) {
            currentPiece.storePiece(currentPiece);
            onGround = false;
        }
    } else {

        if (currentTime - lastDropTime >= DROP_DELAY / std::max(1, 2 - level / 5)) {
            currentY++;
            lastDropTime = currentTime;
        }
        onGround = false;
    }
}


void drawGameOverScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("BAUHS93.ttf", 48);
    if (!font) {
        cout << "Không thể load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Color textColor = {255, 0, 0, 255}; // Màu chữ đỏ
    SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER!", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW = surface->w, textH = surface->h;
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_Rect bgRect = {textRect.x - 20, textRect.y - 10, textRect.w + 40, textRect.h + 20};
    SDL_RenderFillRect(renderer, &bgRect);

    // Vẽ chữ lên trên nền
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}

void drawWinScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("BAUHS93.ttf", 48);
    if (!font) {
        cout << "Không thể load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Color textColor = {255, 0, 0, 255}; // Màu chữ đỏ
    SDL_Surface* surface = TTF_RenderText_Solid(font, "YOU WIN!", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW = surface->w, textH = surface->h;
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_Rect bgRect = {textRect.x - 20, textRect.y - 10, textRect.w + 40, textRect.h + 20};
    SDL_RenderFillRect(renderer, &bgRect);


    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}

void renderSettingsMenu(SDL_Renderer* renderer) {

    const int SETTINGS_WIDTH = 350;
    const int SETTINGS_HEIGHT = 200;

    int settingsX = (SCREEN_WIDTH - SETTINGS_WIDTH) / 2;
    int settingsY = (SCREEN_HEIGHT - SETTINGS_HEIGHT) / 2;

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // xám đậm
    SDL_Rect settingsRect = {settingsX, settingsY, SETTINGS_WIDTH, SETTINGS_HEIGHT};
    SDL_RenderFillRect(renderer, &settingsRect);

    SDL_Color white = {255, 255, 255, 255};

    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Settings", white);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2,
        settingsY + 20,
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &messageRect);

    surfaceMessage = TTF_RenderText_Solid(font, "Press + to increase volume", white);
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect increaseVolumeRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2,
        settingsY + 60,
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &increaseVolumeRect);

    surfaceMessage = TTF_RenderText_Solid(font, "Press - to decrease volume", white);
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect decreaseVolumeRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2,
        settingsY + 100,
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &decreaseVolumeRect);
    surfaceMessage = TTF_RenderText_Solid(font, "Press M to turn off/on volume", white);
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect turnonoroffVolumeRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2,
        settingsY + 140,
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &turnonoroffVolumeRect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}




void render() {
    if (gameOver) {
        drawGameOverScreen();
        return;
    }
    if (gameWin) {
        drawWinScreen();
        return;
    }

    if (showSettingsMenu) {
        renderSettingsMenu(renderer);
        SDL_RenderPresent(renderer);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);

    renderBackground(renderer, backgroundTexture);

    drawGrid();

    drawTetromino(currentPiece, renderer);

    SDL_Color white = {255, 255, 255, 255};

    static TextRenderer text(renderer);

    int infoX = SCREEN_WIDTH - 100;
    int y = 30;

    text.drawText("Score: " + std::to_string(score), infoX, y, white);
    y += 30;
    text.drawText("Level: " + std::to_string(level), infoX, y, white);
    y += 30;

    text.drawText("Next Piece", infoX, y, white);

    int nextPieceY = y + 30;
    drawMiniTetromino(nextPiece, infoX-30, nextPieceY);
    y += 90;

    text.drawText("Hold Piece", infoX, y, white);

    int holdPieceY = y + 30;
    if (hasHoldPiece) {
        drawMiniTetromino(holdPiece, infoX - 30, holdPieceY);
    }

    if (paused) {
        static Uint32 lastTime = SDL_GetTicks();
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime > 500) {
            lastTime = currentTime;
        }

        Uint8 alpha = (currentTime / 100) % 2 == 0 ? 255 : 0;

        SDL_Color pausedColor = {255, 255, 255, alpha};
        text.drawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, pausedColor);
    }

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    shuffleBag();
    currentPiece = getNextPieceFromBag();
    nextPiece = getNextPieceFromBag();
    if (Mix_Init(MIX_INIT_MP3) == 0) {
        std::cout << "Mix_Init Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cout << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        return -1;
    }

    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return -1;
    }

    backgroundTexture = loadTexture("Tetris1.jpg", renderer);
    renderBackground(renderer, backgroundTexture);

    Settings settings;

    currentMusic = Mixer::loadMusicForLevel(level);
    if (currentMusic && Mix_PlayMusic(currentMusic, -1) == -1) {
        std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
        return -1;
    }
    Mix_VolumeMusic(volume);

    startScreenLoop();

    spawnNewPiece();

    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            handleInput(e);
        }
        if (showSettingsMenu) {
            renderSettingsMenu(renderer);
        }

        if (!showStartScreen)
            update();

        render();
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(backgroundTexture);
    SDL_Quit();

    return 0;
}

