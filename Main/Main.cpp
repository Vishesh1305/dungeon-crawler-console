#include <cstdio>
#include <windows.h>
#include "Game/Game.h"

int main(int argc, char* argv[])
{

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    
    GameInstance* game = GameInit();
    if (!game)
    {
        printf("Failed to initialize game\nExiting..\n");
        return 1;
    }
    
    GameRun(game);
    
    GameFree(game);
    
    return 0;
}
