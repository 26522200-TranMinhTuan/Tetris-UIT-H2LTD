#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h> 
#include <time.h>    
#include <thread>
#include <chrono>

using namespace std;

#define H 25
#define W 14

// MẢNG MẪU HẰNG SỐ (CỐ ĐỊNH, KHÔNG BỊ BIẾN ĐỔI KHI XOAY)
const char blocks[7][4][4] = {
    {{' ','I',' ',' '},{' ','I',' ',' '},{' ','I',' ',' '},{' ','I',' ',' '}},
    {{' ',' ',' ',' '},{' ','O','O',' '},{' ','O','O',' '},{' ',' ',' ',' '}},
    {{' ',' ',' ',' '},{' ','T',' ',' '},{'T','T','T',' '},{' ',' ',' ',' '}},
    {{' ',' ',' ',' '},{' ','S','S',' '},{'S','S',' ',' '},{' ',' ',' ',' '}},
    {{' ',' ',' ',' '},{'Z','Z',' ',' '},{' ','Z','Z',' '},{' ',' ',' ',' '}},
    {{' ',' ',' ',' '},{'J',' ',' ',' '},{'J','J','J',' '},{' ',' ',' ',' '}},
    {{' ',' ',' ',' '},{' ',' ','L',' '},{'L','L','L',' '},{' ',' ',' ',' '}}
};

void rotateBlock(char matrix[4][4]) {
    char temp[4][4];
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) temp[j][3 - i] = matrix[i][j];
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) matrix[i][j] = temp[i][j];
}

struct PlayerState {
    char board[H][W];
    int x, y, b;
    int score;
    bool gameOver;
    int dropCounter;

    char currentShape[4][4];

    void reset() {
        score = 0;
        gameOver = false;
        dropCounter = 0;
        x = W / 2 - 2;
        y = 1;
        b = rand() % 7;

        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                if (i == 0 || i == H - 1 || j == 0 || j == W - 1) board[i][j] = '#';
                else board[i][j] = ' ';
            }
        }
    }
};

void updateGravity(PlayerState& p, int dropSpeedTicks) {
    if (!p.gameOver) {
        p.dropCounter++;
        if (p.dropCounter >= dropSpeedTicks) {
            p.dropCounter = 0;
            p.y++;
        }
    }
}

void handleHorizontalInput(PlayerState& p, char key) {
    if (key == 'a' || key == 'A') p.x--;
    else if (key == 'd' || key == 'D') p.x++;
}

void handleSoftDrop(PlayerState& p, char key) {
    if (key == 's' || key == 'S') p.y++;
}

void handleHardDrop(PlayerState& p, char key) {
    if (key == ' ') {
        while (p.y < H - 2) p.y++;
    }
}

bool canMoveCustom(PlayerState& p, int dx, int dy, char testBlock[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (testBlock[i][j] != ' ') {
                int xt = p.x + j + dx;
                int yt = p.y + i + dy;
                if (xt < 1 || xt >= W - 1 || yt < 1 || yt >= H - 1) return false;
                if (p.board[yt][xt] != ' ') return false;
            }
        }
    }
    return true;
}

bool tryRotate(PlayerState& p) {
    char temp[4][4];
    memcpy(temp, p.currentShape, sizeof(temp));
    rotateBlock(temp);
    if (canMoveCustom(p, 0, 0, temp)) {
        memcpy(p.currentShape, temp, sizeof(p.currentShape));
        return true;
    }
    return false;
}

void lockBlockToBoard(PlayerState& p) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (p.currentShape[i][j] != ' ') {
                int boardY = p.y + i;
                int boardX = p.x + j;
                if (boardY >= 1 && boardY < H - 1 && boardX >= 1 && boardX < W - 1) {
                    p.board[boardY][boardX] = p.currentShape[i][j];
                }
            }
        }
    }
}