#include <iostream>
#include <conio.h>
#include <windows.h> 
#include <time.h>    
using namespace std;
#define H 20
#define W 15
char board[H][W] = {};

void initBoard(){
    for (int i = 0 ; i < H ; i++)
        for (int j = 0 ; j < W ; j++)
            if (i == 0 || i == H-1 || j ==0 || j == W-1) board[i][j] = '#';
            else board[i][j] = ' ';
}

void draw(){
    system("cls"); 
    for (int i = 0 ; i < H ; i++, cout<<endl)
        for (int j = 0 ; j < W ; j++) cout<<board[i][j];
}