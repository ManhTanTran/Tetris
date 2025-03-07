#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "def.h"

using namespace std;

struct Block {
    int x, y;
};

// Cấu trúc của một khối Tetris
struct Tetromino {
    vector<Block> blocks;
    SDL_Color color;
    Block pivot;

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
    bool isColliding(const Tetromino& piece, int dx, int dy);
    void storePiece(const Tetromino& piece);


    void move(int dx, int dy) {
        vector<Block> newBlocks = blocks;
        for (auto &b : newBlocks) {
            b.x += dx;
            b.y += dy;
        }
        if (isValidMove(newBlocks))
            blocks = newBlocks;
    }

    void drop() {
        while (!isColliding(*this, 0, 1)) {
            move(0, 1);
        }
        storePiece(*this);  // Lưu khối vào lưới khi chạm đáy
    }

    void rotate() {
        vector<Block> newBlocks;
        // Tính toán vị trí mới sau khi xoay quanh pivot
        for (const auto &b : blocks) {
            int newX = pivot.x - (b.y - pivot.y);
            int newY = pivot.y + (b.x - pivot.x);
            newBlocks.push_back({newX, newY});
        }
        // Nếu xoay hợp lệ thì cập nhật luôn
        if (isValidMove(newBlocks)) {
            blocks = newBlocks;
            return;
        }
        // Nếu không hợp lệ, thử Wall Kick: dịch chuyển nhẹ để tìm vị trí hợp lệ
        vector<pair<int, int>> wallKicks = {
            {1, 0}, {-1, 0}, {0, -1}, {0, 1},
            {1, -1}, {-1, -1}, {1, 1}, {-1, 1},
            {2, 0}, {-2, 0}, {0, -2}, {0, 2}
        };
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
        // Nếu xoay vẫn không hợp lệ, giữ nguyên trạng thái ban đầu (không xoay)
    }
};

Tetromino currentPiece;
Tetromino nextPiece;

// Danh sách các Tetromino mẫu
vector<Tetromino> tetrominoes = {
    // Khối I
    {{{3, 0}, {4, 0}, {5, 0}, {6, 0}}, {0, 255, 255, 255}, {4, 0}},
    // Khối O
    {{{4, 0}, {5, 0}, {4, 1}, {5, 1}}, {255, 255, 0, 255}, {4, 0}},
    // Khối T
    {{{4, 0}, {5, 0}, {6, 0}, {5, 1}}, {255, 0, 255, 255}, {5, 0}},
    // Khối L
    {{{4, 0}, {5, 0}, {6, 0}, {6, 1}}, {255, 165, 0, 255}, {5, 0}},
    // Khối J
    {{{4, 0}, {5, 0}, {6, 0}, {4, 1}}, {0, 0, 255, 255}, {5, 0}},
    // Khối S
    {{{5, 0}, {6, 0}, {4, 1}, {5, 1}}, {0, 255, 0, 255}, {5, 0}},
    // Khối Z
    {{{4, 0}, {5, 0}, {5, 1}, {6, 1}}, {255, 0, 0, 255}, {5, 0}}
};

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

void spawnNewPiece() {
    srand(time(0));
    int index = rand() % tetrominoes.size();
    currentPiece = tetrominoes[index];
}

void drawGrid() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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
}

void drawStoredBlocks() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            if (grid[y][x].a != 0) {
                SDL_SetRenderDrawColor(renderer, grid[y][x].r, grid[y][x].g, grid[y][x].b, 255);
                SDL_Rect rect = {x * BLOCK_SIZE, y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void drawTetromino(const Tetromino& piece) {
    SDL_SetRenderDrawColor(renderer, piece.color.r, piece.color.g, piece.color.b, piece.color.a);
    for (const auto &block : piece.blocks) {
        SDL_Rect rect = {block.x * BLOCK_SIZE, block.y * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &rect);
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
    score += linesCleared * 100;
    lines += linesCleared;
    level = 1 + lines / 10;  // Level tăng mỗi khi xóa 10 dòng
}



void storePiece() {
    for (const auto &b : currentPiece.blocks) {
        if (b.y >= 0 && b.y < ROWS && b.x >= 0 && b.x < COLS)
            grid[b.y][b.x] = currentPiece.color;
    }
    clearFullRows();
    spawnNewPiece();
    if (isGameOver()) {
        gameOver = true;
    }
}

void renderScore() {
    TTF_Font* font = TTF_OpenFont("arialbd.ttf", 24);
    if (!font) {
        cout << "Failed to load font: " << TTF_GetError() << endl;
        return;
    }

    SDL_Color textColor = {0, 0, 0, 255};

    string scoreText = "Score: " + to_string(score);
    SDL_Surface* surface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    if (!surface) {
        cout << "Failed to create surface: " << TTF_GetError() << endl;
        TTF_CloseFont(font);
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        cout << "Failed to create texture: " << SDL_GetError() << endl;
        SDL_FreeSurface(surface);
        TTF_CloseFont(font);
        return;
    }

    SDL_Rect rect = {COLS * BLOCK_SIZE + 20, 10, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void renderNextPiece(SDL_Renderer* renderer, Tetromino nextPiece) {
    int startX = 320; // Tọa độ X của Next Piece
    int startY = 150; // Tọa độ Y của Next Piece

    SDL_Color color = nextPiece.color;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);

    for (int i = 0; i < 4; i++) {
        int x = startX + nextPiece.blocks[i].x * BLOCK_SIZE;
        int y = startY + nextPiece.blocks[i].y * BLOCK_SIZE;
        SDL_Rect rect = {x, y, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }
}


void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color textColor = {0, 0, 0, 255}; // Màu đen
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderUI(SDL_Renderer* renderer) {
    TTF_Font* font = TTF_OpenFont("arialbd.ttf", 24); // Load font với kích thước 24
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return;
    }

    renderText(renderer, font, "Next Piece", 310, 100);
    renderText(renderer, font, "Level: 1", 310, 180);
    renderText(renderer, font, "Lines: 0", 310, 230);

    TTF_CloseFont(font); // Giải phóng font sau khi vẽ xong
}




void drawStartScreen() {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_RenderClear(renderer);

    TTF_Font* font = TTF_OpenFont("arial.ttf", 48);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(font, "TETRIS", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {SCREEN_WIDTH / 2 - 100, 100, 200, 50};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_Surface* playSurface = TTF_RenderText_Solid(font, "Press ENTER to Play", white);
    SDL_Texture* playTexture = SDL_CreateTextureFromSurface(renderer, playSurface);
    SDL_Rect playRect = {SCREEN_WIDTH / 2 - 150, 200, 300, 40};
    SDL_RenderCopy(renderer, playTexture, NULL, &playRect);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_FreeSurface(playSurface);
    SDL_DestroyTexture(playTexture);
    TTF_CloseFont(font);
}

void handleInput(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN) {
        if (showStartScreen) {
            if (e.key.keysym.sym == SDLK_RETURN) {
                showStartScreen = false;
            }
        } else {
            switch (e.key.keysym.sym) {
                case SDLK_LEFT:  currentPiece.move(-1, 0); break;
                case SDLK_RIGHT: currentPiece.move(1, 0); break;
                case SDLK_DOWN:  currentPiece.move(0, 1); break;
                case SDLK_UP:    currentPiece.rotate(); break;
                case SDLK_SPACE: currentPiece.drop(); break;
            }
        }
    }
}

void update() {
    if (gameOver) return;

    // Điều chỉnh tốc độ rơi dựa trên level
    int speedMultiplier = max(1, 2 - level / 5);  // Level càng cao, tốc độ càng nhanh
    Uint32 currentDropDelay = DROP_DELAY / speedMultiplier;

    Uint32 currentTime = SDL_GetTicks();
    if (currentTime > lastDropTime + currentDropDelay) {
        if (!isColliding(currentPiece, 0, 1)) {
            currentPiece.move(0, 1);
        } else {
            storePiece();
        }
        lastDropTime = currentTime;
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

    SDL_Color textColor = {255, 0, 0, 255}; // Màu đỏ
    SDL_Surface* surface = TTF_RenderText_Solid(font, "GAME OVER!", textColor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    int textW = surface->w, textH = surface->h;
    SDL_Rect textRect = {SCREEN_WIDTH / 2 - textW / 2, SCREEN_HEIGHT / 2 - textH / 2, textW, textH};

    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);

    SDL_RenderPresent(renderer);
}


void render() {
    if (gameOver) {
        drawGameOverScreen();
        return;
    }
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderClear(renderer);
    drawGrid();
    drawStoredBlocks();
    drawTetromino(currentPiece);
    renderScore(); // Vẽ cột điểm bên phải
    renderUI(renderer);

    SDL_RenderPresent(renderer);

}

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    window = SDL_CreateWindow("Tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        cout << "Failed to load font: " << TTF_GetError() << endl;
        return -1;
    }
    srand(SDL_GetTicks());
    spawnNewPiece();
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;
            handleInput(e);
        }
        if (!showStartScreen) update();
        render();
        SDL_Delay(16);
    }
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
