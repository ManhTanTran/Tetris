#ifndef TETRIS_H
#define TETRIS_H

#include <vector>  // Thêm thư viện vector
#include <SDL.h>

struct Block {
    int x, y;
};

struct Tetromino {
public:
    std::vector<Block> blocks;  // Định nghĩa đúng vector
    SDL_Color color;
    Block pivot;

    void move(int dx, int dy);
    void rotate();
    void drop();
    void storePiece();
    bool isColliding(const Tetromino& piece, int xOffset, int yOffset);
    bool isValidMove(const std::vector<Block>& newBlocks);
};

#endif


