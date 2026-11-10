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

void ConsoleRenderer::drawBoardAt(const unsigned char* field, int fieldWidth, int fieldHeight, int offsetX, int offsetY) {
    WORD colors[] = { 0, 11, 14, 13, 10, 12, 9, 6, 15, 8, 7 };
    for (int x = 0; x < fieldWidth; x++) {
        for (int y = 0; y < fieldHeight; y++) {
            int index = field[y * fieldWidth + x];
            int screenIdx = (y + offsetY) * screenWidth + (x * 2 + offsetX); 

            if (index == 9) { // Tường biên `#`
                screenBuffer[screenIdx] = L'\x2588'; 
                screenBuffer[screenIdx + 1] = L'\x2588';
                attrBuffer[screenIdx] = 8; 
                attrBuffer[screenIdx + 1] = 8;
            } else if (index > 0 && index < 9) { // Khối gạch
                screenBuffer[screenIdx] = L'\x2588';
                screenBuffer[screenIdx + 1] = L'\x2588';
                attrBuffer[screenIdx] = colors[index];
                attrBuffer[screenIdx + 1] = colors[index];
            }
        }
    }
}