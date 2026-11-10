#include "Renderer.h"
#include <stdio.h>

ConsoleRenderer::ConsoleRenderer(int width, int height) 
    : screenWidth(width), screenHeight(height), dwBytesWritten(0) {
    screenBuffer = new wchar_t[screenWidth * screenHeight];
    attrBuffer = new WORD[screenWidth * screenHeight]; 
    clearBuffer();

    hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);

    COORD coord = { (short)screenWidth, (short)screenHeight };
    SetConsoleScreenBufferSize(hConsole, coord);
    SMALL_RECT rectWindow = { 0, 0, (short)(screenWidth - 1), (short)(screenHeight - 1) };
    SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);
}

ConsoleRenderer::~ConsoleRenderer() {
    delete[] screenBuffer;
    delete[] attrBuffer;
    CloseHandle(hConsole);
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
            
            if (index == 9) { // Tường biên `#` - Đổi thành khối đặc màu xám tối
                screenBuffer[screenIdx] = L'\x2588'; 
                screenBuffer[screenIdx + 1] = L'\x2588';
                attrBuffer[screenIdx] = 8; 
                attrBuffer[screenIdx + 1] = 8;
            } else if (index == 10) { // Dòng rác `X` - Khối nhiễu mờ
                screenBuffer[screenIdx] = L'\x2592';
                screenBuffer[screenIdx + 1] = L'\x2592';
                attrBuffer[screenIdx] = 7;
                attrBuffer[screenIdx + 1] = 7;
            } else if (index > 0 && index < 9) { // Khối gạch - Đổi thành khối đặc màu
                screenBuffer[screenIdx] = L'\x2588';
                screenBuffer[screenIdx + 1] = L'\x2588';
                attrBuffer[screenIdx] = colors[index];
                attrBuffer[screenIdx + 1] = colors[index];
            }
        }
    }
}

void ConsoleRenderer::drawMiniBlock(int blockId, int startX, int startY) {
    if (blockId < 0 || blockId > 6) return; 
    WORD colors[] = { 11, 14, 13, 10, 12, 9, 6 }; 
    WORD color = colors[blockId];

    int shapes[7][4][4] = {
        {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, 
        {{0,0,0,0}, {0,1,1,0}, {0,1,1,0}, {0,0,0,0}}, 
        {{0,0,0,0}, {0,1,0,0}, {1,1,1,0}, {0,0,0,0}}, 
        {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}}, 
        {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}}, 
        {{0,0,0,0}, {1,0,0,0}, {1,1,1,0}, {0,0,0,0}}, 
        {{0,0,0,0}, {0,0,1,0}, {1,1,1,0}, {0,0,0,0}}  
    };

    for (int py = 0; py < 4; py++) {
        for (int px = 0; px < 4; px++) {
            if (shapes[blockId][py][px] == 1) {
                int screenIdx = (startY + py) * screenWidth + (startX + px * 2);
                screenBuffer[screenIdx] = L'\x2588'; // Đổi block preview thành khối đặc
                screenBuffer[screenIdx + 1] = L'\x2588';
                attrBuffer[screenIdx] = color;
                attrBuffer[screenIdx + 1] = color;
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
    
    swprintf_s(&screenBuffer[10 * screenWidth + uiX], 30, L"H O L D  (C)");
    drawMiniBlock(holdBlockId, uiX, 11); 

    swprintf_s(&screenBuffer[16 * screenWidth + uiX], 30, L"N E X T");
    drawMiniBlock(nextBlockId, uiX, 17); 
    
    swprintf_s(&screenBuffer[22 * screenWidth + uiX], 30, L"C O N T R O L S");
    swprintf_s(&screenBuffer[24 * screenWidth + uiX], 30, L"W     : Rotate 90deg");
    swprintf_s(&screenBuffer[25 * screenWidth + uiX], 30, L"A / D : Left / Right");
    swprintf_s(&screenBuffer[26 * screenWidth + uiX], 30, L"S     : Soft Drop");
    swprintf_s(&screenBuffer[27 * screenWidth + uiX], 30, L"SPACE : Hard Drop");
    swprintf_s(&screenBuffer[28 * screenWidth + uiX], 30, L"C     : Hold Block");
    swprintf_s(&screenBuffer[29 * screenWidth + uiX], 30, L"R     : RESTART Game");
    swprintf_s(&screenBuffer[30 * screenWidth + uiX], 30, L"Q     : Quit to Menu");

    if (isGameOver) {
        swprintf_s(&screenBuffer[13 * screenWidth + 28], 25, L"[ G A M E  O V E R ]");
        swprintf_s(&screenBuffer[15 * screenWidth + 26], 30, L"Press 'R' to Restart");
        swprintf_s(&screenBuffer[16 * screenWidth + 26], 30, L"Press 'Q' to Quit");
    }
}

void ConsoleRenderer::drawVersusUI(int score1, int hold1, int next1, int garbage1,
                                   int score2, int hold2, int next2, int garbage2,
                                   int gameTimeSec, int speedLevel,
                                   bool p1Dead, bool p2Dead) {
    swprintf_s(&screenBuffer[1 * screenWidth + 45], 35, L"T E T R I S  -  2P VERSUS");
    swprintf_s(&screenBuffer[2 * screenWidth + 44], 35, L"TIME: %02d:%02d  |  SPEED: Lvl %d", gameTimeSec / 60, gameTimeSec % 60, speedLevel);
    
    swprintf_s(&screenBuffer[4 * screenWidth + 2], 18, L"--- PLAYER 1 ---");
    swprintf_s(&screenBuffer[5 * screenWidth + 2], 18, L"SCORE: %06d", score1);
    swprintf_s(&screenBuffer[6 * screenWidth + 2], 18, L"GARBAGE: %d", garbage1);
    swprintf_s(&screenBuffer[8 * screenWidth + 2], 18, L"HOLD (C)");
    drawMiniBlock(hold1, 2, 9);
    swprintf_s(&screenBuffer[14 * screenWidth + 2], 18, L"NEXT");
    drawMiniBlock(next1, 2, 15);

    swprintf_s(&screenBuffer[4 * screenWidth + 100], 18, L"--- PLAYER 2 ---");
    swprintf_s(&screenBuffer[5 * screenWidth + 100], 18, L"SCORE: %06d", score2);
    swprintf_s(&screenBuffer[6 * screenWidth + 100], 18, L"GARBAGE: %d", garbage2);
    swprintf_s(&screenBuffer[8 * screenWidth + 100], 18, L"HOLD (O)");
    drawMiniBlock(hold2, 100, 9);
    swprintf_s(&screenBuffer[14 * screenWidth + 100], 18, L"NEXT");
    drawMiniBlock(next2, 100, 15);

    int botY = 29;
    swprintf_s(&screenBuffer[botY * screenWidth + 18], 40, L"P1: WASD | SPACE (Hard) | C (Hold)");
    swprintf_s(&screenBuffer[botY * screenWidth + 64], 40, L"P2: IJKL | ENTER (Hard) | O (Hold)");
    swprintf_s(&screenBuffer[(botY + 1) * screenWidth + 46], 30, L"R: Restart  |  Q: Menu");

    if (p1Dead) swprintf_s(&screenBuffer[15 * screenWidth + 23], 20, L"[ P1 KO! P2 WINS! ]");
    if (p2Dead) swprintf_s(&screenBuffer[15 * screenWidth + 65], 20, L"[ P2 KO! P1 WINS! ]");
}

void ConsoleRenderer::drawMainMenu() {
    int startX = 40;
    // Trang trí menu bằng khối vuông vức đẹp hơn
    swprintf_s(&screenBuffer[6 * screenWidth + startX], 40, L"████████████████████████████████████████");
    swprintf_s(&screenBuffer[7 * screenWidth + startX], 40, L"██    T E T R I S  -  U I T  H2LTD    ██");
    swprintf_s(&screenBuffer[8 * screenWidth + startX], 40, L"████████████████████████████████████████");
    
    swprintf_s(&screenBuffer[12 * screenWidth + startX], 40, L"    [ 1 ]  SOLO MODE  (Single Player)   ");
    swprintf_s(&screenBuffer[14 * screenWidth + startX], 40, L"    [ 2 ]  VERSUS MODE (2 Players 1v1)  ");
    swprintf_s(&screenBuffer[16 * screenWidth + startX], 40, L"    [ Q ]  EXIT GAME                    ");
    
    swprintf_s(&screenBuffer[20 * screenWidth + startX], 40, L"████████████████████████████████████████");
    swprintf_s(&screenBuffer[22 * screenWidth + startX], 40, L"    PRESS 1, 2 OR Q TO CHOOSE OPTION    ");
}

void ConsoleRenderer::render() {
    for (short i = 0; i < screenHeight; i++) {
        WriteConsoleOutputCharacterW(hConsole, &screenBuffer[i * screenWidth], screenWidth, { 0, i }, &dwBytesWritten);
        WriteConsoleOutputAttribute(hConsole, &attrBuffer[i * screenWidth], screenWidth, { 0, i }, &dwBytesWritten);
    }
}