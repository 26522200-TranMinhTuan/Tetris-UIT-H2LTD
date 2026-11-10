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

public:
    ConsoleRenderer(int width = 120, int height = 35);
    ~ConsoleRenderer();
};

#endif