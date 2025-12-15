#include "Game.h"
#include "../UI/UI.h"
//--------------------
// GAME FUNCTION IMPLEMENTATION
//--------------------

GameInstance* GameInit()
{
    GameInstance* game = (GameInstance*)malloc(sizeof(GameInstance));
    if (game == nullptr)
    {
        printf("ERROR 01: Failed to allocate memory for Game Instance. Exiting.\n");
        return nullptr;
    }
    game->stats = (GameStats*)malloc(sizeof(GameStats));
    
    if (game->stats == nullptr)
    {
        printf("ERROR 02: Failed to allocate memory for GameStats. Exiting.\n");
        free(game);
        return nullptr;
    }
    memset(game->stats, 0, sizeof(GameStats));
    game->currentState = MAIN_MENU;
    game->isRunning = true;
    game->player = nullptr;
    game->dungeon = nullptr;
    game->inventory = nullptr;
    game->questLog = nullptr;
    game->enemyCount = 0;
    game->abilityCount = 0;
    game->shop = nullptr;
    
    //TODOs here 
    //Enemy initialization goes here
    //Ability initialization goes here
    //Game shop init goes here.
    
    return game;
    
}

void GameRun(GameInstance* game)
{
    while (game->isRunning)
    {
        CLEAR_SCREEN();
        switch (game->currentState)
        {
        case MAIN_MENU:
            {
                game->currentState = GameShowMainMenu();
                break;
            }
        case CHARACTER_CREATION:
            {
                GameHandleCharacterCreation(game);
                game->currentState = DIFFICULTY_SELECT;
                break;
            }
        case DIFFICULTY_SELECT:
            {
                GameHandleGameDifficultySelection(game);
                game->currentState = NEW_GAME;
                break;
            }
        case NEW_GAME:
            {
                //game->dungeon = DungeonInit();
                //game->questLog = QuestInit();
                //game->inventory = CreateInventory();
                //game->player->currentRoom = 0;
                game->currentState = GAME_LOOP;
                break;
            }
        case LOAD_GAME:
            {
                printf("Game Loaded... testing.\n");
                game->currentState = GAME_LOOP;
                
                break;
            }
        case GAME_LOOP:
            {
                GameHandleGameLoop(game);
                break;
            }
        case PAUSE_MENU:
            {
                GameHandlePauseMenu(game);
                break;
            }
        case GAME_OVER:
            {
                game->isRunning = false;
                break;
            }
        }
    }
}

void GameFree(GameInstance* game)
{
    if (game == nullptr)
    {
        return;
    }
    if (game->player != nullptr)
    {
        free(game->player);
        game->player = nullptr;
    }
    if (game->dungeon != nullptr)
    {
        free(game->dungeon);
        game->dungeon = nullptr;
    }
    if (game->inventory != nullptr)
    {
        InventoryNode* curr = game->inventory->head;
        while (curr != nullptr)
        {
            InventoryNode* temp = curr;
            curr = curr->next;
            free(temp);
        }
        free(game->inventory);
        game->inventory = nullptr;
    }
    if (game->questLog != nullptr)
    {
        free(game->questLog);
        game->questLog = nullptr;
    }
    if (game->shop != nullptr)
    {
        free(game->shop);
        game->shop = nullptr;
    }
    if (game->stats != nullptr)
    {
        free(game->stats);
        game->stats = nullptr;
    }
    free(game);
    game = nullptr;
}

GameState GameShowMainMenu()
{
    CLEAR_SCREEN();
    UI::UI_PrintHeader("DUNGEON CRAWLER");
    printf("\n");
    printf("  1. New Game\n");
    printf("  2. Load Game\n");
    printf("  3. Exit\n");
    printf("\n");
    UI::UI_PrintDivider();
    
    unsigned short choice = UI::UI_GetMenuInput(1, 3);
    
    if (choice == 1) return CHARACTER_CREATION;
    if (choice == 2) return LOAD_GAME;
    
    exit(0);   // NOLINT(concurrency-mt-unsafe)


}

void GameHandleCharacterCreation(GameInstance* game)
{
    printf("Character Creation - TODO\n");
}

void GameHandleGameDifficultySelection(GameInstance* game)
{
    printf("Difficulty Selection - TODO\n");
    
}

void GameHandleGameLoop(GameInstance* game)
{
    UI::UI_PrintDivider();
}

void GameHandlePauseMenu(GameInstance* game)
{
    printf("Pause Menu - todo\n");
}

//--------------------
// PLAYER FUNCTIONS
//--------------------

Player* PlayerCreate()
{
    Player* player = (Player*)calloc(1, sizeof(Player));
    
    if (player == nullptr)
    {
        printf("ERROR 03: Failed to allocate memory for player.\nExiting.\n");
        return nullptr;
    }
    
    player->goldMultiplier = 1.0f;
    player->expMultiplier = 1.0f;
    
    // Every thing else is already set to 0 because of calloc.
    
    return player;
}

void PlayerInitStats(Player* player)
{
    player->health = STARTING_HEALTH;
    player->maxHealth = STARTING_HEALTH;
    player->attack = STARTING_ATTACK;
    player->defense = STARTING_DEFENCE;
    player->exp = STARTING_XP;
    player->level = 1;
    player->gold = STARTING_GOLD;
    player->currentRoom = 0;
    
}
