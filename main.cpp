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

struct PlayerState {
    char board[H][W];
    int x, y, b;
    int score;
    bool gameOver;
    int dropCounter;

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