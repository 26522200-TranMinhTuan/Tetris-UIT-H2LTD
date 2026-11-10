#include "Renderer.h"
#include <stdio.h>

ConsoleRenderer::ConsoleRenderer(int width, int height) 
    : screenWidth(width), screenHeight(height), dwBytesWritten(0) {
    screenBuffer = new wchar_t[screenWidth * screenHeight];
    attrBuffer = new WORD[screenWidth * screenHeight]; 

    hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    COORD coord = { (short)screenWidth, (short)screenHeight };
    SetConsoleScreenBufferSize(hConsole, coord);
    SMALL_RECT rectWindow = { 0, 0, (short)(screenWidth - 1), (short)(screenHeight - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);
}

void ConsoleRenderer::clearBuffer() {
    for (int i = 0; i < screenWidth * screenHeight; i++) {
        screenBuffer[i] = L' ';
        attrBuffer[i] = 15; 
    }
}