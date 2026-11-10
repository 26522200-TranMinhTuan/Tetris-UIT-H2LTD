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
};