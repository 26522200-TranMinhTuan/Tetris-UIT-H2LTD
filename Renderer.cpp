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
            } else if (index == 10) { // Dòng rác `X` - Khối nhiễu mờ
                screenBuffer[screenIdx] = L'\x2592';
                screenBuffer[screenIdx + 1] = L'\x2592';
                attrBuffer[screenIdx] = 7;
                attrBuffer[screenIdx + 1] = 7;
            }
        }
    }
}

void ConsoleRenderer::drawUI(int score, int highScore, int nextBlockId, int holdBlockId, int linesCleared, int gameTimeSec, int level, bool isGameOver) {
    int uiX = 70; 
    swprintf_s(&screenBuffer[2 * screenWidth + uiX], 30, L"T E T R I S  (SOLO)");
    swprintf_s(&screenBuffer[3 * screenWidth + uiX], 30, L"UIT - H2LTD");

    swprintf_s(&screenBuffer[5 * screenWidth + uiX], 30, L"TIME  : %02d:%02d", gameTimeSec / 60, gameTimeSec % 60);
    swprintf_s(&screenBuffer[6 * screenWidth + uiX], 30, L"LEVEL : %02d", level);
    swprintf_s(&screenBuffer[7 * screenWidth + uiX], 30, L"SCORE : %06d", score);
    swprintf_s(&screenBuffer[8 * screenWidth + uiX], 30, L"HIGH  : %06d", highScore);
}