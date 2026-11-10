#include <iostream>
#include <fstream>
#include <conio.h>
#include <windows.h> 
#include <time.h>    
#include <thread>
#include <chrono>
#include "Renderer.h"

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

// --- HỆ THỐNG ÂM THANH RETRO (Trúc) ---
void playSoundAsync(int freq, int duration) {
    std::thread([freq, duration]() {
        Beep(freq, duration);
    }).detach();
}

void rotateBlock(char matrix[4][4]) {
    char temp[4][4];
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) temp[j][3 - i] = matrix[i][j];
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) matrix[i][j] = temp[i][j];
}

struct PlayerState {
    char board[H][W];
    int x, y, b;
    int next_b, hold_b;
    bool canHold;
    int score;
    int pendingGarbage;
    bool gameOver;
    int dropCounter;
    char currentShape[4][4]; 

    void reset() {
        score = 0;
        pendingGarbage = 0;
        gameOver = false;
        hold_b = -1;
        canHold = true;
        dropCounter = 0;
        x = W / 2 - 2;
        y = 1;
        b = rand() % 7;
        next_b = rand() % 7;
        memcpy(currentShape, blocks[b], sizeof(currentShape));

        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                if (i == 0 || i == H - 1 || j == 0 || j == W - 1) board[i][j] = '#';
                else board[i][j] = ' ';
            }
        }
    }
};

int highScore = 0;

int loadHighScore() {
    ifstream file("highscore.txt");
    int hs = 0;
    if (file.is_open()) { file >> hs; file.close(); }
    else { ofstream newFile("highscore.txt"); if (newFile.is_open()) { newFile << 0; newFile.close(); } }
    return hs;
}

void saveHighScore(int currentScore) {
    if (currentScore >= highScore) {
        highScore = currentScore;
        ofstream file("highscore.txt");
        if (file.is_open()) { file << highScore << endl; file.flush(); file.close(); }
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

bool canMove(PlayerState& p, int dx, int dy) {
    return canMoveCustom(p, dx, dy, p.currentShape);
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

int removeLine(PlayerState& p) {
    int linesCleared = 0;
    for (int i = H - 2; i > 0; i--) {
        bool isFull = true;
        for (int j = 1; j < W - 1; j++) {
            if (p.board[i][j] == ' ') { isFull = false; break; }
        }
        if (isFull) {
            linesCleared++;
            for (int ii = i; ii > 1; ii--) {
                for (int jj = 1; jj < W - 1; jj++) p.board[ii][jj] = p.board[ii - 1][jj];
            }
            for (int jj = 1; jj < W - 1; jj++) p.board[1][jj] = ' ';
            i++;
        }
    }

    if (linesCleared > 0) {
        if (linesCleared >= 4) {
            playSoundAsync(1000, 100); playSoundAsync(1200, 150); // Ăn Tetris
        } else {
            playSoundAsync(800, 150); // Ăn hàng thường
        }
        p.score += linesCleared * 100;
    } else {
        playSoundAsync(250, 100); // Âm thanh khóa gạch khi không ăn điểm
    }
    return linesCleared;
}

void lockAndSpawnNext(PlayerState& p) {
    lockBlockToBoard(p);
    removeLine(p);
    p.x = W / 2 - 2; 
    p.y = 1; 
    p.b = p.next_b; 
    p.next_b = rand() % 7; 
    memcpy(p.currentShape, blocks[p.b], sizeof(p.currentShape)); 
    p.canHold = true;
    p.dropCounter = 0;

    if (!canMove(p, 0, 0)) {
        p.gameOver = true;
        playSoundAsync(200, 500); // Âm thanh Game Over
    }
}

void getDisplayField(PlayerState& p, unsigned char* outField) {
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            char ch = p.board[r][c];
            if (ch == '#') outField[r * W + c] = 9;
            else if (ch >= 'A' && ch <= 'Z') outField[r * W + c] = (ch % 7) + 1;
            else outField[r * W + c] = 0;
        }
    }

    if (!p.gameOver) {
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (p.currentShape[i][j] != ' ') {
                    int boardY = p.y + i;
                    int boardX = p.x + j;
                    if (boardY >= 1 && boardY < H - 1 && boardX >= 1 && boardX < W - 1) {
                        outField[boardY * W + boardX] = (p.currentShape[i][j] % 7) + 1;
                    }
                }
            }
        }
    }
}

void renderSolo(ConsoleRenderer& renderer, PlayerState& p, int gameTimeSec, int level) {
    renderer.clearBuffer();
    unsigned char tempField[H * W] = { 0 };
    getDisplayField(p, tempField);
    renderer.drawBoardAt(tempField, W, H, 25, 3);
    renderer.drawUI(p.score, 0, p.next_b, p.hold_b, 0, gameTimeSec, level, p.gameOver);
    renderer.render();
}

void runSoloMode(ConsoleRenderer& renderer) {
    PlayerState p;
    p.reset();
    bool quitToMenu = false;

    auto startTime = std::chrono::steady_clock::now();
    renderSolo(renderer, p, 0, 1);

    while (!quitToMenu) {
        auto now = std::chrono::steady_clock::now();
        int gameTimeSec = (int)std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        int level = (gameTimeSec / 25) + 1;
        int dropSpeedTicks = max(3, 30 - (level - 1) * 3);

        if (p.gameOver) {
            renderSolo(renderer, p, gameTimeSec, level);
            char c = getch();
            char lowC = tolower(c);
            if (lowC == 'r') { 
                p.reset(); 
                startTime = std::chrono::steady_clock::now(); 
                renderSolo(renderer, p, 0, 1); 
                continue; 
            }
            if (lowC == 'q') break;
            continue;
        }

        bool hasMoved = false;
        while (kbhit()) {
            char c = getch();
            char lowC = tolower(c);

            if (lowC == 'a' && canMove(p, -1, 0)) { p.x--; hasMoved = true; playSoundAsync(400, 30); }
            else if (lowC == 'd' && canMove(p, 1, 0)) { p.x++; hasMoved = true; playSoundAsync(400, 30); }
            else if (lowC == 's' && canMove(p, 0, 1)) { p.y++; hasMoved = true; playSoundAsync(400, 30); }
            else if (c == ' ') { 
                playSoundAsync(350, 80); // Drop sound
                while (canMove(p, 0, 1)) p.y++; 
                lockAndSpawnNext(p);
                hasMoved = true;
                break;
            }
            else if (lowC == 'w') {
                char temp[4][4]; memcpy(temp, p.currentShape, sizeof(temp)); rotateBlock(temp);
                if (canMoveCustom(p, 0, 0, temp)) { 
                    memcpy(p.currentShape, temp, sizeof(p.currentShape)); 
                    hasMoved = true; 
                    playSoundAsync(600, 40); // Rotate sound
                }
            }
            else if (lowC == 'c' && p.canHold) {
                if (p.hold_b == -1) { p.hold_b = p.b; p.b = p.next_b; p.next_b = rand() % 7; }
                else { swap(p.b, p.hold_b); }
                memcpy(p.currentShape, blocks[p.b], sizeof(p.currentShape));
                p.x = W / 2 - 2; p.y = 1; p.canHold = false; hasMoved = true;
                playSoundAsync(500, 50); // Hold sound
            }
            else if (lowC == 'r') { p.reset(); startTime = std::chrono::steady_clock::now(); hasMoved = true; break; }
            else if (lowC == 'q') { quitToMenu = true; break; }
        }

        if (quitToMenu) break;

        if (!p.gameOver) {
            p.dropCounter++;
            if (p.dropCounter >= dropSpeedTicks) {
                p.dropCounter = 0;
                if (canMove(p, 0, 1)) {
                    p.y++;
                } else {
                    lockAndSpawnNext(p);
                }
                hasMoved = true;
            }
        }

        if (hasMoved || p.gameOver) {
            renderSolo(renderer, p, gameTimeSec, level);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    srand((unsigned int)time(0));
    ConsoleRenderer renderer(120, 35);

    bool running = true;
    while (running) {
        renderer.clearBuffer();
        renderer.drawMainMenu();
        renderer.render();

        char choice = getch();
        if (choice == '1') {
            runSoloMode(renderer);
        } else if (choice == '3' || choice == 'q' || choice == 'Q') {
            running = false;
        }
    }
    return 0;
}