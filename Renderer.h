#ifndef RENDERER_H
#define RENDERER_H

#include <windows.h>
#include <string>

class ConsoleRenderer {
private:
    int screenWidth;
    int screenHeight;
    HANDLE hConsole;
    wchar_t* screenBuffer;
    WORD* attrBuffer;
    DWORD dwBytesWritten;

    void drawMiniBlock(int blockId, int startX, int startY);

public:
    ConsoleRenderer(int width = 120, int height = 35);
    ~ConsoleRenderer();

    void clearBuffer();
    void drawBoardAt(const unsigned char* field, int fieldWidth, int fieldHeight, int offsetX, int offsetY);
    void drawUI(int score, int highScore, int nextBlockId, int holdBlockId, int linesCleared = 0, int gameTimeSec = 0, int level = 1, bool isGameOver = false);
    void drawVersusUI(int score1, int hold1, int next1, int garbage1,
                      int score2, int hold2, int next2, int garbage2,
                      int gameTimeSec, int speedLevel,
                      bool p1Dead, bool p2Dead);
    void drawMainMenu();
    void render();
};

#endif // RENDERER_H