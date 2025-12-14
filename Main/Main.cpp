#include <cstdio>
#include <cstring>
#include <windows.h>
#include "UI/UI.h"

int main(int argc, char* argv[])
{

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    
    UI::UI_DisplayHealthBar(45, 100);
    UI::UI_DisplayExperienceBar(45, 100);
    
    
    return 0;
}
