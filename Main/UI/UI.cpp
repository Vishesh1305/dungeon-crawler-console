#include "UI.h"
#include <cstdio>
#include <cstring>
#include <windows.h>

//--------------------
// PRINT FUNCTIONS
//--------------------
void UI::UI_PrintHeader(const char* title)
{
    unsigned short len = static_cast<unsigned short>(strlen(title));
    unsigned short totalSpace = 38;
    unsigned short leftPadding = (totalSpace - len) / 2;
    unsigned short rightPadding = (totalSpace - len) - leftPadding;
    
    printf("+======================================+\n");
    printf("|");
    for (unsigned short i = 0; i < leftPadding; i++) printf(" ");
    printf("%s", title);
    for (unsigned short i = 0; i < rightPadding; i++) printf(" ");
    printf("|\n");
    printf("+======================================+\n");
    
}

void UI::UI_PrintDivider()
{
    printf("+======================================+\n");
}

void UI::UI_PrintSection(const char* name)
{
    printf("\n--- %s ", name);
    unsigned short remaining = 35 - static_cast<unsigned short>(strlen(name)) - 4;
    for (unsigned short i = 0; i < remaining; i++) printf("-");
    printf("\n");
}

void UI::UI_PrintColored(const char* text, const char* color, bool newLine)
{
    if (newLine)
    {
        printf("%s%s%s\n", color, text, RESET);
    }else
    {
        printf("%s%s%s", color, text, RESET);
    }
}

void UI::UI_PrintCentered(const char* text)
{
    unsigned short padding = (40 - static_cast<unsigned short>(strlen(text))) / 2;
    printf("%*s%s\n", padding, "", text);
}

//--------------------
// INPUT FUNCTIONS
//--------------------

unsigned short UI::UI_GetMenuInput(short int minChoice, short int maxChoice)
{
    unsigned short choice = 0;
    while (true)
    {
        printf("> ");
        if (scanf_s("%hd", &choice) != 1)
        {
            while (getchar() != '\n') {}
            printf("Invalid Input. Try Again: ");
            continue;
        }
        if (choice >= minChoice && choice <= maxChoice)
        {
            while (getchar() != '\n') {}
            return choice;
        }
        printf("Invalid Choice! Enter in range %d - %d: ", minChoice, maxChoice);
    }
}

char UI::UI_GetCharInput()
{
    char input;
    scanf_s("%c", &input, 1); // NOLINT(cert-err33-c)
    while (getchar() != '\n') {}
    return input;
}

void UI::UI_GetStringInput(const char* prompt, char* buffer, int maxLength)
{
    printf("%s", prompt);
    scanf_s("%49s", buffer, maxLength);  // NOLINT(cert-err33-c)
    while (getchar() != '\n') {}
}

Direction UI::UI_GetDirectionInput()
{
    printf("Direction: (1 = North, 2 = East, 3 = South, 4 = West): ");
    unsigned short choice = UI_GetMenuInput(1, 4);
    return static_cast<Direction>(choice - 1);
}

bool UI::UI_ConfirmAction(const char* message)
{
    while (true)
    {
        printf("%s (Y/N): ", message);
        char response = UI_GetCharInput();
        if (response == 'Y' || response == 'y')
        {
            return true;
        }
        else if (response == 'N' || response == 'n')
        {
            return false;
        }
        else
        {
            printf("%sInvalid Choice! Try Again: %s", RED, RESET);
        }
    }
}


//--------------------
// BAR DISPLAY FUNCTIONS
//--------------------

void UI::UI_DisplayHealthBar(unsigned short int current, unsigned short int max)
{
    float percent = static_cast<float>(current) / static_cast<float>(max);
    unsigned short barLength = 20;
    unsigned short fillLength = static_cast<unsigned short>(barLength * percent);
    printf("Health:[");
    
    for (unsigned short i = 0; i < fillLength; i++)
    {
        if (percent > 0.5f)
        {
            printf("%s%s%s", GREEN, "=", RESET);
        }else if (percent > 0.25f)
        {
            printf("%s%s%s", YELLOW, "=", RESET);
        }else
        {
            printf("%s%s%s", RED, "=", RESET);
        }
    }
    
    for (unsigned short i = fillLength; i < barLength; i++)
    {
        printf("-");
    }
    printf("]%d/%d (%.0f%%)\n", current, max, percent*100);
}

void UI::UI_DisplayExperienceBar(unsigned short int current, unsigned short int max)
{
    float percent = static_cast<float>(current) / static_cast<float>(max);
    unsigned short barLength = 20;
    unsigned short fillLength = static_cast<unsigned short>(barLength * percent);
    printf("Experience:[");
    for (unsigned short i = 0; i < fillLength; i++)
    {
        printf("%s%s%s", BLUE, "=", RESET);
    }
    for (unsigned short i = fillLength; i < barLength; i++)
    {
        printf("-");
    }
    printf("] %.0f%%\n", percent*100);
}

void UI::UI_DisplayLoadingBar()
{
    printf("[");
    for (unsigned short i = 0; i < 20; i++)
    {
        printf("=");
        fflush(stdout);
        UI_TimedPause(50);
    }
    printf("]100%%\n");
}


//--------------------
// ANIMATION FUNCTIONS
//--------------------

void UI::UI_DisplayLevelUpAnimation(unsigned short oldLevel, unsigned short newLevel)
{
    CLEAR_SCREEN();
    printf("\n\n");
    UI_PrintCentered("======================================");
    UI_PrintCentered("*** LEVEL UP! ***");
    printf(" Level %d -> Level %d\n", oldLevel, newLevel);
    UI_PrintCentered("======================================");
    printf("\n\n");
    
}

void UI::UI_DisplayCombatAnimation(const char* action, unsigned short damage, bool isCritical)
{
    if (isCritical)
    {
        printf("%s >>> CRITICAL HIT <<< \n%s", RED, RESET);
    }
    printf("%s Dealt %d Damage!\n", action, damage);
}

void UI::UI_DisplayVictoryScreen(Player* player, GameStats* stats)
{
    CLEAR_SCREEN();
    UI_PrintHeader("VICTORY!");
    printf("\n");
    printf("You defeated the final boss!\n");
    printf("Final Level: %d\n", player->level);
    printf("Final Gold: %d\n", player->gold);
    printf("Enemies Defeated: %d\n\n", stats->totalEnemiesDefeated);
}

void UI::UI_DisplayDefeatScreen(Player* player, GameStats* stats)
{
    CLEAR_SCREEN();
    UI_PrintHeader("DEALT DEFEAT!");
    printf("\n");
    printf("You have been defeated!\n");
    printf("Final Level: %d\n", player->level);
    printf("Final Gold: %d\n", player->gold);
    printf("Enemies Defeated: %d\n", stats->totalEnemiesDefeated);
}
void UI::UI_DisplayGameoverScreen(Player* player, GameStats* stats)
{
    UI_DisplayDefeatScreen(player, stats);
}

//--------------------
// TIMING FUNCTIONS
//--------------------

void UI::UI_PauseScreen()
{
    printf("\nPress ENTER to continue...");
    getchar();
}

void UI::UI_TimedPause(unsigned short milliseconds)
{
    Sleep(milliseconds);
}

//--------------------
// MESSAGE DISPLAY FUNCTIONS
//--------------------

void UI::UI_DisplaySuccessMessage(const char* message)
{
    printf("%s[SUCCESS]%s%s\n", GREEN, RESET, message);
}

void UI::UI_DisplayErrorMessage(const char* message)
{
    printf("%s[ERROR]%s%s\n", RED, RESET, message);
}

void UI::UI_DisplayWarningMessage(const char* message)
{
    printf("%s[WARNING]%s%s\n", YELLOW, RESET, message);
}

void UI::UI_DisplayInfoMessage(const char* message)
{
    printf("%s[INFO]%s%s\n", CYAN, RESET, message);
}








