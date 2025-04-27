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

// Cấu trúc của một khối Tetris
struct Tetromino {
    vector<Block> blocks;
    SDL_Color color;
    Block pivot;
    int shape[4][4][4];
    void draw(SDL_Renderer* renderer, int offsetX, int offsetY, int blockSize);

    bool isValidMove(const vector<Block>& newBlocks) {
        for (const auto &b : newBlocks) {
            // Cho phép các block có y âm (đang xuất hiện bên ngoài lưới trên)
            if (b.x < 0 || b.x >= COLS || b.y >= ROWS)
                return false;
            // Chỉ kiểm tra va chạm nếu block nằm trong lưới hiển thị (y >= 0)
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
        storePiece(*this);  // Lưu khối vào lưới khi chạm đáy
    }

    void rotate() {
    if (blocks.empty()) return; // Avoid errors if there are no blocks

    // Calculate new positions after rotation around the pivot
    vector<Block> newBlocks;
    for (const auto &b : blocks) {
        int newX = pivot.x - (b.y - pivot.y);
        int newY = pivot.y + (b.x - pivot.x);
        newBlocks.push_back({newX, newY});
    }

    // Check if the rotation is valid
    if (isValidMove(newBlocks)) {
        blocks = newBlocks;
        return;
    }
    std::cout << "Rotation failed without Wall Kick\n";
    return; // Tạm tắt xử lý Wall Kick


    // Wall Kick adjustments to handle blocked rotations
    vector<pair<int, int>> wallKicks = {
        {1, 0}, {-1, 0}, {0, -1}, {0, 1}, // Basic shifts
        {1, -1}, {-1, -1}, {1, 1}, {-1, 1} // Diagonal shifts
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

// Danh sách các Tetromino mẫu
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
    {0, 255, 255, 255}, // I - Cyan
    {0, 0, 255, 255},   // J - Blue
    {255, 165, 0, 255}, // L - Orange
    {255, 255, 0, 255}, // O - Yellow
    {0, 255, 0, 255},   // S - Green
    {160, 32, 240, 255},// T - Purple
    {255, 0, 0, 255}    // Z - Red
};

void shuffleBag() {
    bag.clear();
    for (int i = 0; i < 7; ++i) {
        bag.push_back(i);
    }
    std::shuffle(bag.begin(), bag.end(), g); // dùng engine g
    bagIndex = 0;
}

Tetromino createTetrominoFromId(int id) {
    Tetromino t;
    t.color = tetrominoColors[id];
    std::memcpy(t.shape, tetrominoShapes[id], sizeof(t.shape));

    // Khởi tạo blocks từ shape[0] (xoay đầu tiên)
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 4; ++x) {
            if (t.shape[0][y][x]) {
                t.blocks.push_back({x + 3, y}); // cộng 3 để căn giữa trong lưới
            }
        }
    }

    // Pivot mặc định là block đầu tiên
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

    // Duyệt qua bảng từ dưới lên trên để xóa dòng đầy
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool fullLine = true;

        // Kiểm tra xem dòng có đầy không
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 0) {
                fullLine = false;
                break;
            }
        }

        // Nếu dòng đầy, di chuyển các dòng phía trên xuống
        if (fullLine) {
            for (int yy = y; yy > 0; yy--) {
                for (int xx = 0; xx < BOARD_WIDTH; xx++) {
                    board[yy][xx] = board[yy - 1][xx];
                }
            }

            // Dọn dòng đầu tiên
            for (int xx = 0; xx < BOARD_WIDTH; xx++) {
                board[0][xx] = 0;
            }

            linesCleared++;
            y++;  // Tăng y để kiểm tra lại dòng hiện tại sau khi dịch chuyển

        }
    }
}



void spawnNewPiece() {
    currentPiece = nextPiece;
    nextPiece = getNextPieceFromBag(); // Dùng đúng bag

    // Đặt pivot (nếu bạn dùng pivot để xoay)
    currentPiece.pivot = {5, 1};

    // Set vị trí khối mới
    currentX = 3; // giữa lưới 10 cột
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
        spawnNewPiece(); // Sinh khối mới từ nextPiece
    } else {
        std::swap(currentPiece, holdPiece);
        currentX = 3;
        currentY = 0;
        currentRotation = 0;

        // Cập nhật lại blocks từ shape (frame 0)
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
    // Màu chính của khối
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Viền sáng (trên & trái)
    SDL_SetRenderDrawColor(renderer,
        std::min(color.r + 60, 255),
        std::min(color.g + 60, 255),
        std::min(color.b + 60, 255), 255);

    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w - 1, rect.y); // top
    SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h - 1); // left

    // Bóng tối (dưới & phải)
    SDL_SetRenderDrawColor(renderer,
        std::max(color.r - 60, 0),
        std::max(color.g - 60, 0),
        std::max(color.b - 60, 0), 255);

    SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h - 1, rect.x + rect.w - 1, rect.y + rect.h - 1); // bottom
    SDL_RenderDrawLine(renderer, rect.x + rect.w - 1, rect.y, rect.x + rect.w - 1, rect.y + rect.h - 1); // right
}


void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // Vẽ đường dọc cho lưới chơi (chỉ vẽ khu vực GRID_WIDTH)
    for (int i = 0; i <= COLS; i++) {
        int x = i * BLOCK_SIZE;
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }

    // Vẽ đường ngang
    for (int i = 0; i <= ROWS; i++) {
        int y = i * BLOCK_SIZE;
        SDL_RenderDrawLine(renderer, 0, y, COLS * BLOCK_SIZE, y);
    }

    // Vẽ các khối 3D trên lưới
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            // Tạo một SDL_Rect để đại diện cho ô
            SDL_Rect rect;
            rect.x = col * BLOCK_SIZE;
            rect.y = row * BLOCK_SIZE;
            rect.w = BLOCK_SIZE;
            rect.h = BLOCK_SIZE;

            // Vẽ khối 3D tại vị trí đó
            SDL_Color blockColor = grid[row][col];  // Giả sử grid[row][col] là SDL_Color
            draw3DBlock(renderer, rect, blockColor);
        }
    }
}

void drawTetromino(const Tetromino& piece, SDL_Renderer* renderer) {
    // Duyệt qua các ô trong shape của Tetromino
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (piece.shape[currentRotation][i][j]) {
                // Tính toán vị trí x, y dựa trên currentX, currentY và BLOCK_SIZE
                int x = (currentX + j) * BLOCK_SIZE;
                int y = (currentY + i) * BLOCK_SIZE;

                // Tạo một SDL_Rect cho khối tại vị trí (x, y)
                SDL_Rect rect;
                rect.x = x;
                rect.y = y;
                rect.w = BLOCK_SIZE;
                rect.h = BLOCK_SIZE;

                // Vẽ khối 3D tại vị trí này với màu sắc của Tetromino
                draw3DBlock(renderer, rect, piece.color);  // Chú ý việc truyền 'renderer' ở đây
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
    int linesCleared = 0;  // Đếm số dòng đã xóa

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
            linesCleared++;  // Tăng số dòng đã xóa
        }
    }

    // Xóa phần còn lại
    for (int y = newRow; y >= 0; y--) {
        for (int x = 0; x < COLS; x++) {
            grid[y][x].a = 0;
        }
    }

    // Cập nhật điểm và level
    lines += linesCleared;          // Cộng tổng số dòng đã xóa
    score += linesCleared * 100;     // Cộng điểm
    level = 1 + lines / 10;          // Level tăng sau mỗi 10 dòng
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
    SDL_RenderCopy(renderer, background, NULL, NULL); // Vẽ full màn hình
}


void drawStartScreen() {

    SDL_Color white = {255, 255 , 255, 255};
    TTF_Font* font = TTF_OpenFont("arialbd.ttf", 48);

    int alpha = (SDL_GetTicks() / 500) % 2 == 0 ? 255 : 50;  // Nhấp nháy mỗi 500ms

    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "TETRIS", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_SetTextureAlphaMod(titleTexture, alpha); // Thay đổi độ trong suốt


    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 100, 50, 200, 50};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Tạo bề mặt chữ "Press ENTER to Play"
    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Let's Play", white);
    SDL_Texture* playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_SetTextureAlphaMod(playTexture, alpha);

    // Tính toán vị trí căn giữa (phía dưới chữ TETRIS)
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
        renderBackground(renderer, backgroundTexture); // Vẽ ảnh nền
        drawStartScreen(); // Vẽ UI của màn hình Start
        SDL_RenderPresent(renderer); // **Bắt buộc** để hiển thị
        SDL_Delay(16);
    }
}

void resetGame() {
    // Xóa lưới
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLS; ++j)
            grid[i][j] = {0, 0, 0};  // hoặc màu nền

    // Reset các biến
    score = 0;
    level = 1;
    linesCleared = 0;
    gameOver = false;

    // Reset bag, hold
    bag.clear();
    shuffleBag();
    holdUsedThisTurn = false;
    hasHoldPiece = false;

    // Spawn khối đầu tiên
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
            // Nếu đang ở menu settings
            switch (e.key.keysym.sym) {
                case SDLK_s:
                    showSettingsMenu = false; // Bấm S để thoát settings
                    paused = false;  // Tiếp tục game khi thoát settings
                    break;
                case SDLK_EQUALS:
                    Mix_VolumeMusic(MIX_MAX_VOLUME); // Tăng âm lượng tối đa
                    break;
                case SDLK_MINUS:
                    Mix_VolumeMusic(MIX_MAX_VOLUME / 4); // Giảm âm lượng
                    break;
                case SDLK_m:  // Bấm M để tắt/bật âm thanh
                    if (isMuted) {
                        isMuted = false;
                        Mix_VolumeMusic(MIX_MAX_VOLUME); // Bật âm thanh
                    } else {
                        isMuted = true;
                        Mix_VolumeMusic(0); // Tắt âm thanh
                    }
                    break;
            }
        }
        else {
            if (e.key.keysym.sym == SDLK_s) {
                showSettingsMenu = true;  // Bấm S để vào menu settings
                paused = true;  // Dừng game khi vào settings
                return;  // Tránh xử lý các phím khác nếu đang ở menu settings
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

    // Khi thua
    if (gameOver && !musicPlayed) {
        Mixer::haltMusic();
        Mixer::playMusic("win_theme.mp3");
        musicPlayed = true;
    }

    // Khi thắng
    if (level >= 5 && !gameWin) {
        gameWin = true;
        Mixer::haltMusic();
        Mixer::playMusic("win_theme.mp3");
        musicPlayed = true;
        return; // Không update nữa nếu đã thắng
    }

    if (gameOver || gameWin) return;

    Uint32 currentTime = SDL_GetTicks();

    // Nếu đang chạm đáy
    if (checkCollision(currentX, currentY + 1, currentRotation)) {
        if (!onGround) {
            onGround = true;
            lockTimerStart = currentTime;
        } else if (currentTime - lockTimerStart >= lockDelay) {
            currentPiece.storePiece(currentPiece);
            onGround = false;
        }
    } else {
        // Vẫn rơi tự do
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

    TTF_Font* font = TTF_OpenFont("BAUHS93.ttf", 48); // Load font Arial cỡ 48
    if (!font) {
        cout << "Không thể load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Color textColor = {255, 0, 0, 255}; // Màu chữ đỏ
    SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER!", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW = surface->w, textH = surface->h;
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};

    // Vẽ nền phía sau chữ (màu đen với độ trong suốt)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);  // Màu xám, độ trong suốt 200
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

    TTF_Font* font = TTF_OpenFont("BAUHS93.ttf", 48); // Load font Arial cỡ 48
    if (!font) {
        cout << "Không thể load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Color textColor = {255, 0, 0, 255}; // Màu chữ đỏ
    SDL_Surface* surface = TTF_RenderText_Solid(font, "YOU WIN!", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW = surface->w, textH = surface->h;
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};

    // Vẽ nền phía sau chữ (màu đen với độ trong suốt)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);  // Màu xám, độ trong suốt 200
    SDL_Rect bgRect = {textRect.x - 20, textRect.y - 10, textRect.w + 40, textRect.h + 20};
    SDL_RenderFillRect(renderer, &bgRect);

    // Vẽ chữ lên trên nền
    SDL_RenderCopy(renderer, texture, NULL, &textRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}

void renderSettingsMenu(SDL_Renderer* renderer) {
    // Tính toán vị trí căn giữa khung
    const int SETTINGS_WIDTH = 350;  // Chiều rộng khung
    const int SETTINGS_HEIGHT = 200; // Chiều cao khung

    int settingsX = (SCREEN_WIDTH - SETTINGS_WIDTH) / 2;  // Vị trí căn giữa theo chiều ngang
    int settingsY = (SCREEN_HEIGHT - SETTINGS_HEIGHT) / 2; // Vị trí căn giữa theo chiều dọc

    // Vẽ nền settings (khung nhỏ hơn)
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255); // xám đậm
    SDL_Rect settingsRect = {settingsX, settingsY, SETTINGS_WIDTH, SETTINGS_HEIGHT};
    SDL_RenderFillRect(renderer, &settingsRect);

    // Tạo màu chữ trắng
    SDL_Color white = {255, 255, 255, 255};

    // Vẽ chữ "Settings" ở giữa khung
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, "Settings", white);
    SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect messageRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2, // Căn giữa theo chiều ngang
        settingsY + 20, // Cách mép trên khung một chút
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &messageRect);

    // Vẽ hướng dẫn tăng âm lượng
    surfaceMessage = TTF_RenderText_Solid(font, "Press + to increase volume", white);
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect increaseVolumeRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2, // Căn giữa theo chiều ngang
        settingsY + 60, // Cách chữ "Settings" một chút
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &increaseVolumeRect);

    // Vẽ hướng dẫn giảm âm lượng
    surfaceMessage = TTF_RenderText_Solid(font, "Press - to decrease volume", white);
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect decreaseVolumeRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2, // Căn giữa theo chiều ngang
        settingsY + 100, // Cách chữ "Press + to increase volume" một chút
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &decreaseVolumeRect);
    surfaceMessage = TTF_RenderText_Solid(font, "Press M to turn off/on volume", white);
    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect turnonoroffVolumeRect = {
        settingsX + (SETTINGS_WIDTH - surfaceMessage->w) / 2, // Căn giữa theo chiều ngang
        settingsY + 140, // Cách chữ "Settings" một chút
        surfaceMessage->w, surfaceMessage->h
    };
    SDL_RenderCopy(renderer, Message, NULL, &turnonoroffVolumeRect);

    // Giải phóng tài nguyên
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
        renderSettingsMenu(renderer); // <- GỌI VÀO ĐÂY
        SDL_RenderPresent(renderer);   // <- Và nhớ Present
        return;
    }

    // Xóa màn hình
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);

    // Vẽ nền
    renderBackground(renderer, backgroundTexture);

    // Vẽ lưới
    drawGrid();

    // Vẽ khối hiện tại với block 3D
    drawTetromino(currentPiece, renderer);

    // Màu chữ
    SDL_Color white = {255, 255, 255, 255};

    // Tạo text renderer (giả sử bạn đã khởi tạo TTF_Init từ trước!)
    static TextRenderer text(renderer); // Chỉ khởi tạo một lần

    int infoX = SCREEN_WIDTH - 100; // Vị trí lề phải
    int y = 30;

    // Hiển thị thông tin điểm số, level, dòng
    text.drawText("Score: " + std::to_string(score), infoX, y, white);
    y += 30;
    text.drawText("Level: " + std::to_string(level), infoX, y, white);
    y += 30;

    // Hiển thị khối tiếp theo
    text.drawText("Next Piece", infoX, y, white);
    // Tính toán vị trí căn giữa của khối tiếp theo
    int nextPieceY = y + 30;
    drawMiniTetromino(nextPiece, infoX-30, nextPieceY);
    y += 90;

    // Hiển thị khối đang giữ
    text.drawText("Hold Piece", infoX, y, white);
    // Tính toán vị trí căn giữa của khối hold
    int holdPieceY = y + 30;
    if (hasHoldPiece) {
        drawMiniTetromino(holdPiece, infoX - 30, holdPieceY);
    }

    // Nếu trò chơi bị tạm dừng, hiển thị chữ "Paused" lấp lánh
    if (paused) {
        static Uint32 lastTime = SDL_GetTicks(); // Thời gian lần render trước
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime > 500) { // Mỗi nửa giây thay đổi màu
            lastTime = currentTime;
        }

        // Tạo hiệu ứng lấp lánh bằng cách thay đổi độ trong suốt của chữ
        Uint8 alpha = (currentTime / 100) % 2 == 0 ? 255 : 0;  // Thay đổi giữa 255 và 0

        SDL_Color pausedColor = {255, 255, 255, alpha};
        text.drawText("PAUSED", SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 50, pausedColor);
    }
    // Hiển thị tất cả
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    // Khởi tạo SDL
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

    // Khởi tạo cửa sổ và renderer
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font
    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cout << "Failed to load font: " << TTF_GetError() << std::endl;
        return -1;
    }

    // Load background
    backgroundTexture = loadTexture("Tetris1.jpg", renderer);
    renderBackground(renderer, backgroundTexture);

    Settings settings;

    // Bắt đầu chơi nhạc
    currentMusic = Mixer::loadMusicForLevel(level);
    if (currentMusic && Mix_PlayMusic(currentMusic, -1) == -1) {
        std::cerr << "Mix_PlayMusic Error: " << Mix_GetError() << std::endl;
        return -1;
    }
    Mix_VolumeMusic(volume); // Set âm lượng nhạc theo biến volume luôn!

    // Hiển thị màn hình bắt đầu
    startScreenLoop();

    // Random seed và spawn mảnh đầu tiên
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

    // Dọn tài nguyên khi kết thúc
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(backgroundTexture);
    SDL_Quit();

    return 0;
}

