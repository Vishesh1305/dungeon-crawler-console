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
    if (player == nullptr)
    {
        printf("ERROR - PlayerInitStats: Player is null\n");
        return;
    }
    player->health = STARTING_HEALTH;
    player->maxHealth = STARTING_HEALTH;
    player->attack = STARTING_ATTACK;
    player->defense = STARTING_DEFENCE;
    player->exp = STARTING_XP;
    player->level = 1;
    player->gold = STARTING_GOLD;
    player->currentRoom = 0;
    
}

void PlayerDisplayStats(Player* player)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerDisplayStats: Player is null\n");
        return;
    }
    
    CLEAR_SCREEN();
    UI::UI_PrintHeader("CHARACTER STATS");
    printf("\n");
    printf("Name: %s\n", player->name);
    printf("Level: %hu\n", player->level);
    printf("Trait: %s\n", PlayerGetTraitName(player->trait));
    printf("\n");
    UI::UI_PrintSection("HEALTH & VITALITY");
    UI::UI_DisplayHealthBar(player->health, player->maxHealth);
    UI::UI_DisplayExperienceBar(player->exp, XP_PER_LEVEL);
    printf("\n");
    UI::UI_PrintSection("COMBAT STATS");
    printf("Attack: %hu\n", player->attack);
    printf("Defence: %hu\n", player->defense);
    UI::UI_PrintSection("RESOURCES");
    printf("Gold: %hu\n", player->gold);
    printf("Current Room: %hu\n", player->currentRoom);
}

void PlayerDisplayStatusBar(Player* player)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerDisplayStatusBar: Player is null\n");
        return;
    }
    
    printf("%s", BOLD);
    UI::UI_PrintDivider();
    printf("%s", RESET);
    
    printf("%s [LVL: %hu]", player->name, player->level);
    UI::UI_DisplayHealthBar(player->health, player->maxHealth);
    printf("| ");
    UI::UI_DisplayExperienceBar(player->exp, XP_PER_LEVEL);
    printf("Gold: %hu\n", player->gold);
    
    printf("%s", BOLD);
    UI::UI_PrintDivider();
    printf("%s", RESET);
}

void PlayerLevelup(Player* player)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerLevelup: Player is null\n");
        return;
    }
    
    UI::UI_DisplayLevelUpAnimation(player->level, player->level + 1);
    
    unsigned short oldHealth = player->maxHealth;
    unsigned short oldAttack = player->attack;
    unsigned short oldDefense = player->defense;
    
    player->level += 1;
    player->maxHealth += HP_LEVEL_GAIN;
    player->health = player->maxHealth;
    player->attack += ATTACK_LEVEL_GAIN;
    player->defense += DEFENCE_LEVEL_GAIN;
    player->exp = 0;
    
    UI::UI_PrintSection("STAT INCREASES");
    printf("Max Health: %hu -> %hu\n", oldHealth, player->maxHealth);
    printf("Attack: %hu -> %hu\n", oldAttack, player->attack);
    printf("Defense: %hu -> %hu\n", oldDefense, player->defense);
    
    UI::UI_PauseScreen();
}

void PlayerDamage(Player* player, unsigned short damage)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerDamage: Player is null\n");
        return;
    }
    player->health -= damage;
}

void PlayerHeal(Player* player, unsigned short heal)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerHeal: Player is null\n");
        return;
    }
    
    player->health += heal;
    if (player->health > player->maxHealth) player->health = player->maxHealth;  // NOLINT 
    
}

void PlayerApplyTrait(Player* player, PlayerTrait trait)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerApplyTrait: Player is null\n");
        exit(1);
    }
    switch (trait)
    {
    case TRAIT_HEAVY_ARMOUR:
        player->defense = (unsigned short)(player->defense * 1.3);
        player->attack = (unsigned short)(player->attack * 0.9);
        break;
    
    case TRAIT_QUICK_HANDS:
        player->attack = (unsigned short)(player->attack * 1.2);
        player->goldMultiplier = 1.15f;
        break;
    
    case TRAIT_FORTUNATE:
        player->goldMultiplier = 1.50f;
        break;
    
    case TRAIT_SCHOLARLY:
        player->expMultiplier = 1.25f;
        break;
    
    case TRAIT_BEAST_MASTER:
        player->canCharmEnemies = true;
        break;
    }
    player->trait = trait;
}

void PlayerSelectTrait(Player* player)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerSelectTrait: Player is null\n");
        return;
    }
    
    PlayerTrait selectedTrait = {};
    bool isConfirmed = false;
    while (!isConfirmed)
    {
        CLEAR_SCREEN();
        UI::UI_PrintHeader("CHOOSE YOUR TRAIT");
        UI::UI_PrintSection("Traits change your playstyle. Choose carefully!");
        UI::UI_PrintDivider();
        
        printf("1) %s\n", PlayerGetTraitName(TRAIT_HEAVY_ARMOUR));
        printf("2) %s\n", PlayerGetTraitName(TRAIT_QUICK_HANDS));
        printf("3) %s\n", PlayerGetTraitName(TRAIT_FORTUNATE));
        printf("4) %s\n", PlayerGetTraitName(TRAIT_SCHOLARLY));
        printf("5) %s\n", PlayerGetTraitName(TRAIT_BEAST_MASTER));
        
        UI::UI_PrintDivider();
        printf("Your choice (1-5): ");
        unsigned short choice = UI::UI_GetMenuInput(1, 5);
        
        switch (choice) //NOLINT
        {
        case 1:
            {
                selectedTrait = { TRAIT_HEAVY_ARMOUR }; break;
            }
        case 2:
            {
                selectedTrait = { TRAIT_QUICK_HANDS }; break;
            }
        case 3:
            {
                selectedTrait = { TRAIT_FORTUNATE }; break;
            }
        case 4:
            {
                selectedTrait = { TRAIT_SCHOLARLY }; break;
            }
        case 5:
            {
                selectedTrait = {TRAIT_BEAST_MASTER}; break;
            }
        }
        
        isConfirmed = UI::UI_ConfirmAction("Confirm this trait? (Y/N)");
        if (isConfirmed == false)
        {
            UI::UI_DisplayInfoMessage("No Worries. Pick again.");
            UI::UI_TimedPause(700);
        }
    }
    
    PlayerApplyTrait(player, selectedTrait);
    char message[256];
    sprintf_s(message, sizeof(message), "Trait applied: %s", PlayerGetTraitName(selectedTrait)); //NOLINT
    UI::UI_DisplaySuccessMessage(message);
    UI::UI_TimedPause(700);
}

const char* PlayerGetTraitName(PlayerTrait trait)
{
    switch (trait)
    {
        case TRAIT_HEAVY_ARMOUR:
            {
                return "Heavy Armour Expert";
                
            }
    case TRAIT_QUICK_HANDS:
            {
                return "Quick Hands";
            }
    case TRAIT_FORTUNATE:
            {
                return "Fortunate";
            }
    case TRAIT_SCHOLARLY:
            {
                return "Scholarly";
            }
    case TRAIT_BEAST_MASTER:
            {
                return "Beast Master";
            }
    default:  // NOLINT(clang-diagnostic-covered-switch-default)
            {
                return "Unknown";
            }
    }
}

/*I just have 5 traits I just noticed that I mentioned 2 extra lol. anyways just letting you know for your future ref.*/