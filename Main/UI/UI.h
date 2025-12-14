#pragma once


#include "../Game/Game.h"


#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"
#define CLEAR_SCREEN() system("cls")

class UI
{
public:
    //--------------------
    // PRINTING FUNCTIONS
    //--------------------
    
    static void UI_PrintHeader(const char* title);
    static void UI_PrintDivider();
    static void UI_PrintSection(const char* name);
    static void UI_PrintColored(const char* text, const char* color, bool newLine = false);
    static void UI_PrintCentered(const char* text);
    
    //--------------------
    // INPUT HANDLING FUNCTIONS
    //--------------------
    
    static unsigned short UI_GetMenuInput(short int minChoice, short int maxChoice);
    static char UI_GetCharInput();
    static void UI_GetStringInput(const char* prompt, char* buffer, int maxLength);
    static Direction UI_GetDirectionInput();
    static bool UI_ConfirmAction(const char* message);
    
    //--------------------
    // BAR DISPLAY FUNCTIONS
    //--------------------
    
    static void UI_DisplayHealthBar(unsigned short int current, unsigned short int max);
    static void UI_DisplayExperienceBar(unsigned short int current, unsigned short int max);
    static void UI_DisplayLoadingBar();
    //--------------------
    // ANIMATION FUNCTIONS
    //--------------------
    
    static void UI_DisplayLevelUpAnimation(unsigned short oldLevel, unsigned short newLevel);
    static void UI_DisplayCombatAnimation(const char* action, unsigned short damage, bool isCritical);
    static void UI_DisplayVictoryScreen(Player* player, GameStats* stats);
    static void UI_DisplayDefeatScreen(Player* player, GameStats* stats);
    static void UI_DisplayGameoverScreen(Player* player, GameStats* stats);
    
    //--------------------
    // TIMING FUNCTIONS
    //--------------------
    
    static void UI_PauseScreen();
    static void UI_TimedPause(unsigned short milliseconds);
    
    //--------------------
    // MESSAGE DISPLAY FUNCTIONS
    //--------------------
    
    static void UI_DisplaySuccessMessage(const char* message);
    static void UI_DisplayErrorMessage(const char* message);
    static void UI_DisplayWarningMessage(const char* message);
    static void UI_DisplayInfoMessage(const char* message);

};
