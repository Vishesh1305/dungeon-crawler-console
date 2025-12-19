#include "Game.h"
#include "../UI/UI.h"
#include <cstdlib>
#include <cstring>
//--------------------
// GAME FUNCTIONS
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
    

    GameInitializeEnemies(game);
    GameInitializeAbilities(game);
    
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
                UI::UI_DisplayInfoMessage("Generating Dungeon...");
                UI::UI_DisplayLoadingBar();
                game->dungeon = DungeonInit();
                DungeonGenerateRooms(game->dungeon);
                DungeonGenerateConnections(game->dungeon);
                
                game->questLog = QuestInit();
                game->inventory = InventoryCreate();
                
                QuestGenerate(game->questLog, game->player->level);
                
                game->dungeon->rooms[0].explored = true;
                game->player->currentRoom = 0;
                game->currentState = GAME_LOOP;
                break;
            }
        case LOAD_GAME:
            {
                UI::UI_DisplayInfoMessage("Loading Saved Data..");
                UI::UI_DisplayLoadingBar();
                if (game->player != nullptr) { free(game->player); game->player = nullptr; }
                if (game->inventory != nullptr) { InventoryFree(game->inventory); game->inventory = nullptr; }
                if (game->questLog != nullptr) { free(game->questLog); game->questLog = nullptr; }
                if (game->stats != nullptr) { free(game->stats); game->stats = nullptr; }
                if (game->dungeon != nullptr) { free(game->dungeon); game->dungeon = nullptr; }

                Dungeon* d = nullptr;


                if (FileLoadGame(&game->player, &game->inventory, &game->questLog, (GameStatistics**)&game->stats, &d))
                {
                    if (d != nullptr)
                    {
                        DungeonGenerateConnections(d);
                        // ADD THIS: Regenerate room descriptions
                        const char* roomDescriptions[20] = {
                            "A dark corridor with stone walls",
                            "A musty chamber filled with cobwebs",
                            "A huge hall with ancient pillars",
                            "A narrow passage with dripping water",
                            "A circular room with mysterious runes",
                            "A dusty library with old tombs",
                            "An armoury with rusty weapons",
                            "A torture chamber with old equipment",
                            "A throne room in ruins",
                            "A chapel with broken statues",
                            "A treasury vault which is empty",
                            "A kitchen with rotting food",
                            "A bedroom with tattered curtains",
                            "A study with scattered papers",
                            "A laboratory with strange equipments",
                            "A prison with empty cells and some skeleton ruins",
                            "A garden overgrown with weeds",
                            "A fountain room with stagnant water",
                            "A war room filled with faded maps",
                            "A crypt with ancient tombs"
                        };
        
                        for (short i = 0; i < d->totalRooms; i++)
                        {
                            strcpy_s(d->rooms[i].description, 
                                     sizeof(d->rooms[i].description), 
                                     roomDescriptions[i % 20]);
                        }
                    }
                    game->dungeon = d;
                    game->currentState = GAME_LOOP;
                }
                else
                {
                    if (d != nullptr) free(d);
                    UI::UI_DisplayErrorMessage("Failed to load savegame!");
                    UI::UI_TimedPause(1500);
                    game->currentState = MAIN_MENU;
                }
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
                if (game->player == nullptr)
                {
                    game->isRunning = false;
                    break;
                }

                GameStatus status = GameCheckGameStatus(game);

                if (status == GAME_WON)
                    UI::UI_DisplayVictoryScreen(game->player, game->stats);
                else if (status == GAME_LOST)
                    UI::UI_DisplayDefeatScreen(game->player, game->stats);
                else
                    UI::UI_DisplayGameoverScreen(game->player, game->stats);

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
        //PlayerFree(game->player);
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
    
    return GAME_OVER;
    


}

void GameHandleCharacterCreation(GameInstance* game)
{
    CLEAR_SCREEN();
    UI::UI_PrintHeader("CHARACTER CREATION");
    printf("\n");
    
    game->player = PlayerCreate();
    PlayerInitStats(game->player);
    
    printf("Welcome to Dungeon Crawler, brave tarnished\n");
    UI::UI_GetStringInput("Enter Your Name: ", game->player->name, MAX_NAME_LENGTH);
    UI::UI_TimedPause(500);
    PlayerSelectTrait(game->player);
    printf("Follow the instructions below to clear the screen and proceed further");
    UI::UI_PauseScreen();
    
    CLEAR_SCREEN();
    UI::UI_PrintHeader("CHARACTER CREATED");
    printf("Name: %s\n", game->player->name);
    printf("Trait: %s\n", PlayerGetTraitName(game->player->trait));
    UI::UI_PrintSection("Starting Stats");
    printf("\n");
    printf("  Health: %hu\n", game->player->maxHealth);
    printf("  Attack: %hu\n", game->player->attack);
    printf("  Defence: %hu\n", game->player->defense);
    printf("  Gold: %hu\n", game->player->gold);
    
    UI::UI_PauseScreen();
}

void GameHandleGameDifficultySelection(GameInstance* game)
{
    DifficultyLevel selectedDifficulty = {};
    bool isConfirmed = false;
    const char* format;
    while (!isConfirmed)
    {
        CLEAR_SCREEN();
        UI::UI_PrintHeader("SELECT DIFFICULTY");
        printf("\n");
        printf("1) Easy - 50%% more health, Weaker enemies.\n");
        printf("2) Normal - Standard, balanced gameplay.\n");
        printf("3) Hard - 50%% less health, Stronger enemies.\n");
        printf("4) Insane - Permadeath, Stronger enemies, +100%% rewards\n\n");
        
        unsigned short choice = UI::UI_GetMenuInput(1, 4);
        
        switch (choice)
        {
        case 1:
            {
                selectedDifficulty = EASY;
                game->player->maxHealth = (unsigned short)(game->player->maxHealth * 1.5);
                game->player->health = game->player->maxHealth;
                game->player->defense = (unsigned short)(game->player->defense * 1.35);
                format = "EASY";
                break;
            }
        case 2:
            {
                selectedDifficulty = NORMAL; 
                format = "NORMAL";
                break;
            }
        case 3:
            {
                selectedDifficulty = HARD; 
                game->player->maxHealth = (unsigned short)(game->player->maxHealth * 0.75);
                game->player->health = game->player->maxHealth;
                game->player->defense = (unsigned short)(game->player->defense * 0.8);
                format = "HARD";
                break;
            }
        case 4:
            {
                selectedDifficulty = INSANE;
                game->player->maxHealth = (unsigned short)(game->player->maxHealth * 0.5);
                game->player->health = game->player->maxHealth;
                game->player->defense = (unsigned short)(game->player->defense * 0.7);
                format = "INSANE";
                break;
            }
        default:
            {
                selectedDifficulty = NORMAL; 
                format = "NORMAL";
                break;
            }
        }
        
        isConfirmed = UI::UI_ConfirmAction("Confirm this difficulty?");
        if (isConfirmed == false)
        {
            printf("No Worries. Pick again\n");
            UI::UI_TimedPause(750);
        }
        
        game->player->difficulty = selectedDifficulty;
        char message[256];
        sprintf_s(message, sizeof(message), "Diffculty applied: %s", format); //NOLINT
        UI::UI_DisplayInfoMessage(message);
        printf("\n");
        PlayerDisplayStats(game->player);
        printf("\n");
        UI::UI_PauseScreen();
    }
    
}

void GameHandleGameLoop(GameInstance* game)
{
    if (game == nullptr)
    {
        printf("ERROR - GameHandleGameLoop: game is null\n");
        return;
    }
    UI::UI_PrintDivider();
    
    DungeonDisplayRoom(game->player, game->dungeon);
    PlayerDisplayStatusBar(game->player);
    
    unsigned short currentRoom = game->player->currentRoom;
    if (game->dungeon->rooms[currentRoom].hasBoss)
    {
        UI::UI_DisplayWarningMessage("You sense a powerful presence ahead..");
    }
    if (game->dungeon->rooms[currentRoom].hasShop)
    {
        UI::UI_DisplayInfoMessage("You see a merchant here!");
        printf("Press 'S' to visit the shop.\n");
    }
    DungeonDisplayActionMenu();
    char choice = UI::UI_GetCharInput();
    
    switch (choice)
    {
    case '1':
        {
            Direction direction = UI::UI_GetDirectionInput();
            DungeonMoveToRoom(game->player, game->dungeon, direction);
            game->dungeon->rooms[game->player->currentRoom].explored = true;
            GameHandleEncounter(game);
            break;
        }
    case '2':
        {
            CLEAR_SCREEN();
            PlayerDisplayStats(game->player);
            UI::UI_PauseScreen();
            break;
        }
    case '3':
        {
            CLEAR_SCREEN();
            if (game->inventory != nullptr && game->inventory->itemCount > 0)
            {
                InventoryDisplay(game->inventory);
                printf("\nPress 'U' to use an item or any other key to return\n");
                char invChoice = UI::UI_GetCharInput();
                
                if (invChoice == 'U' || invChoice == 'u')
                {
                    printf("Enter item ID to use: ");
                    short itemID;
                    scanf_s("%hd", &itemID);
                    while (getchar() != '\n') {}
                    InventoryUseItem(game->inventory, game->player, itemID);
                }
            }
            else
            {
                UI::UI_DisplayWarningMessage("Inventory is empty!");
            }
            UI::UI_PauseScreen();
            break;
        }
    case '4':
        {
            CLEAR_SCREEN();
            if (game->questLog != nullptr)
            {
                QuestDisplay(game->questLog, game->player);
            }
            else
            {
                printf("No quest log available!\n");
            }
            UI::UI_PauseScreen();
            break;
        }
    case '5':
        {
            CLEAR_SCREEN();
            AbilityDisplayList(game->player);
            UI::UI_PauseScreen();
            break;
        }
    case '6':
        {
            CLEAR_SCREEN();
            DungeonDisplayMap(game->player, game->dungeon);
            UI::UI_PauseScreen();
            break;
        }
    case '7':
        {
            CLEAR_SCREEN();
            if (FileSaveGame(game->player, game->inventory, game->questLog, game->stats, game->dungeon))
            {
                UI::UI_DisplaySuccessMessage("Game saved successfully!");
            }
            else
            {
                UI::UI_DisplayErrorMessage("Failed to save game!");
            }
            UI::UI_PauseScreen();
            break;
        }
    case '8':
        {
            game->currentState = PAUSE_MENU;
            break;
        }
    case 's':
    case 'S':
        {
            if (game->dungeon->rooms[currentRoom].hasShop)
            {
                if (game->shop == nullptr)
                {
                    game->shop = ShopInit();
                }
                UI::UI_DisplayInfoMessage("The merchant rummages through goods...");
                UI::UI_DisplayLoadingBar();
                ShopGenerateItems(game->shop, game->player->level);
                ShopMenu(game->shop, game->player, game->inventory);
            }
            else
            {
                UI::UI_DisplayWarningMessage("There's no shop here, brave adventurer!");
                UI::UI_TimedPause(1200);
            }
            break;
        }
    default:
        {
            UI::UI_DisplayErrorMessage("Invalid Choice!");
            UI::UI_TimedPause(500);
            break;
        }
    }
    
    if (game->player->exp >= (game->player->level * XP_PER_LEVEL))
    {
        PlayerLevelup(game->player);
        AbilityCheckUnlocks(game->player, game);
    }
    
    if (game->questLog != nullptr)
    {
        QuestCheckCompletion(game->questLog, game->player, game);
    }
    
    PlayerUpdateStatusEffects(game->player);
    
    GameStatus status = GameCheckGameStatus(game);
    if (status != GAME_CONTINUE)
    {
        game->currentState = GAME_OVER;
    }
}

void GameHandlePauseMenu(GameInstance* game)
{
        if (game == nullptr)
    {
        printf("ERROR - GameHandlePauseMenu: game is null\n");
        return;
    }
    
    bool inPauseMenu = true;
    
    while (inPauseMenu)
    {
        CLEAR_SCREEN();
        UI::UI_PrintHeader("GAME PAUSED");
        printf("\n");
        
        printf("  1. Resume Game\n");
        printf("  2. View Stats\n");
        printf("  3. View Inventory\n");
        printf("  4. View Quests\n");
        printf("  5. View Abilities\n");
        printf("  6. View Map\n");
        printf("  7. Save Game\n");
        printf("  8. Game Settings\n");
        printf("  9. Return to Main Menu\n");
        printf("  0. Quit Game\n");
        printf("\n");
        UI::UI_PrintDivider();
        
        unsigned short choice = UI::UI_GetMenuInput(0, 9);
        
        switch (choice)
        {
        case 1: // Resume Game
            {
                inPauseMenu = false;
                game->currentState = GAME_LOOP;
                break;
            }
        case 2: // View Stats
            {
                CLEAR_SCREEN();
                PlayerDisplayStats(game->player);
                UI::UI_PauseScreen();
                break;
            }
        case 3: // View Inventory
            {
                CLEAR_SCREEN();
                if (game->inventory != nullptr && game->inventory->itemCount > 0)
                {
                    InventoryDisplay(game->inventory);
                    printf("\nPress 'U' to use an item or any other key to return\n");
                    char invChoice = UI::UI_GetCharInput();
                    
                    if (invChoice == 'U' || invChoice == 'u')
                    {
                        printf("Enter item ID to use: ");
                        short itemID;
                        scanf_s("%hd", &itemID);
                        while (getchar() != '\n') {}
                        InventoryUseItem(game->inventory, game->player, itemID);
                    }
                }
                else
                {
                    UI::UI_DisplayWarningMessage("Inventory is empty!");
                }
                UI::UI_PauseScreen();
                break;
            }
        case 4: // View Quests
            {
                CLEAR_SCREEN();
                if (game->questLog != nullptr)
                {
                    QuestDisplay(game->questLog, game->player);
                }
                else
                {
                    printf("No quest log available!\n");
                }
                UI::UI_PauseScreen();
                break;
            }
        case 5: // View Abilities
            {
                CLEAR_SCREEN();
                AbilityDisplayList(game->player);
                UI::UI_PauseScreen();
                break;
            }
        case 6: // View Map
            {
                CLEAR_SCREEN();
                DungeonDisplayMap(game->player, game->dungeon);
                UI::UI_PauseScreen();
                break;
            }
        case 7: // Save Game
            {
                CLEAR_SCREEN();
                if (FileSaveGame(game->player, game->inventory, game->questLog, game->stats, game->dungeon))
                {
                    UI::UI_DisplaySuccessMessage("Game saved successfully!");
                }
                else
                {
                    UI::UI_DisplayErrorMessage("Failed to save game!");
                }
                UI::UI_PauseScreen();
                break;
            }
        case 8: // Game Settings
            {
                CLEAR_SCREEN();
                UI::UI_PrintHeader("GAME SETTINGS");
                printf("\n");
                printf("Current Difficulty: ");
                switch (game->player->difficulty)
                {
                case EASY:
                    printf("%sEASY%s\n", GREEN, RESET);
                    break;
                case NORMAL:
                    printf("%sNORMAL%s\n", YELLOW, RESET);
                    break;
                case HARD:
                    printf("%sHARD%s\n", RED, RESET);
                    break;
                case INSANE:
                    printf("%sINSANE%s\n", MAGENTA, RESET);
                    break;
                }
                printf("\n");
                UI::UI_PrintSection("GAME STATISTICS");
                printf("Enemies Defeated: %hu\n", game->stats->totalEnemiesDefeated);
                printf("Gold Earned: %hu\n", game->stats->totalGoldEarned);
                printf("Damage Dealt: %hu\n", game->stats->totalDamageDealt);
                printf("Damage Taken: %hu\n", game->stats->totalDamageTaken);
                printf("Items Collected: %hu\n", game->stats->itemsCollected);
                printf("Quests Completed: %hu\n", game->stats->questsCompleted);
                printf("Deaths: %hu\n", game->stats->deathCount);
                printf("\n");
                UI::UI_PauseScreen();
                break;
            }
        case 9: // Return to Main Menu
            {
                if (UI::UI_ConfirmAction("Return to main menu? Unsaved progress will be lost"))
                {
                    inPauseMenu = false;
                    
                    // Clean up current game state
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
                        InventoryFree(game->inventory);
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
                    
                    game->currentState = MAIN_MENU;
                }
                break;
            }
        case 0: // Quit Game
            {
                if (UI::UI_ConfirmAction("Are you sure you want to quit? Unsaved progress will be lost"))
                {
                    inPauseMenu = false;
                    game->isRunning = false;
                    game->currentState = GAME_OVER;
                }
                break;
            }
        default:
            {
                UI::UI_DisplayErrorMessage("Invalid choice!");
                UI::UI_TimedPause(500);
                break;
            }
        }
    }
}

GameStatus GameCheckGameStatus(GameInstance* game)
{
    if (game == nullptr)
    {
        printf("ERROR - GameCheckGameStatus: game is null\n");
        return GAME_CONTINUE;
    }
    if (game->player->level >= MAX_LEVEL && game->player->currentRoom == 19)
    {
        return GAME_WON;
    }
    if (game->player->health <= 0)
    {
        return GAME_LOST;
    }
    return GAME_CONTINUE;
}

/*void GameHandleEncounter(GameInstance* game)
{
    if (game == nullptr) return;
    unsigned short currentRoom = game->player->currentRoom;
    EncounterType en = game->dungeon->rooms[currentRoom].encounterType;
    
    switch (en)
    {
    case EMPTY:
        {
            UI::UI_DisplayInfoMessage("This room is empty and quite.");
            UI::UI_TimedPause(1000);
            break;
        }
    case ENEMY:
        {
            Enemy* enemy = EnemyGenerateForLevel(game, game->player->level);
            if (enemy != nullptr)
            {
                CLEAR_SCREEN();
                UI::UI_DisplayWarningMessage("An enemy appears!");
                EnemyDisplayStats(enemy);
                UI::UI_TimedPause(1000);
                
                //start Combat
                CombatResult result = CombatStart(game->player, enemy, game);
                if (result == COMBAT_VICTORY)
                {
                    CombatAwardVictory(game->player, enemy, game);
                    game->dungeon->rooms[currentRoom].encounterType = EMPTY;
                }
                else if (result == COMBAT_DEFEAT)
                {
                    game->currentState = GAME_OVER;
                }
                else if (result == COMBAT_ESCAPE)
                {
                    UI::UI_DisplayWarningMessage("You managed to escape!");
                }
                
                free(enemy);
            }
            break;
        }
    case TREASURE:
        {
            CLEAR_SCREEN();
            UI::UI_DisplaySuccessMessage("You found a treasure chest!");
            ItemData treasure = ItemGenerateTreasure(game->player->level);
            printf("You found: %s\n", treasure.name);
            ItemDisplay(&treasure);
            
            // Inventory Management goes here
            game->dungeon->rooms[currentRoom].encounterType = EMPTY;
            break;
        }
    case QUEST:
        {
            CLEAR_SCREEN();
            printf("Quest related stuff goes here.\n");
            break;
        }
    case BOSS:
        {
            CLEAR_SCREEN();
            UI::UI_DisplayWarningMessage("=== BOSS FIGHT ===");
            printf("A powerful boss blocks your path\n");
            
            Enemy* boss = EnemyGenerateForLevel(game, game->player->level + 3);
            if (boss == nullptr)
            {
                printf("Failed to allocate boss. Returning. -> GameHandleEncounter()\n");
                return;
            }
            boss->health = (short)(boss->health * 2);
            boss->baseHealth = boss->health;
            boss->attack = (short)(boss->attack * 1.5);
            boss->expReward = (short)(boss->expReward * 3);
            boss->goldReward = (short)(boss->goldReward * 3);
            
            EnemyDisplayStats(boss);
            UI::UI_TimedPause(1500);
            
            CombatResult result = CombatStart(game->player, boss, game);
            if (result == COMBAT_VICTORY)
            {
                CombatAwardVictory(game->player, boss, game);
                UI::UI_DisplaySuccessMessage("You defeated the boss!");
                game->dungeon->rooms[currentRoom].encounterType = EMPTY;
                game->dungeon->rooms[currentRoom].hasBoss = false;
            }
            else if (result == COMBAT_DEFEAT)
            {
                game->currentState = GAME_OVER;
            }
            else if (result == COMBAT_ESCAPE)
            {
                UI::UI_DisplayWarningMessage("You barely managed to escape the boss...");
            }
            free(boss);
            UI::UI_PauseScreen();
            break;
        }
    }
    game->dungeon->rooms[currentRoom].explored = true;
}*/
void GameHandleEncounter(GameInstance* game)
{
    if (game == nullptr)
    {
        printf("ERROR - GameHandleEncounter: game is null\n");
        return;
    }
    
    unsigned short currentRoom = game->player->currentRoom;
    Room* room = &game->dungeon->rooms[currentRoom];
    EncounterType encounter = room->encounterType;
    
    room->explored = true;
    
    switch (encounter)
    {
    case EMPTY:
        {
            UI::UI_DisplayInfoMessage("This room is empty and quiet.");
            UI::UI_TimedPause(1000);
            break;
        }
    case ENEMY:
        {
            Enemy* enemy = EnemyGenerateForLevel(game, game->player->level);
            if (enemy != nullptr)
            {
                CLEAR_SCREEN();
                UI::UI_DisplayWarningMessage("An enemy appears!");
                printf("\n");
                EnemyDisplayStats(enemy);
                UI::UI_TimedPause(1500);
                
                // Start Combat
                CombatResult result = CombatStart(game->player, enemy, game);
                
                switch (result)
                {
                case COMBAT_VICTORY:
                    {
                        CombatAwardVictory(game->player, enemy, game);
                        room->encounterType = EMPTY;
                        
                        // Update quest progress for killing enemies
                        if (game->questLog != nullptr)
                        {
                            QuestUpdateProgress(game->questLog, KILL_ENEMIES, 1);
                        }
                        break;
                    }
                case COMBAT_DEFEAT:
                    {
                        game->stats->deathCount++;
                        
                        // Check if playing on Insane difficulty (permadeath)
                        if (game->player->difficulty == INSANE)
                        {
                            UI::UI_DisplayErrorMessage("PERMADEATH - Your journey ends here!");
                            game->currentState = GAME_OVER;
                        }
                        else
                        {
                            // Respawn with penalty
                            UI::UI_DisplayWarningMessage("You were defeated but managed to escape...");
                            game->player->health = game->player->maxHealth / 2;
                            game->player->gold = (int)(game->player->gold * 0.75f);
                            game->player->currentRoom = 0; // Return to starting room
                            UI::UI_DisplayInfoMessage("You lost 25% of your gold and returned to the entrance.");
                        }
                        UI::UI_TimedPause(2000);
                        break;
                    }
                case COMBAT_ESCAPE:
                    {
                        UI::UI_DisplayWarningMessage("You managed to escape!");
                        UI::UI_TimedPause(1000);
                        break;
                    }
                }
                
                free(enemy);
            }
            else
            {
                UI::UI_DisplayErrorMessage("Failed to generate enemy!");
                room->encounterType = EMPTY;
            }
            break;
        }
    case TREASURE:
        {
            CLEAR_SCREEN();
            UI::UI_PrintHeader("TREASURE FOUND!");
            printf("\n");
            UI::UI_DisplaySuccessMessage("You found a treasure chest!");
            printf("\n");
            
            // Generate treasure item
            ItemData treasure = ItemGenerateTreasure(game->player->level);
            
            printf("Inside you find: ");
            switch (treasure.rarity)
            {
            case COMMON:
                printf("%s", RESET);
                break;
            case UNCOMMON:
                printf("%s", GREEN);
                break;
            case RARE:
                printf("%s", BLUE);
                break;
            case LEGENDARY:
                printf("%s", MAGENTA);
                break;
            }
            printf("%s%s\n\n", treasure.name, RESET);
            
            ItemDisplay(&treasure);
            printf("\n");
            
            // Try to add to inventory
            if (game->inventory != nullptr)
            {
                if (!InventoryIsFull(game->inventory))
                {
                    if (InventoryAddItem(game->inventory, treasure))
                    {
                        UI::UI_DisplaySuccessMessage("Item added to inventory!");
                        game->stats->itemsCollected++;
                        
                        // Update quest progress for collecting items
                        if (game->questLog != nullptr)
                        {
                            QuestUpdateProgress(game->questLog, COLLECT_ITEMS, 1);
                        }
                    }
                    else
                    {
                        UI::UI_DisplayErrorMessage("Failed to add item to inventory!");
                    }
                }
                else
                {
                    UI::UI_DisplayWarningMessage("Inventory is full!");
                    printf("\nWould you like to:\n");
                    printf("1. Leave the item behind\n");
                    printf("2. Drop an item to make room\n");
                    
                    unsigned short choice = UI::UI_GetMenuInput(1, 2);
                    
                    if (choice == 2)
                    {
                        CLEAR_SCREEN();
                        InventoryDisplay(game->inventory);
                        printf("\nEnter item ID to drop (0 to cancel): ");
                        short dropID;
                        scanf_s("%hd", &dropID);
                        while (getchar() != '\n') {}
                        
                        if (dropID != 0)
                        {
                            if (InventoryRemoveItem(game->inventory, dropID))
                            {
                                if (InventoryAddItem(game->inventory, treasure))
                                {
                                    UI::UI_DisplaySuccessMessage("Swapped items successfully!");
                                    game->stats->itemsCollected++;
                                    
                                    if (game->questLog != nullptr)
                                    {
                                        QuestUpdateProgress(game->questLog, COLLECT_ITEMS, 1);
                                    }
                                }
                            }
                            else
                            {
                                UI::UI_DisplayErrorMessage("Item not found!");
                            }
                        }
                        else
                        {
                            UI::UI_DisplayInfoMessage("Item left behind.");
                        }
                    }
                    else
                    {
                        UI::UI_DisplayInfoMessage("Item left behind.");
                    }
                }
            }
            
            // Also award some gold
            unsigned short bonusGold = RandomShort(10, 50) * (game->player->level);
            bonusGold = (unsigned short)(bonusGold * game->player->goldMultiplier);
            game->player->gold += bonusGold;
            game->stats->totalGoldEarned += bonusGold;
            
            printf("\n%sYou also found %hu gold!%s\n", YELLOW, bonusGold, RESET);
            
            room->encounterType = EMPTY;
            UI::UI_PauseScreen();
            break;
        }
    case QUEST:
        {
            CLEAR_SCREEN();
            UI::UI_PrintHeader("QUEST ENCOUNTER");
            printf("\n");
            
            // Check if we can add a new quest
            if (game->questLog != nullptr && game->questLog->questCount < MAX_QUESTS)
            {
                UI::UI_DisplayInfoMessage("You found a mysterious notice board!");
                printf("\n");
                printf("A weathered parchment catches your eye...\n\n");
                
                if (UI::UI_ConfirmAction("Accept a new quest"))
                {
                    short oldCount = game->questLog->questCount;
                    QuestGenerate(game->questLog, game->player->level);
                    
                    if (game->questLog->questCount > oldCount)
                    {
                        QuestData* newQuest = &game->questLog->quests[game->questLog->questCount - 1];
                        
                        printf("\n");
                        UI::UI_DisplaySuccessMessage("New Quest Accepted!");
                        printf("\n");
                        printf("%sQuest: %s%s\n", CYAN, newQuest->title, RESET);
                        printf("%s\n", newQuest->description);
                        printf("Reward: %s%hd gold%s\n", YELLOW, newQuest->rewardGold, RESET);
                    }
                }
                else
                {
                    UI::UI_DisplayInfoMessage("You decided to pass on this quest for now.");
                }
            }
            else if (game->questLog != nullptr && game->questLog->questCount >= MAX_QUESTS)
            {
                UI::UI_DisplayWarningMessage("Your quest log is full!");
                printf("Complete some quests before accepting new ones.\n");
            }
            else
            {
                UI::UI_DisplayInfoMessage("Nothing of interest here...");
            }
            
            room->encounterType = EMPTY;
            UI::UI_PauseScreen();
            break;
        }
    case BOSS:
        {
            CLEAR_SCREEN();
            printf("\n");
            printf("%s", RED);
            UI::UI_PrintHeader("!!! BOSS FIGHT !!!");
            printf("%s", RESET);
            printf("\n");
            
            UI::UI_DisplayWarningMessage("A powerful boss blocks your path!");
            printf("\nThe ground trembles beneath your feet...\n\n");
            UI::UI_TimedPause(1500);
            
            // Generate boss enemy (higher level than player)
            Enemy* boss = EnemyGenerateForLevel(game, game->player->level + 3);
            
            if (boss == nullptr)
            {
                printf("ERROR - Failed to generate boss enemy.\n");
                room->encounterType = EMPTY;
                room->hasBoss = false;
                return;
            }
            
            // Enhance boss stats
            boss->baseHealth = (short)(boss->baseHealth * 2.5f);
            boss->health = boss->baseHealth;
            boss->attack = (short)(boss->attack * 1.75f);
            boss->defense = (short)(boss->defense * 1.5f);
            boss->expReward = (short)(boss->expReward * 4);
            boss->goldReward = (short)(boss->goldReward * 5);
            boss->lootRarity = LEGENDARY;
            
            // Give the boss a special name
            char bossName[MAX_NAME_LENGTH];
            sprintf_s(bossName, sizeof(bossName), "Elite %s", boss->name);
            strcpy_s(boss->name, sizeof(boss->name), bossName);
            
            printf("%s=== %s ===%s\n\n", MAGENTA, boss->name, RESET);
            EnemyDisplayStats(boss);
            
            printf("\n");
            UI::UI_DisplayWarningMessage("Prepare yourself for battle!");
            UI::UI_TimedPause(2000);
            
            // Boss fight
            UI::UI_DisplayWarningMessage("A powerful presence approaches...");
            UI::UI_DisplayLoadingBar();
            CombatResult result = CombatStart(game->player, boss, game);
            
            switch (result)
            {
            case COMBAT_VICTORY:
                {
                    CLEAR_SCREEN();
                    printf("\n");
                    printf("%s", YELLOW);
                    UI::UI_PrintHeader("BOSS DEFEATED!");
                    printf("%s", RESET);
                    printf("\n");
                    
                    CombatAwardVictory(game->player, boss, game);
                    
                    // Bonus rewards for boss kill
                    unsigned short bonusExp = (unsigned short)(boss->expReward * 0.5f);
                    unsigned short bonusGold = (unsigned short)(boss->goldReward * 0.5f);
                    
                    printf("\n%s=== BOSS BONUS REWARDS ===%s\n", MAGENTA, RESET);
                    PlayerGainExperience(game->player, bonusExp);
                    printf("\n");
                    PlayerGainGold(game->player, bonusGold);
                    
                    // Guaranteed legendary item drop from boss
                    printf("\n");
                    UI::UI_DisplaySuccessMessage("The boss dropped a legendary item!");
                    ItemData bossLoot = ItemGenerateRandom(LEGENDARY, (ItemType)RandomShort(0, 2));
                    ItemDisplay(&bossLoot);
                    
                    if (game->inventory != nullptr && !InventoryIsFull(game->inventory))
                    {
                        InventoryAddItem(game->inventory, bossLoot);
                        UI::UI_DisplaySuccessMessage("Item added to inventory!");
                        game->stats->itemsCollected++;
                    }
                    else
                    {
                        UI::UI_DisplayWarningMessage("Inventory full! Legendary item lost!");
                    }
                    
                    room->encounterType = EMPTY;
                    room->hasBoss = false;
                    
                    // Check for game victory condition
                    if (currentRoom == MAX_ROOMS - 1)
                    {
                        printf("\n");
                        UI::UI_DisplaySuccessMessage("You have conquered the dungeon!");
                        game->currentState = GAME_OVER;
                    }
                    
                    UI::UI_PauseScreen();
                    break;
                }
            case COMBAT_DEFEAT:
                {
                    game->stats->deathCount++;
                    
                    if (game->player->difficulty == INSANE)
                    {
                        UI::UI_DisplayErrorMessage("PERMADEATH - The boss has ended your journey!");
                        game->currentState = GAME_OVER;
                    }
                    else
                    {
                        UI::UI_DisplayWarningMessage("The boss defeated you...");
                        game->player->health = game->player->maxHealth / 3;
                        game->player->gold = (int)(game->player->gold * 0.5f);
                        game->player->currentRoom = 0;
                        UI::UI_DisplayInfoMessage("You lost 50% of your gold and barely escaped with your life.");
                    }
                    UI::UI_TimedPause(2500);
                    break;
                }
            case COMBAT_ESCAPE:
                {
                    UI::UI_DisplayWarningMessage("You barely managed to escape the boss!");
                    UI::UI_DisplayInfoMessage("The boss awaits your return...");
                    UI::UI_TimedPause(1500);
                    break;
                }
            }
            
            free(boss);
            break;
        }
    default:
        {
            UI::UI_DisplayInfoMessage("Nothing happens...");
            UI::UI_TimedPause(500);
            break;
        }
    }
}

void GameInitializeEnemies(GameInstance* game)
{
    game->enemyCount = 5;
    
    //-----------------
    // Enemy 0: Goblin 
    //-----------------
    game->enemyList[0].enemyID = 0;
    errno_t err = strcpy_s(game->enemyList[0].name, sizeof(game->enemyList[0].name), "Goblin");
    if (err != 0)
    {
        UI::UI_DisplayErrorMessage("Failed to copy enemy name[0] -> Goblin");
        printf("\n");
    }
    game->enemyList[0].baseHealth = 30;
    game->enemyList[0].attack = 8;
    game->enemyList[0].defense = 3;
    game->enemyList[0].expReward = 20;
    game->enemyList[0].goldReward = 10;
    game->enemyList[0].difficulty = 1;
    game->enemyList[0].lootRarity = COMMON;
    game->enemyList[0].statusEffectCount = 0;
    
    //-----------------
    // Enemy 1: Skelly
    //-----------------
    game->enemyList[1].enemyID = 1;
    err = strcpy_s(game->enemyList[1].name, sizeof(game->enemyList[1].name), "Skelly");
    if (err != 0)
    {
        UI::UI_DisplayErrorMessage("Failed to copy enemy name[1] -> Skelly");
        printf("\n");
    }
    game->enemyList[1].baseHealth = 40;
    game->enemyList[1].attack = 12;
    game->enemyList[1].defense = 5;
    game->enemyList[1].expReward = 30;
    game->enemyList[1].goldReward = 15;
    game->enemyList[1].difficulty = 2;
    game->enemyList[1].lootRarity = COMMON;
    game->enemyList[1].statusEffectCount = 0;
    
    //-----------------
    // Enemy 2: Omen
    //-----------------
    game->enemyList[2].enemyID = 2;
    err = strcpy_s(game->enemyList[2].name, sizeof(game->enemyList[2].name), "Omen");
    if (err != 0)
    {
        UI::UI_DisplayErrorMessage("Failed to copy enemy name[2] -> Omen");
        printf("\n");
    }
    game->enemyList[2].baseHealth = 60;
    game->enemyList[2].attack = 15;
    game->enemyList[2].defense = 8;
    game->enemyList[2].expReward = 50;
    game->enemyList[2].goldReward = 25;
    game->enemyList[2].difficulty = 3;
    game->enemyList[2].lootRarity = UNCOMMON;
    game->enemyList[2].statusEffectCount = 0;
    
    //-----------------
    // Enemy 3: Banished Knight
    //-----------------
    game->enemyList[3].enemyID = 3;
    err = strcpy_s(game->enemyList[3].name, sizeof(game->enemyList[3].name), "Banished Knight");
    if (err != 0)
    {
        UI::UI_DisplayErrorMessage("Failed to copy enemy name[3] -> Banished Knight");
        printf("\n");
    }
    game->enemyList[3].baseHealth = 80;
    game->enemyList[3].attack = 20;
    game->enemyList[3].defense = 12;
    game->enemyList[3].expReward = 80;
    game->enemyList[3].goldReward = 40;
    game->enemyList[3].difficulty = 4;
    game->enemyList[3].lootRarity = RARE;
    game->enemyList[3].statusEffectCount = 0;
    
    //-----------------
    // Enemy 4: Elden Beast
    //-----------------
    game->enemyList[4].enemyID = 4;
    err = strcpy_s(game->enemyList[4].name, sizeof(game->enemyList[4].name), "Elden Beast");
    if (err != 0)
    {
        UI::UI_DisplayErrorMessage("Failed to copy enemy name[4] -> Elden Beast");
        printf("\n");
    }
    game->enemyList[4].baseHealth = 120;
    game->enemyList[4].attack = 30;
    game->enemyList[4].defense = 18;
    game->enemyList[4].expReward = 150;
    game->enemyList[4].goldReward = 100;
    game->enemyList[4].difficulty = 5;
    game->enemyList[4].lootRarity = LEGENDARY;
    game->enemyList[4].statusEffectCount = 0;
}

void GameInitializeAbilities(GameInstance* game)
{
    if (game == nullptr)
    {
        printf("ERROR - GameInitializeAbilities: game is null\n");
        return;
    }
    
    game->abilityCount = 5;
    
    // ==================================
    // ABILITY 1: Power Strike (Level 2)
    // ==================================
    game->abilityList[0].abilityId = 1;
    strcpy_s(game->abilityList[0].name, sizeof(game->abilityList[0].name), "Power Strike");
    strcpy_s(game->abilityList[0].description, sizeof(game->abilityList[0].description), 
             "A powerful attack dealing 1.5x damage");
    game->abilityList[0].unlockedAtLevel = 2;
    game->abilityList[0].damageMultiplier = 1.5f;
    game->abilityList[0].cooldown = 2;
    game->abilityList[0].cooldownRemaining = 0;
    
    // ==================================
    // ABILITY 2: Double Slash (Level 4)
    // ==================================
    game->abilityList[1].abilityId = 2;
    strcpy_s(game->abilityList[1].name, sizeof(game->abilityList[1].name), "Double Slash");
    strcpy_s(game->abilityList[1].description, sizeof(game->abilityList[1].description), 
             "Strike twice dealing normal damage each hit");
    game->abilityList[1].unlockedAtLevel = 4;
    game->abilityList[1].damageMultiplier = 1.0f;
    game->abilityList[1].cooldown = 3;
    game->abilityList[1].cooldownRemaining = 0;
    
    // ===============================
    // ABILITY 3: Life Drain (Level 5)
    // ===============================
    game->abilityList[2].abilityId = 3;
    strcpy_s(game->abilityList[2].name, sizeof(game->abilityList[2].name), "Life Drain");
    strcpy_s(game->abilityList[2].description, sizeof(game->abilityList[2].description), 
             "Attack that heals you for 50% of damage dealt");
    game->abilityList[2].unlockedAtLevel = 5;
    game->abilityList[2].damageMultiplier = 1.2f;
    game->abilityList[2].cooldown = 4;
    game->abilityList[2].cooldownRemaining = 0;
    
    // ==============================
    // ABILITY 4: Whirlwind (Level 7)
    // ==============================
    game->abilityList[3].abilityId = 4;
    strcpy_s(game->abilityList[3].name, sizeof(game->abilityList[3].name), "Whirlwind");
    strcpy_s(game->abilityList[3].description, sizeof(game->abilityList[3].description), 
             "Spinning attack dealing 2x damage");
    game->abilityList[3].unlockedAtLevel = 7;
    game->abilityList[3].damageMultiplier = 2.0f;
    game->abilityList[3].cooldown = 5;
    game->abilityList[3].cooldownRemaining = 0;
    
    // ========================================
    // ABILITY 5: Devastating Blow (Level 9)
    // ========================================
    game->abilityList[4].abilityId = 5;
    strcpy_s(game->abilityList[4].name, sizeof(game->abilityList[4].name), "Devastating Blow");
    strcpy_s(game->abilityList[4].description, sizeof(game->abilityList[4].description), 
             "Ultimate attack dealing 3x damage");
    game->abilityList[4].unlockedAtLevel = 9;
    game->abilityList[4].damageMultiplier = 3.0f;
    game->abilityList[4].cooldown = 6;
    game->abilityList[4].cooldownRemaining = 0;
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

void PlayerFree(Player* player)
{
    if (player == nullptr) return;
    free(player);
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
    
    printf("Name: %s | [LVL: %hu]\n", player->name, player->level);
    UI::UI_DisplayHealthBar(player->health, player->maxHealth);
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
        return;
    }
    switch (trait)
    {
    case TRAIT_HEAVY_ARMOUR:
        player->defense = (unsigned short)(player->defense * 1.5);
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
        
        isConfirmed = UI::UI_ConfirmAction("Confirm this trait?");
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

void PlayerApplyStatusEffects(Player* player, StatusEffect effect)
{
    if (player == nullptr)
    {
        return;
    }
    
    if (player->statusEffectCount >= 10)
    {
        printf("Cannot apply more status effects (max 10)\n");
        return;
    }
    
    switch (effect.type)
    {
    case FORTIFIED:
        player->defense += effect.statModifier;
        printf("%s", GREEN);
        printf("Defense increased by %hd! (Duration: %d turns)\n", 
               effect.statModifier, effect.duration);
        printf("%s", RESET);
        break;
        
    case WEAKENED:
        player->attack -= effect.statModifier;
        printf("%s", RED);
        printf("Attack decreased by %hd! (Duration: %d turns)\n", 
               effect.statModifier, effect.duration);
        printf("%s", RESET);
        break;
        
    case POISON:
        printf("%s", MAGENTA);
        printf("You've been poisoned! (%d damage per turn for %d turns)\n",
               effect.damagePerTurn, effect.duration);
        printf("%s", RESET);
        break;
        
    case BLEED:
        printf("%s", RED);
        printf("You're bleeding! (%d damage per turn for %d turns)\n",
               effect.damagePerTurn, effect.duration);
        printf("%s", RESET);
        break;
        
    case STUN:
        printf("%s", YELLOW);
        printf("You've been stunned! (Cannot act for %d turns)\n", effect.duration);
        printf("%s", RESET);
        break;
    }
    
    player->statusEffect[player->statusEffectCount] = effect;
    player->statusEffectCount++;
}

void PlayerUpdateStatusEffects(Player* player)
{
    if (player == nullptr)
    {
        printf("ERROR - PlayerUpdateStatusEffects: Player is null\n");
        return;
    }
    
    if (player->statusEffectCount == 0)
    {
        return;
    }
    printf("\n");
    UI::UI_PrintColored("Status Effects", CYAN, true);
    printf("%s", RESET);
    
    
    for (unsigned short i = 0; i < player->statusEffectCount; i++)
    {
        StatusEffect* effect = &player->statusEffect[i];
        switch (effect->type)
        {
        case POISON:
            {
                PlayerDamage(player, effect->damagePerTurn);
                printf("%s", MAGENTA);
                printf("💀 Poison deals %d damage! (%d turns left)\n",
                      effect->damagePerTurn, effect->duration);
                printf("%s", RESET);
                break;
            }
        case BLEED:
            {
                PlayerDamage(player, effect->damagePerTurn);
                printf("%s", RED);
                printf("🩸 Bleeding deals %d damage! (%d turns left)\n",
                      effect->damagePerTurn, effect->duration);
                printf("%s", RESET);
                break;
            }
        case STUN:
            {
                printf("%s", YELLOW);
                printf("😵 You are stunned! (%d turns left)\n", effect->duration);
                printf("%s", RESET);
                break;
            }
        case FORTIFIED:
            {
                printf("%s", GREEN);
                printf("🛡️  Fortified! (+%hd defense, %d turns left)\n",
                      effect->statModifier, effect->duration);
                printf("%s", RESET);
                break;
            }
        case WEAKENED:
            {
                printf("%s", RED);
                printf("💔 Weakened! (-%hd attack, %d turns left)\n",
                      effect->statModifier, effect->duration);
                printf("%s", RESET);
                break;
            }
        }
        effect->duration--;
        if (effect->duration <= 0)
        {
            switch (effect->type)
            {
            case FORTIFIED:
                {
                    player->defense -= effect->statModifier;
                    printf("%s", CYAN);
                    printf("⚠️  Fortification wore off! (Defense decreased by %hd)\n",
                          effect->statModifier);
                    printf("%s", RESET);
                    break;
                }
            case WEAKENED:
                {
                    player->attack += effect->statModifier;
                    printf("%s", CYAN);
                    printf("⚠️  Weakness wore off! (Attack restored by %hd)\n",
                          effect->statModifier);
                    printf("%s", RESET);
                    break;
                }
            case STUN:
            case BLEED:
            case POISON:
                {
                    printf("%s", CYAN);
                    printf("⚠️  Status effect wore off!\n");
                    printf("%s", RESET);
                    break;
                }
            }
            for (unsigned short j = i; j <player->statusEffectCount - 1; j++)
            {
                player->statusEffect[j] = player->statusEffect[j + 1];
            }
            player->statusEffectCount--;
            i--;
        }
    }
    printf("\n");
}

void PlayerGainExperience(Player* player, unsigned short exp)
{
    if (player == nullptr) return;
    
    unsigned short adjustedExp = (unsigned short)(exp * player->expMultiplier);
    player->exp += adjustedExp;
    
    printf("%s", GREEN);
    printf("Gained %hu Experience!", adjustedExp);
    printf("%s", RESET);
    printf("[%hu/%hu]", player->exp, XP_PER_LEVEL);
}

void PlayerGainGold(Player* player, unsigned short gold)
{
    if (player == nullptr) return;
    
    unsigned short adjustedGold = (unsigned short)(gold * player->goldMultiplier);
    player->gold += adjustedGold;
    
    printf("%s", YELLOW);
    printf("Gained %hu Gold!", adjustedGold);
    printf("%s", RESET);
    printf("[Total: %hu]\n", player->gold);
}

//--------------------
// ENEMY FUNCTIONS
//--------------------

Enemy* EnemyInit(GameInstance* game,short enemyID)
{
    if (game == nullptr || enemyID < 0 || enemyID >= game->enemyCount)
    {
        printf("ERROR - Invalid enemy ID: %hd\n", enemyID);
        return nullptr;
    }
    
    Enemy* enemy = (Enemy*)malloc(sizeof(Enemy));
    if (enemy == nullptr)
    {
        printf("Failed to allocate memory for enemy - EnemyINIT()\n");
        return nullptr;
    }
    *enemy = game->enemyList[enemyID];
    enemy->health = enemy->baseHealth;
    enemy->statusEffectCount = 0;
    return enemy;
}

Enemy* EnemyGenerateForLevel(GameInstance* game, unsigned short playerLevel)
{
    if (game == nullptr)
    {
        return nullptr;
    }
    short suitableEnemies[MAX_ENEMIES];
    short suitableCount = 0;
    
    for (short i=0; i < game->enemyCount; i++)
    {
        short enemyDiff = game->enemyList[i].difficulty;
        
        if (enemyDiff >= playerLevel - 1 && enemyDiff <= playerLevel + 2)
        {
            suitableEnemies[suitableCount] = i;
            suitableCount++;
        }
    }
    if (suitableCount == 0)
    {
        suitableCount = game->enemyCount;
        for (short i = 0; i < suitableCount; i++)
            suitableEnemies[i] = i;
    }
    short randomIndex = RandomShort(0, (short)suitableCount - 1);
    short selectedIndex = suitableEnemies[randomIndex];
    
    Enemy* enemy = EnemyInit(game, selectedIndex);
    if (enemy == nullptr)
    {
        return nullptr;
    }
    
    float levelDiff = (float)(playerLevel - enemy->difficulty);
    float levelMultiplier = 1.f + (levelDiff * 0.1f);
    
    enemy->baseHealth = (short)(enemy->baseHealth * levelMultiplier);
    enemy->health = enemy->baseHealth;
    enemy->attack = (short)(enemy->attack * levelMultiplier);
    enemy->defense = (short) (enemy->defense * levelMultiplier);
    enemy->expReward = (short) (enemy->expReward * levelMultiplier);
    enemy->goldReward = (short) (enemy->goldReward * levelMultiplier);
    
    return enemy;
}

void EnemyDisplayStats(Enemy* enemy)
{
    UI::UI_PrintDivider();
    if (enemy == nullptr)
    {
        printf("Pointer to enemy is a nullptr - EnemyDisplayStats()\n");
        return;
    }
    UI::UI_PrintSection(enemy->name);
    printf("Health: [%hd/%hd]\n", enemy->health, enemy->baseHealth);
    
    //Combat stats
    printf("Attack: %hd\n", enemy->attack);
    printf("Defense %hd\n", enemy->defense);
    
    //diff
    printf("Difficulty: %hd\n", enemy->difficulty);
    if (enemy->statusEffectCount > 0)
    {
        printf("Status Effects: %hd active\n", enemy->statusEffectCount);
    }
    UI::UI_PrintDivider();
    
}

bool EnemyIsAlive(Enemy* enemy)
{
    if (enemy == nullptr)
    {
        return false;
    }
    
    return enemy->health > 0;
}

void EnemyUpdateStatusEffects(Enemy* enemy)
{
    if (enemy == nullptr) return;
    
    for (short i=0; i < enemy->statusEffectCount; i++)
    {
        StatusEffect* effect = &enemy->statusEffect[i];
        switch (effect->type) //NOLINT
        {
        case POISON:
        case BLEED:
            {
                EnemyDamage(enemy, effect->damagePerTurn);
                char mssg[50];
                sprintf_s(mssg, "%s takes %hu damage from %s!\n", enemy->name, effect->damagePerTurn, (effect->type == POISON ? "Poison" : "Bleed")); //NOLINT
                printf("%s%s%s", CYAN, mssg, RESET);
                break;
            }
        }
        effect->duration--;
        if (effect->duration <= 0)
        {
            for (short j = i; j < enemy->statusEffectCount - 1; j++)
                enemy->statusEffect[j] = enemy->statusEffect[j + 1];
            
            enemy->statusEffectCount--;
            i--;
        }
    }
}


void EnemyDamage(Enemy* enemy, unsigned short damage)
{
    if (enemy == nullptr) return;
    if (damage >= enemy->health)
    {
        enemy->health = 0;
    }else
    {
        enemy->health -= (short)damage;
    }
}


//--------------------
// ITEM FUNCTIONS
//--------------------

ItemData ItemCreate(short itemID, const char* name, ItemType type, ItemRarity rarity, short value, short cost)
{
    ItemData item;
    item.itemID = itemID;
    errno_t err = strcpy_s(item.name, sizeof(item.name), name);
    if (err != 0) printf("Failed to copy item name -> ItemCreate().\n");
    item.type = type;
    item.rarity = rarity;
    item.value = value;
    item.cost = cost;
    item.quantity = 1;

    switch (type)
    {
    case WEAPON:
        {
            sprintf_s(item.description, "A weapon that increases attack by %hd.", value);
            break;
        }
        case ARMOR:
        {
            sprintf_s(item.description, "Armor that increases defense by %hd.", value);
            break;
        }
    case POTION:
        {
            sprintf_s(item.description, "A Potion that increases %hd health.", value);
            break;
        }
    }
    return item;
}

void ItemApplyEffect(ItemData* item, Player* player)
{
    if (item == nullptr || player == nullptr) return;
    switch (item->type)
    {
    case WEAPON:
        {
            player->attack += item->value;
            printf("%sAttack increased by %hd!%s\n", GREEN, item->value, RESET);
            break;
        }
    case ARMOR:
        {
            player->defense += item->value;
            printf("%sDefense increased by %hd!%s\n", GREEN, item->value, RESET);
            break;
        }
    case POTION:
        {
            PlayerHeal(player, item->value);
            printf("%sRestored %hd health!%s\n", GREEN, item->value, RESET);
            break;
        }
    }
    
}

const char* ItemGetTypeName(ItemType type)
{
    switch (type)
    {
    case WEAPON:
        {
            return "Weapon";
        }
    case ARMOR:
        {
            return "Armor";
        }
    case POTION:
        {
            return "Potion";
        }
    }
    return "";
}

const char* ItemGetRarityName(ItemRarity rarity)
{
    switch (rarity)
    {
    case COMMON:
        {
            return "Common";
        }
    case UNCOMMON:
        {
            return "Uncommon";
        }
    case RARE:
        {
            return "Rare";
        }
    case LEGENDARY:
        {
            return "Legendary";
        }
    }
    return "Unknown";
}

void ItemDisplay(ItemData* item)
{
    if (item == nullptr) return;
    
    switch (item->rarity)
    {
    case COMMON:
        {
            printf("%s", RESET);
            break;
        }
    case UNCOMMON:
        {
            printf("%s", GREEN);
            break;
        }
    case RARE:
        {
            printf("%s", BLUE);
            break;
        }
    case LEGENDARY:
        {
            printf("%s", MAGENTA);
        }
    }
    printf("%s%s\n", item->name, RESET);
    printf("Type: %s\n", ItemGetTypeName(item->type));
    printf("Rarity: %s\n", ItemGetRarityName(item->rarity));
    printf("Value: %hd\n", item->value);
    printf("Cost: %hd gold\n", item->cost);
    printf("Description: %s\n", item->description);
}

ItemData ItemGenerateTreasure(unsigned short playerLevel)
{
    ItemRarity rarity;
    float roll = RandomFloat(0.f, 1.f);
    
    if (playerLevel < 3)
    {
        if (roll < 0.70f)
        {
            rarity = COMMON;
        }
        else if (roll < 0.95f)
        {
            rarity = UNCOMMON;
        }
        else
        {
            rarity = RARE;
        }
    }
    else if (playerLevel < 7)
    {
        if (roll < 0.50f)
        {
            rarity = COMMON;
        }
        else if (roll < 0.80f)
        {
            rarity = UNCOMMON;
        }
        else if (roll < 0.95f)
        {
            rarity = RARE;
        }
        else
        {
            rarity = LEGENDARY;
        }
    }
    else
    {
        if (roll < 0.30f)
        {
            rarity = COMMON;
        }
        else if (roll < 0.60f)
        {
            rarity = UNCOMMON;
        }
        else if (roll < 0.90f)
        {
            rarity = RARE;
        }
        else
        {
            rarity = LEGENDARY;
        }
    }
    
    ItemType type;
    float typeRoll = RandomFloat(0.f, 1.f);
    if (typeRoll < 0.40f)
    {
        type = WEAPON;
    }
    else if (typeRoll < 0.70f)
    {
        type = ARMOR;
    }
    else
    {
        type = POTION;
    }
    return ItemGenerateRandom(rarity, type);
}

ItemData ItemGenerateRandom(ItemRarity rarity, ItemType type)
{
    static short nextItemID = 1000;
    
    ItemData item;
    item.itemID = nextItemID++;
    item.type = type;
    item.rarity = rarity;
    item.quantity = 1;
    
    short baseValue = 0;
    short baseCost = 0;
    switch (rarity)
    {
    case COMMON:
        {
            baseValue = RandomShort(5, 15);
            baseCost = RandomShort(10, 30);
            break;
        }
    case UNCOMMON:
        {
            baseValue = RandomShort(15, 30);
            baseCost = RandomShort(30, 70);
            break;
        }
    case RARE:
        {
            baseValue = RandomShort(30, 50);
            baseCost = RandomShort(70, 150);
            break;
        }
    case LEGENDARY:
        {
            baseValue = RandomShort(50, 100);
            baseCost = RandomShort(150, 500); 
            break;
        }
    }
    item.value = baseValue;
    item.cost = baseCost;
    
    const char* rarityPrefix[4] = {"Rusty", "Fine", "Masterwork", "Legendary"};
    const char* weaponNames[5] = {"Sword", "Axe", "Mace", "Dagger", "Spear"};
    const char* armorNames[5] = {"Helmet", "Chestplate", "Gauntlets", "Boots", "Shield"};
    const char* potionNames[4] = {"Health Potion", "Healing Elixir", "Life Flask", "Restoration Brew"};
    
    switch (type)
    {
    case WEAPON:
        {
            sprintf_s(item.name, "%s %s", rarityPrefix[rarity], weaponNames[RandomShort(0, 4)]);
            sprintf_s(item.description, "A weapon that increases attack by %hd", baseValue);
            break;
        }
    case ARMOR:
        {
            sprintf_s(item.name, "%s %s", rarityPrefix[rarity], armorNames[RandomShort(0, 4)]);
            sprintf_s(item.description, "Armor that increases defense by %hd", baseValue);
            break;
        }
    case POTION:
        {
            sprintf_s(item.name, "%s", potionNames[rarity]);
            sprintf_s(item.description, "Restores %hd health", baseValue);
            break;
        }
    }
    return item;
}
//--------------------
// DUNGEON FUNCTIONS
//--------------------


Dungeon* DungeonInit()
{
    Dungeon* dungeon = (Dungeon*)malloc(sizeof(Dungeon));
    if (dungeon == nullptr)
    {
        printf("ERROR - Failed to allocate memory for dungeon.\n");
        return nullptr;
    }
    
    dungeon->totalRooms = MAX_ROOMS;
    
    for (short i = 0; i < MAX_ROOMS; i++)
    {
        dungeon->rooms[i].roomID = (short)1000 + i;
        
        dungeon->rooms[i].hasShop = false;
        dungeon->rooms[i].hasBoss = false;
        dungeon->rooms[i].explored = false;
        dungeon->rooms[i].encounterType = EMPTY;
        
        for (short j = 0; j < 4; j++)
        {
            dungeon->rooms[i].connections[j] = -1;
        }
        
        errno_t err = strcpy_s(dungeon->rooms[i].description, sizeof(dungeon->rooms[i].description), "An empty room");
        if (err!=0)
        {
            printf("Failed to copy room description data.\n");
        }
    }
    return dungeon;
}

void DungeonGenerateRooms(Dungeon* dungeon)
{
    if (dungeon == nullptr)
    {
        printf("ERROR - Dungeon ptr is null in DungeonGenRooms()\n");
        return;
    }
    const char* roomDescriptions[20] = 
        {
        "A dark corridor with stone walls",
        "A musty chamber filled with cobwebs",
        "A huge hall with ancient pillars",
        "A narrow passage with dripping water",
        "A circular room with mysterious runes",
        "A dusty library with old tombs",
        "An armoury with rusty weapons",
        "A torture chamber with old equipment",
        "A throne room in ruins",
        "A chapel with broken statues",
        "A treasury vault which is empty",
        "A kitchen with rotting food",
        "A bedroom with tattered curtains",
        "A study with scattered papers",
        "A laboratory with strange equipments",
        "A prison with empty cells and some skeleton ruins",
        "A garden overgrown with weeds",
        "A fountain room with stagnant water",
        "A war room filled with faded maps",
        "A crypt with ancient tombs"
        };
    for (short i = 0; i < MAX_ROOMS; i++)
    {
        errno_t err = strcpy_s(dungeon->rooms[i].description, sizeof(dungeon->rooms[i].description), roomDescriptions[i % 20]);
        if (err!=0)
        {
            printf("Failed to copy room description data.\n");
        }
        
        dungeon->rooms[i].hasShop = false;
        
        if (i == 0)
        {
            dungeon->rooms[i].encounterType = EMPTY;
            dungeon->rooms[i].hasShop = false;
            dungeon->rooms[i].explored = true;
            continue;
        }
        if (i == MAX_ROOMS - 1)
        {
            dungeon->rooms[i].encounterType = BOSS;
            dungeon->rooms[i].hasBoss = true;
            dungeon->rooms[i].hasShop = false;
            continue;
        }
        float roll = RandomFloat(0.0f, 1.0f);
        if (roll < 0.70f)
        {
            dungeon->rooms[i].encounterType = ENEMY;
        }
        else if (roll < 0.80f)
        {
            dungeon->rooms[i].encounterType = TREASURE;
        }
        else if (roll < 0.85f)
        {
            dungeon->rooms[i].encounterType = QUEST;
        }
        else if (roll < 0.93f)
        {
            dungeon->rooms[i].encounterType = EMPTY;
            dungeon->rooms[i].hasShop = true;
        }else
        {
            dungeon->rooms[i].encounterType = EMPTY;
        }
    }
}

void DungeonGenerateConnections(Dungeon* dungeon)
{
    if (dungeon == nullptr)
    {
        return;
    }
    for (short i = 0; i < MAX_ROOMS; i++)
    {
        for (short j = 0; j < 4; j++)
        {
            dungeon->rooms[i].connections[j] = -1;
        }
    }
    
    for (short row = 0; row < DUNGEON_ROWS; row++)
    {
        for (short col = 0; col < DUNGEON_COLS; col++)
        {
            short currentIndex = DungeonGetRoomIndex(row,col);
            if (row > 0)
            {
                short northIndex = DungeonGetRoomIndex((short)row-1,col);
                dungeon->rooms[currentIndex].connections[NORTH] = northIndex;
            }
            if (col < DUNGEON_COLS - 1)
            {
                short eastIndex = DungeonGetRoomIndex(row,(short)col + 1);
                dungeon->rooms[currentIndex].connections[EAST] = eastIndex;
            }
            if (row < DUNGEON_ROWS - 1)
            {
                short southIndex = DungeonGetRoomIndex((short)row + 1,col);
                dungeon->rooms[currentIndex].connections[SOUTH] = southIndex;
            }
            if (col > 0)
            {
                short westIndex = DungeonGetRoomIndex(row,(short)col - 1);
                dungeon->rooms[currentIndex].connections[WEST] = westIndex;
            }
        }
    }
}

void DungeonFree(Dungeon* dungeon)
{
    if (dungeon != nullptr)
    {
        free(dungeon);
    }
}

void DungeonDisplayRoom(Player* player, Dungeon* dungeon)
{
    if (player == nullptr || dungeon == nullptr)
    {
        printf("Failed to display room as either player or dungeon is a nullptr.\n");
        return;
    }
    unsigned short currRoom = player->currentRoom;
    Room* room = &dungeon->rooms[currRoom];
    
    CLEAR_SCREEN();
    UI::UI_PrintHeader("CURRENT ROOM");
    printf("\n");
    printf("Room: %hd: - %s\n", room->roomID, room->description);
    printf("\n");
    
    //Exits
    UI::UI_PrintSection("Available Exits");
    bool hasExits = false;
    for (short i = 0; i < 4; i++)
    {
        if (room->connections[i] != -1)
        {
            hasExits = true;
            
            short connectedRoomIndex = room->connections[i];
            bool isExplored = dungeon->rooms[connectedRoomIndex].explored;
            printf("- %s (to room %hd)%s\n", DungeonGetDirectionName((Direction) i), connectedRoomIndex, isExplored ? " (explored)" : "");
        }
    }
    
    if (!hasExits)
    {
        printf("No Exits are available!\n");
    }
    printf("\n");
    if (room->hasShop)
    {
        UI::UI_DisplayInfoMessage("There's a shop in this room!");
    }
    
    if (room->hasBoss)
    {
        UI::UI_DisplayWarningMessage("You sense a powerful boss here!");
    }
    
    if (room->encounterType != EMPTY && !room->explored)
    {
        UI::UI_DisplayWarningMessage("Something awaits you here...");
    }
}

void DungeonDisplayActionMenu()
{
    printf("\n");
    printf("1) Move to another room\n");
    printf("2) Check Stats\n");
    printf("3) View Inventory\n");
    printf("4) View Quests\n");
    printf("5) View Abilities\n");
    printf("6) View Map\n");
    printf("7) Save Game\n");
    printf("8) Pause Menu\n");
    printf("\nYour Choice: ");
}

void DungeonMoveToRoom(Player* player, Dungeon* dungeon, Direction direction)
{
    if (player == nullptr || dungeon == nullptr)
    {
        printf("Ptrs are null -> Move to another room func.\n");
        return;
    }
    unsigned short currRoom = player->currentRoom;
    short nextRoom = dungeon->rooms[currRoom].connections[direction];
    if (nextRoom == -1)
    {
        UI::UI_DisplayErrorMessage("You cannot go that way!");
        UI::UI_TimedPause(1000);
        return;
    }
    player->currentRoom = nextRoom;
    UI::UI_DisplaySuccessMessage("Moving to next room...");
    UI::UI_TimedPause(500);
}

Direction DungeonGetDirectionInput()
{
    UI::UI_PrintCentered("Choose Direction:");
    printf("1) North\n");
    printf("2) East\n");
    printf("3) South\n");
    printf("4) West\n");
    
    unsigned short choice = UI::UI_GetMenuInput(1, 4);
    return Direction(choice - 1);
}

const char* DungeonGetDirectionName(Direction dir)
{
    switch (dir)
    {
    case NORTH:
        {
            return "North";
        }
    case EAST:
        {
            return "East";
        }
    case SOUTH:
        {
            return "South";
        }
    case WEST:
        {
            return "West";
        }
    }
    return "North";
}

void DungeonDisplayMap(Player* player, Dungeon* dungeon)
{
    if (player == nullptr || dungeon == nullptr)
    {
        return;
    }
    UI::UI_PrintHeader("DUNGEON MAP");
    printf("\nLegend: [P]=You | [X]=Explored | [?] = Unknown | [B]=Boss [S]=Shop\n\n");
    for (short row=0; row < DUNGEON_ROWS; row++)
    {
        for (short col=0; col < DUNGEON_COLS; col++)
        {
            short index = DungeonGetRoomIndex(row, col);
            Room* room = &dungeon->rooms[index];
            if (index == player->currentRoom)
            {
                printf("[P]");
            }
            else if (room->explored)
            {
                if (room->hasBoss)
                {
                    printf("[B]");
                }
                else if (room->hasShop)
                {
                    printf("[S]");
                }
                else
                {
                    printf("[X]");
                }
            }else
            {
                printf("[?]");
            }
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
    printf("Current Position: Room %hd\n", player->currentRoom);
    printf("Explored: %hd/%hd\n", CountExploredRooms(dungeon), MAX_ROOMS);
}

short DungeonGetRoomIndex(short row, short col)
{
    return DUNGEON_COLS * row + col;
}

//--------------------
// COMBAT FUNCTIONS
//--------------------

CombatResult CombatStart(Player* player, Enemy* enemy, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || game == nullptr) return COMBAT_DEFEAT;
    
    bool combatActive = true;
    CombatResult result = COMBAT_DEFEAT;
    while (combatActive)
    {
        CLEAR_SCREEN();
        UI::UI_PrintHeader("COMBAT");
        
        UI::UI_PrintSection("YOU");
        PlayerDisplayStatusBar(player);
        printf("\n");
        
        UI::UI_PrintSection("ENEMY");
        EnemyDisplayStats(enemy);
        printf("\n");
        
        CombatDisplayMenu(player, enemy);
        unsigned short choice = UI::UI_GetMenuInput(1, 4);
        switch (choice)
        {
        case 1:
            {
                CombatPlayerAttack(player, enemy, game);
                break;
            }
        case 2:
            {
                if (player->abilityCount > 0)
                {
                    CombatUseAbility(player, enemy, game);
                }
                else
                {
                    UI::UI_DisplayErrorMessage("No abilities unlocked yet!");
                    UI::UI_TimedPause(1000);
                    continue;
                }
                break;
            }
        case 3:
            {
                if (game->inventory != nullptr && game->inventory->itemCount > 0)
                {
                    CombatUseItem(player, enemy, game);
                }else
                {
                    UI::UI_DisplayErrorMessage("No items in the inventory!");
                    UI::UI_TimedPause(1000);
                    continue;
                }
                break;
            }
        case 4:
            {
                if (CombatAttemptEscape(player) == true)
                {
                    UI::UI_DisplaySuccessMessage("Escaped Successfully!");
                    UI::UI_TimedPause(1000);
                    result = COMBAT_ESCAPE;
                    combatActive = false;
                    continue;
                }
                else
                {
                    UI::UI_DisplayErrorMessage("Failed to escape!");
                    UI::UI_TimedPause(1000);
                }
                break;
            }
        default:
            {
                UI::UI_DisplayWarningMessage("Invalid choice!");
                UI::UI_TimedPause(500);
                continue;
            }
        }
        if (!EnemyIsAlive(enemy))
        {
            result = COMBAT_VICTORY;
            combatActive = false;
            continue;
        }
        
        UI::UI_PrintSection("Enemy's Turn");
        UI::UI_TimedPause(500);
        CombatEnemyAttack(player, enemy, game);
        UI::UI_TimedPause(1000);
        
        if (player->health <= 0)
        {
            result = COMBAT_DEFEAT;
            combatActive = false;
            continue;
        }
        PlayerUpdateStatusEffects(player);
        EnemyUpdateStatusEffects(enemy);
        
        CombatUpdateCooldowns(player);
        UI::UI_PauseScreen();
    }
    return result;
}

void CombatAwardVictory(Player* player, Enemy* enemy, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || game == nullptr) return;
    
    CLEAR_SCREEN();
    UI::UI_PrintHeader("VICTORY");
    printf("\n");
    
    PlayerGainExperience(player, enemy->expReward);
    PlayerGainGold(player, enemy->goldReward);
    
    game->stats->totalEnemiesDefeated++;
    game->stats->totalGoldEarned += enemy->goldReward;
    
    //QuestUpdateProgress(game->questLog, KILL_ENEMIES, 1);
    
    if (RandomChance(0.30f))
    {
        printf("\n");
        ItemData loot = ItemGenerateRandom(enemy->lootRarity, (ItemType)RandomShort(0,2));
        printf("%s dropped: %s\n", enemy->name, loot.name);
        
        if (!InventoryIsFull(game->inventory))
        {
            if (InventoryAddItem(game->inventory, loot))
            {
                UI::UI_DisplaySuccessMessage("Item added to inventory!");
                game->stats->itemsCollected++;
            }
        }
        else
        {
            UI::UI_DisplayWarningMessage("Inventory full! Item left behind.");
        }
    }
    printf("\n");
    UI::UI_PauseScreen();
} // Quest left..

void CombatDisplayMenu(Player* player, Enemy* enemy)
{
    UI::UI_PrintDivider();
    printf("Choose your action:\n");
    printf("1) Attack.\n");
    printf("2) Use Ability. [%hu unlocked]\n", player->abilityCount);
    printf("3) Use Item.\n");
    printf("4) Attempt Escape.\n");
    UI::UI_PrintDivider();
}

void CombatPlayerAttack(Player* player, Enemy* enemy, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || game == nullptr) return;
    
    bool isCritical = RandomChance(0.15f);
    float multiplier = isCritical ? 2.0f : 1.0f;
    
    unsigned short damage = CombatCalculateDamage(player->attack, enemy->defense, multiplier);
    
    EnemyDamage(enemy, damage);
    
    game->stats->totalDamageDealt += damage;
    
    UI::UI_DisplayCombatAnimation("Your attack", damage, isCritical);
    
    if (!EnemyIsAlive(enemy))
    {
        UI::UI_DisplaySuccessMessage("Enemy Defeated!");
    }
}

void CombatEnemyAttack(Player* player, Enemy* enemy, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || game == nullptr) return;
    
    bool isCritical = RandomChance(0.10f);
    float multiplier = isCritical ? 1.5f : 1.0f;
    unsigned short damage = CombatCalculateDamage(enemy->attack, player->defense, multiplier);
    
    PlayerDamage(player, damage);
    game->stats->totalDamageTaken += damage;
    
    char action[50];
    sprintf_s(action, "%s's attack", enemy->name);
    UI::UI_DisplayCombatAnimation(action, damage, isCritical);
    
    if (player->health <= 0)
    {
        UI::UI_DisplayErrorMessage("You have been defeated!");
    }
}

void CombatUseAbility(Player* player, Enemy* enemy, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || game == nullptr)
    {
        return;
    }
    
    if (player->abilityCount == 0)
    {
        UI::UI_DisplayErrorMessage("No abilities unlocked yet!");
        UI::UI_TimedPause(1000);
        return;
    }
    
    CLEAR_SCREEN();
    UI::UI_PrintHeader("SELECT ABILITY");
    printf("\n");
    
    // Display available abilities
    bool hasAvailableAbility = false;
    for (unsigned short i = 0; i < player->abilityCount; i++)
    {
        Ability* ability = &player->unlockedAbilities[i];
        
        printf("%d. %s%s%s\n", i + 1, CYAN, ability->name, RESET);
        printf("   %s\n", ability->description);
        printf("   Damage: %.1fx | Cooldown: %d turns\n", ability->damageMultiplier, ability->cooldown);
        
        if (ability->cooldownRemaining > 0)
        {
            printf("   %s[On Cooldown: %d turns remaining]%s\n", RED, ability->cooldownRemaining, RESET);
        }
        else
        {
            printf("   %s[READY]%s\n", GREEN, RESET);
            hasAvailableAbility = true;
        }
        printf("\n");
    }
    
    printf("0. Cancel\n");
    UI::UI_PrintDivider();
    
    if (!hasAvailableAbility)
    {
        UI::UI_DisplayWarningMessage("All abilities are on cooldown!");
        UI::UI_TimedPause(1500);
        return;
    }
    
    printf("Select ability (0 to cancel): ");
    unsigned short choice = UI::UI_GetMenuInput(0, player->abilityCount);
    
    if (choice == 0)
    {
        UI::UI_DisplayInfoMessage("Cancelled.");
        UI::UI_TimedPause(500);
        return;
    }
    
    Ability* selectedAbility = &player->unlockedAbilities[choice - 1];
    
    // Check if ability is on cooldown
    if (selectedAbility->cooldownRemaining > 0)
    {
        UI::UI_DisplayErrorMessage("That ability is still on cooldown!");
        printf("Turns remaining: %d\n", selectedAbility->cooldownRemaining);
        UI::UI_TimedPause(1500);
        return;
    }
    
    // Use the ability
    printf("\n");
    printf("%s>>> Using %s%s%s <<<%s\n\n", CYAN, YELLOW, selectedAbility->name, CYAN, RESET);
    UI::UI_TimedPause(500);
    
    // Calculate base damage
    unsigned short baseDamage = CombatCalculateDamage(player->attack, enemy->defense, selectedAbility->damageMultiplier);
    
    // Handle different ability types based on abilityId
    switch (selectedAbility->abilityId)
    {
    case 1: // Power Strike - 1.5x damage
        {
            bool isCritical = RandomChance(0.20f); // Slightly higher crit chance for abilities
            unsigned short finalDamage = isCritical ? (unsigned short)(baseDamage * 1.5f) : baseDamage;
            
            EnemyDamage(enemy, finalDamage);
            game->stats->totalDamageDealt += finalDamage;
            
            char action[100];
            sprintf_s(action, sizeof(action), "%s", selectedAbility->name);
            UI::UI_DisplayCombatAnimation(action, finalDamage, isCritical);
            break;
        }
    case 2: // Double Slash - Hit twice
        {
            unsigned short hit1Damage = CombatCalculateDamage(player->attack, enemy->defense, 1.0f);
            unsigned short hit2Damage = CombatCalculateDamage(player->attack, enemy->defense, 1.0f);
            
            // First hit
            EnemyDamage(enemy, hit1Damage);
            game->stats->totalDamageDealt += hit1Damage;
            
            char action1[100];
            sprintf_s(action1, sizeof(action1), "%s (1st Strike)", selectedAbility->name);
            UI::UI_DisplayCombatAnimation(action1, hit1Damage, false);
            UI::UI_TimedPause(600);
            
            // Second hit (only if enemy still alive)
            if (EnemyIsAlive(enemy))
            {
                EnemyDamage(enemy, hit2Damage);
                game->stats->totalDamageDealt += hit2Damage;
                
                char action2[100];
                sprintf_s(action2, sizeof(action2), "%s (2nd Strike)", selectedAbility->name);
                UI::UI_DisplayCombatAnimation(action2, hit2Damage, false);
                
                printf("\n%sTotal Damage: %hu%s\n", YELLOW, hit1Damage + hit2Damage, RESET);
            }
            break;
        }
    case 3: // Life Drain - Damage + heal 50% of damage dealt
        {
            EnemyDamage(enemy, baseDamage);
            game->stats->totalDamageDealt += baseDamage;
            
            char action[100];
            sprintf_s(action, sizeof(action), "%s", selectedAbility->name);
            UI::UI_DisplayCombatAnimation(action, baseDamage, false);
            
            // Heal for 50% of damage dealt
            unsigned short healAmount = baseDamage / 2;
            if (healAmount < 1) healAmount = 1;
            
            PlayerHeal(player, healAmount);
            printf("%s🩸 Life Drain restored %hu health!%s\n", GREEN, healAmount, RESET);
            break;
        }
    case 4: // Whirlwind - 2x damage
        {
            bool isCritical = RandomChance(0.15f);
            unsigned short finalDamage = isCritical ? (unsigned short)(baseDamage * 1.5f) : baseDamage;
            
            EnemyDamage(enemy, finalDamage);
            game->stats->totalDamageDealt += finalDamage;
            
            printf("%s🌀 You spin with devastating force!%s\n", CYAN, RESET);
            
            char action[100];
            sprintf_s(action, sizeof(action), "%s", selectedAbility->name);
            UI::UI_DisplayCombatAnimation(action, finalDamage, isCritical);
            break;
        }
    case 5: // Devastating Blow - 3x damage
        {
            bool isCritical = RandomChance(0.25f); // Higher crit chance for ultimate
            unsigned short finalDamage = isCritical ? (unsigned short)(baseDamage * 1.5f) : baseDamage;
            
            EnemyDamage(enemy, finalDamage);
            game->stats->totalDamageDealt += finalDamage;
            
            printf("%s💥 You unleash a DEVASTATING BLOW!%s\n", RED, RESET);
            
            char action[100];
            sprintf_s(action, sizeof(action), "%s", selectedAbility->name);
            UI::UI_DisplayCombatAnimation(action, finalDamage, isCritical);
            
            if (isCritical)
            {
                printf("%s⚡ MASSIVE CRITICAL HIT! ⚡%s\n", YELLOW, RESET);
            }
            break;
        }
    default:
        {
            // Generic ability handling for any future abilities
            EnemyDamage(enemy, baseDamage);
            game->stats->totalDamageDealt += baseDamage;
            
            char action[100];
            sprintf_s(action, sizeof(action), "%s", selectedAbility->name);
            UI::UI_DisplayCombatAnimation(action, baseDamage, false);
            break;
        }
    }
    
    // Set cooldown
    selectedAbility->cooldownRemaining = selectedAbility->cooldown;
    
    // Check if enemy is defeated
    if (!EnemyIsAlive(enemy))
    {
        printf("\n");
        UI::UI_DisplaySuccessMessage("Enemy Defeated!");
    }
    
    UI::UI_TimedPause(1000);
}

void CombatUseItem(Player* player, Enemy* enemy, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || game == nullptr) return;
    
    CLEAR_SCREEN();
    InventoryDisplay(game->inventory);
    
    printf("Enter ItemID to use (0 to cancel): > ");
    short itemID;
    scanf_s("%hd", &itemID);
    while (getchar() != '\n');
    
    if (itemID == 0)
        return;
    InventoryUseItem(game->inventory, player, itemID);
    UI::UI_TimedPause(1000);
}

bool CombatAttemptEscape(Player* player)
{
    if (player == nullptr) return false;
    
    float escapeChance = 0.40f;
    
    if (player->trait == TRAIT_QUICK_HANDS)
        escapeChance += 0.15f;
    
    return RandomChance(escapeChance);
}

void CombatUpdateCooldowns(Player* player)
{
    if (player == nullptr) return;
    
    for (unsigned short i = 0; i < player->abilityCount; i++)
    {
        if (player->unlockedAbilities[i].cooldownRemaining > 0)
            player->unlockedAbilities[i].cooldownRemaining--;
    }
}

unsigned short CombatCalculateDamage(unsigned short attack, unsigned short defense, float multiplier)
{
    float baseDamage = (float) attack * multiplier;
    float reduction = (float) defense * 0.5f;
    float finalDamage = baseDamage - reduction;
    
    if (finalDamage < 1.0f)
        finalDamage = 1.0f;
    
    return (unsigned short)finalDamage;
}


//--------------------
// INVENTORY FUNCTIONS
//--------------------

Inventory* InventoryCreate()
{
    Inventory* inventory = (Inventory*)malloc(sizeof(Inventory));
    if (inventory == nullptr)
    {
        printf("Failed to allocate memory for Inventory.\n");
        return nullptr;
    }
    inventory->head = nullptr;
    inventory->itemCount = 0;
    return inventory;
}

void InventoryFree(Inventory* inventory)
{
    if (inventory == nullptr)
    {
        return;
    }
    InventoryNode* current = inventory->head;
    while (current != nullptr)
    {
        InventoryNode* next = current->next;
        free(current);
        current = next;
    }
    free(inventory);
}

bool InventoryIsFull(Inventory* inventory)
{
    if (inventory == nullptr) return true;
    return inventory->itemCount >= MAX_INVENTORY;
}

unsigned short InventoryGetTotalValue(Inventory* inventory)
{
    if (inventory == nullptr) return 0;
    unsigned short totalValue = 0;
    InventoryNode* current = inventory->head;
    while (current != nullptr)
    {
        totalValue += (current->item.cost * current->item.quantity);
        current = current->next;
    }
    return totalValue;
}

bool InventoryAddItem(Inventory* inventory, ItemData item)
{
    if (inventory == nullptr)
    {
        return false;
    }
    
    if (InventoryIsFull(inventory))
    {
        UI::UI_DisplayErrorMessage("Inventory is full!");
        return false;
    }
    
    InventoryNode* current = inventory->head;
    while (current != nullptr)
    {
        if (current->item.itemID == item.itemID)
        {
            current->item.quantity += (short)item.quantity;
            return true;
        }
        current = current->next;
    }
    
    InventoryNode* newNode = (InventoryNode*)malloc(sizeof(InventoryNode));

    if (newNode == nullptr)
    {
        printf("ERROR - Failed to allocate memory for inventory node\n");
        return false;
    }

    newNode->item = item;
    newNode->next = inventory->head;
    
    inventory->head = newNode;
    inventory->itemCount++;
    
    return true;
}

bool InventoryRemoveItem(Inventory* inventory, short itemID)
{
    if (inventory == nullptr || inventory->head == nullptr) return false;
    InventoryNode* current = inventory->head;
    InventoryNode* prev = nullptr;
    while (current != nullptr)
    {
        if (current->item.itemID == itemID)
        {
            current->item.quantity--;
            if (current->item.quantity <= 0)
            {
                // remove node from list
                if (prev == nullptr)
                    inventory->head = current->next;
                else
                    prev->next = current->next;
                free(current);
                inventory->itemCount--;
            }
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

ItemData* InventoryFindItem(Inventory* inventory, short itemID)
{
    if (inventory == nullptr) return nullptr;
    InventoryNode* current = inventory->head;
    while (current != nullptr)
    {
        if (current->item.itemID == itemID) return &current->item;
        current = current->next;
    }
    return nullptr;
}

void InventoryDisplay(Inventory* inventory)
{
    if (inventory == nullptr)
    {
        UI::UI_DisplayErrorMessage("Inventory is null!");
        return;
    }
    UI::UI_PrintHeader("INVENTORY");
    if (inventory->itemCount == 0)
    {
        printf("\nInventory is empty!\n");
        return;
    }
    printf("\nItems: %hd/%hd\n\n", inventory->itemCount, MAX_INVENTORY);
    UI::UI_PrintDivider();
    InventoryNode* current = inventory->head;
    short index = 1;
    while (current != nullptr)
    {
        ItemData* item = &current->item;
        printf("%hd. ", index);
        
        switch (item->rarity)
        {
        case COMMON:
            {
                printf("%s", RESET);
                break;
            }
        case UNCOMMON:
            {
                printf("%s", GREEN);
                break;
            }
        case RARE:
            {
                printf("%s", BLUE);
                break;
            }
        case LEGENDARY:
            {
                printf("%s", MAGENTA);
                break;
            }
        }
        printf("%s%s (x %hd)\n", item->name, RESET, item->quantity);
        printf(" ID: %hd | Type: %s | Value: %hd | Cost: %hd\n", item->itemID, ItemGetTypeName(item->type), item->value, item->cost);
        printf("%s\n\n", item->description);
        
        current = current->next;
        index++;
    }
    UI::UI_PrintDivider();
    printf("Total Value: %hd gold\n", InventoryGetTotalValue(inventory));
}

void InventoryUseItem(Inventory* inventory, Player* player, short itemID)
{
    if (inventory == nullptr || player == nullptr) return;
    ItemData* item = InventoryFindItem(inventory, itemID);
    if (item == nullptr)
    {
        UI::UI_DisplayErrorMessage("Inventory is null!"); return;
    }
    ItemApplyEffect(item, player);
    InventoryRemoveItem(inventory, itemID);
    UI::UI_DisplaySuccessMessage("Item used!");
}

//--------------------
// QUEST FUNCTIONS
//--------------------

QuestLog* QuestInit()
{
    QuestLog* questLog = (QuestLog*)malloc(sizeof(QuestLog));
    
    if (questLog == nullptr)
    {
        printf("ERROR - Failed to allocate memory for quest log\n");
        return nullptr;
    }
    
    memset(questLog, 0, sizeof(QuestLog));
    
    return questLog;
}

void QuestFree(QuestLog* questLog)
{
    if (questLog != nullptr) free(questLog);
}

void QuestGenerate(QuestLog* questLog, unsigned short playerLevel)
{
    if (questLog == nullptr || questLog->questCount >= MAX_QUESTS)
    {
        return;
    }
    
    QuestData* quest = &questLog->quests[questLog->questCount];
    
    quest->questID = (short)questLog->questCount + 1;
    quest->completed = false;
    quest->currentProgress = 0;
    
    float roll = RandomFloat(0.0f, 1.0f);
    
    if (roll < 0.50f)
    {
        quest->objectiveType = KILL_ENEMIES;
        quest->targetValue = RandomShort(3, 10);
        quest->rewardGold = (short)quest->targetValue * 20;
        sprintf_s(quest->title, sizeof(quest->title), "Slay %hd Monsters", quest->targetValue);
        sprintf_s(quest->description, sizeof(quest->description), "Defeat %hd enemies in the dungeon", quest->targetValue);
    }
    else if (roll < 0.80f)
    {
        quest->objectiveType = COLLECT_ITEMS;
        quest->targetValue = RandomShort(2, 5);
        quest->rewardGold = quest->targetValue * 30;
        sprintf_s(quest->title, sizeof(quest->title), "Collect %hd Items", quest->targetValue);
        sprintf_s(quest->description, sizeof(quest->description), "Find %hd items in treasure chests", quest->targetValue);
    }
    else
    {
        quest->objectiveType = REACH_LEVEL;
        quest->targetValue = playerLevel + RandomShort(1, 3);
        quest->rewardGold = quest->targetValue * 50;
        sprintf_s(quest->title, sizeof(quest->title), "Reach Level %hd", quest->targetValue);
        sprintf_s(quest->description, sizeof(quest->description), "Train and reach level %hd", quest->targetValue);
    }
    
    questLog->questCount++;
}

void QuestDisplay(QuestLog* questLog, Player* player)
{
    if (questLog == nullptr)
    {
        UI::UI_DisplayErrorMessage("QuestLog is null!"); return;
    }
    if (player == nullptr) return;
    UI::UI_PrintHeader("QUEST LOG");
    if (questLog->questCount == 0)
    {
        printf("\nNo Active Quests!\n");
        return;
    }
    printf("Active Quests: %hd/%hd\n\n", questLog->questCount, MAX_QUESTS);
    UI::UI_PrintDivider();
    for (unsigned short i = 0; i < questLog->questCount; i++)
    {
        QuestData* quest = &questLog->quests[i];
        if (quest->completed)
        {
            printf("%s[COMPLETED]%s", GREEN, RESET);
        }
        else
        {
            printf("%s[ACTIVE]%s", YELLOW, RESET);
        }
        printf("%s\n", quest->title);
        printf("%s\n", quest->description);
        printf(" Progress: %hd/%hd\n", quest->currentProgress, quest->targetValue);
        printf(" Reward: %hd gold\n", quest->rewardGold);
    }
    UI::UI_PrintDivider();
}

void QuestCheckCompletion(QuestLog* questLog, Player* player, GameInstance* game)
{
    if (questLog == nullptr || player == nullptr || game == nullptr)
    {
        return;
    }
    
    for (short i = 0; i < questLog->questCount; i++)
    {
        QuestData* quest = &questLog->quests[i];
        
        if (quest->completed)
        {
            continue;
        }
        
        switch (quest->objectiveType)
        {
        case KILL_ENEMIES:
            if (quest->currentProgress >= quest->targetValue)
            {
                QuestAwardReward(questLog, player, i);
            }
            break;
            
        case COLLECT_ITEMS:
            if (game->stats->itemsCollected >= quest->targetValue)
            {
                quest->currentProgress = (short)game->stats->itemsCollected;
                if (quest->currentProgress >= quest->targetValue)
                {
                    QuestAwardReward(questLog, player, i);
                }
            }
            break;
            
        case REACH_LEVEL:
            quest->currentProgress = (unsigned short)player->level;
            if (player->level >= quest->targetValue)
            {
                QuestAwardReward(questLog, player, i);
            }
            break;
        }
    }
}

void QuestUpdateProgress(QuestLog* questLog, QuestObjectiveType type, short amount)
{
    if (questLog == nullptr)
        return;
    for (unsigned short i = 0; i < questLog->questCount; i++)
    {
        QuestData* quest = &questLog->quests[i];
        if (!quest->completed && quest->objectiveType == type)
        {
            quest->currentProgress += (short)amount;
            printf("%s", CYAN);
            printf("[QUEST PROGRESS] %s: %hd/%hd\n",quest->title, quest->currentProgress, quest->targetValue);
            printf("%s", RESET);
        }
    }
}

void QuestAwardReward(QuestLog* questLog, Player* player, short questIndex)
{
    if (questLog == nullptr || player == nullptr || questIndex >= questLog->questCount || questIndex < 0) return;
    QuestData* quest = &questLog->quests[questIndex];
    if (quest->completed)
        return;
    quest->completed = true;
    UI::UI_DisplaySuccessMessage("QUEST COMPLETED!");
    printf("Quest: %s\n", quest->title);
    printf("Reward: %hd gold\n", quest->rewardGold);
    
    PlayerGainGold(player, quest->rewardGold);
    UI::UI_TimedPause(2000);
}

bool QuestIsComplete(QuestData* quest)
{
    if (quest == nullptr) return false;
    return quest->completed;
}


//--------------------
// ABILITY SYSTEM FUNCTIONS
//--------------------

void AbilityDisplayList(Player* player)
{
    if (player == nullptr)
    {
        return;
    }
    
    UI::UI_PrintHeader("ABILITIES");
    
    if (player->abilityCount == 0)
    {
        printf("\nNo abilities unlocked yet!\n");
        printf("Abilities unlock as you level up.\n");
        return;
    }
    
    printf("\nUnlocked Abilities: %hu/%d\n\n", player->abilityCount, MAX_ABILITIES);
    UI::UI_PrintDivider();
    
    for (unsigned short i = 0; i < player->abilityCount; i++)
    {
        Ability* ability = &player->unlockedAbilities[i];
        
        printf("%d. %s%s%s\n", i + 1, CYAN, ability->name, RESET);
        printf("   %s\n", ability->description);
        printf("   Damage Multiplier: %.1fx\n", ability->damageMultiplier);
        printf("   Cooldown: %d turns\n", ability->cooldown);
        
        if (ability->cooldownRemaining > 0)
        {
            printf("   %s[On Cooldown: %d turns]%s\n", RED, ability->cooldownRemaining, RESET);
        }
        else
        {
            printf("   %s[Ready to use!]%s\n", GREEN, RESET);
        }
        
        printf("\n");
    }
    
    UI::UI_PrintDivider();
}

void AbilityCheckUnlocks(Player* player, GameInstance* game)
{
    if (player == nullptr || game == nullptr)
    {
        printf("ERROR - AbilityCheckUnlocks: null pointer\n");
        return;
    }
    
    // Iterate through ALL abilities in the game's ability list
    for (unsigned short i = 0; i < game->abilityCount; i++)
    {
        Ability* ability = &game->abilityList[i];
        
        // Check if this ability should unlock at current level
        if (ability->unlockedAtLevel == player->level)
        {
            // Check if player already has this ability
            bool alreadyUnlocked = false;
            for (unsigned short j = 0; j < player->abilityCount; j++)
            {
                if (player->unlockedAbilities[j].abilityId == ability->abilityId)
                {
                    alreadyUnlocked = true;
                    break;
                }
            }
            
            // Unlock the ability if not already unlocked and player has room
            if (!alreadyUnlocked && player->abilityCount < MAX_ABILITIES)
            {
                // Copy ability to player's unlocked abilities
                player->unlockedAbilities[player->abilityCount] = *ability;
                player->unlockedAbilities[player->abilityCount].cooldownRemaining = 0;
                player->abilityCount++;
                
                // Display unlock notification
                CLEAR_SCREEN();
                printf("\n");
                printf("%s", YELLOW);
                UI::UI_PrintHeader("NEW ABILITY UNLOCKED!");
                printf("%s", RESET);
                printf("\n");
                
                printf("%s⚔️  %s%s%s  ⚔️%s\n\n", CYAN, YELLOW, ability->name, CYAN, RESET);
                
                printf("%s\n\n", ability->description);
                
                UI::UI_PrintSection("ABILITY STATS");
                printf("Damage Multiplier: %s%.1fx%s\n", GREEN, ability->damageMultiplier, RESET);
                printf("Cooldown: %s%d turns%s\n", CYAN, ability->cooldown, RESET);
                printf("Unlocked at Level: %s%hu%s\n", YELLOW, ability->unlockedAtLevel, RESET);
                
                printf("\n");
                
                // Show ability-specific tips
                switch (ability->abilityId)
                {
                case 1: // Power Strike
                    printf("%sTip: Power Strike deals 1.5x damage - great for finishing enemies!%s\n", CYAN, RESET);
                    break;
                case 2: // Double Slash
                    printf("%sTip: Double Slash hits twice - effective against low defense enemies!%s\n", CYAN, RESET);
                    break;
                case 3: // Life Drain
                    printf("%sTip: Life Drain heals you for 50%% of damage dealt - sustain in long fights!%s\n", CYAN, RESET);
                    break;
                case 4: // Whirlwind
                    printf("%sTip: Whirlwind deals massive 2x damage - perfect for tough enemies!%s\n", CYAN, RESET);
                    break;
                case 5: // Devastating Blow
                    printf("%sTip: Devastating Blow is your ultimate - 3x damage with high crit chance!%s\n", CYAN, RESET);
                    break;
                }
                
                printf("\n");
                UI::UI_DisplaySuccessMessage("Use abilities in combat with the 'Use Ability' option!");
                UI::UI_PauseScreen();
            }
        }
    }
}

bool AbilityIsUnlocked(Player* player, int abilityId)
{
    if (player == nullptr) return false;
    for (unsigned short i = 0; i < player->abilityCount; i++)
    {
        if (player->unlockedAbilities[i].abilityId == abilityId)
            return true;
    }
    return false;
}

Ability* AbilityGetById(Player* player, int abilityId)
{
    if (player == nullptr) return nullptr;
    for (unsigned short i = 0; i < player->abilityCount; i++)
    {
        if (player->unlockedAbilities[i].abilityId == abilityId)
            return &player->unlockedAbilities[i];
    }
    return nullptr;
}

void AbilityUse(Player* player, Enemy* enemy, Ability* ability, GameInstance* game)
{
    if (player == nullptr || enemy == nullptr || ability == nullptr)
    {
        return;
    }
    
    unsigned short damage = CombatCalculateDamage(player->attack, enemy->defense, ability->damageMultiplier);
    
    if (ability->abilityId == 2)
    {
        EnemyDamage(enemy, damage);
        char action[100];
        sprintf_s(action, sizeof(action), "%s (1st hit)", ability->name);
        UI::UI_DisplayCombatAnimation(action, damage, false);
        UI::UI_TimedPause(500);
        
        if (EnemyIsAlive(enemy))
        {
            EnemyDamage(enemy, damage);
            sprintf_s(action, sizeof(action), "%s (2nd hit)", ability->name);
            UI::UI_DisplayCombatAnimation(action, damage, false);
        }
    }
    else if (ability->abilityId == 3)
    {
        EnemyDamage(enemy, damage);
        short healAmount = damage / 2;
        PlayerHeal(player, healAmount);
        printf("%sYou drained %hd health!%s\n", GREEN, healAmount, RESET);
        char action[100];
        sprintf_s(action, sizeof(action), "%s", ability->name);
        UI::UI_DisplayCombatAnimation(action, damage, false);
    }
    else
    {
        EnemyDamage(enemy, damage);
        char action[100];
        sprintf_s(action, sizeof(action), "%s", ability->name);
        UI::UI_DisplayCombatAnimation(action, damage, false);
    }
}

bool AbilityCanUse(Ability* ability)
{
    if (ability == nullptr) return false;
    return ability->cooldownRemaining <= 0;
}

//--------------------
// SHOP FUNCTIONS
//--------------------

Shop* ShopInit()
{
    Shop* shop = (Shop*)malloc(sizeof(Shop));
    if (shop == nullptr)
    {
        printf("ERROR - Failed to allocate memory for shop.\n");
        return nullptr;
    }
    shop->itemCount = 0;
    return shop;
}

void ShopFree(Shop* shop)
{
    if (shop != nullptr)
        free(shop);
}

void ShopGenerateItems(Shop* shop, unsigned short playerLevel)
{
    if (shop == nullptr) return;
    
    shop->itemCount = 0;
    short numItems = RandomShort(5, 8);
    for (unsigned short i = 0; i < numItems; i++)
    {
        if (shop->itemCount >= MAX_INVENTORY) break;
        ItemRarity rarity;
        float roll = RandomFloat(0.0f, 1.0f);
        
        if (playerLevel < 4)
        {
            if (roll < 0.60f)
            {
                rarity = COMMON;
            }
            else if (roll < 0.90f)
            {
                rarity = UNCOMMON;
            }
            else
            {
                rarity = RARE;
            }
        }
        else if (playerLevel < 7)
        {
            if (roll < 0.40f)
            {
                rarity = COMMON;
            }
            else if (roll < 0.70f)
            {
                rarity = UNCOMMON;
            }
            else if (roll < 0.95f)
            {
                rarity = RARE;
            }
            else
            {
                rarity = LEGENDARY;
            }
        }
        else
        {
            if (roll < 0.20f) rarity = COMMON;
            else if (roll < 0.50f) rarity = UNCOMMON;
            else if (roll < 0.85f) rarity = RARE;
            else rarity = LEGENDARY;
        }
        ItemType type = (ItemType)RandomShort(0,2);
        shop->items[shop->itemCount] = ItemGenerateRandom(rarity, type);
        shop->itemCount++;
    }
}

void ShopDisplay(Shop* shop)
{
    if (shop == nullptr){ UI::UI_DisplayErrorMessage("Shop is Null!"); return;}
    UI::UI_PrintHeader("SHOP");
    
    if (shop->itemCount == 0){printf("\nThe SHop  is currently out of stock\n");return;}
    printf("\nWelcome to the shop! We have %hd items for sale.\n\n", shop->itemCount);
    UI::UI_PrintDivider();
    for (short i = 0; i < shop->itemCount; i++)
    {
        ItemData* item = &shop->items[i];
        printf("%hd. ", i+1);
        switch (item->rarity)
        {
        case COMMON:
            {
                printf("%s", RESET);
                break;
            }
        case UNCOMMON:
            {
                printf("%s", GREEN);
                break;
            }
        case RARE:
            {
                printf("%s", BLUE); break;
            }
        case LEGENDARY:
            {
                printf("%s", MAGENTA);break;
            }
        }
        printf("%s%s - %hd gold\n", item->name, RESET, item->cost);
        printf("   Type: %s | Value: %hd\n", ItemGetTypeName(item->type), item->value);
        printf("   %s\n\n", item->description);
    }
    UI::UI_PrintDivider();
}

bool ShopBuyItem(Shop* shop, Player* player, Inventory* inventory, short itemID)
{
    if (shop == nullptr || player == nullptr || inventory == nullptr)
    {
        return false;
    }
    
    ItemData* item = nullptr;
    short itemIndex = -1;
    
    for (short i = 0; i < shop->itemCount; i++)
    {
        if (shop->items[i].itemID == itemID)
        {
            item = &shop->items[i];
            itemIndex = i;
            break;
        }
    }
    
    if (item == nullptr)
    {
        UI::UI_DisplayErrorMessage("Item not found in shop!");
        return false;
    }
    
    if (player->gold < item->cost)
    {
        UI::UI_DisplayErrorMessage("Not enough gold!");
        return false;
    }
    
    if (InventoryIsFull(inventory))
    {
        UI::UI_DisplayErrorMessage("Inventory is full!");
        return false;
    }
    
    player->gold -= item->cost;
    InventoryAddItem(inventory, *item);
    
    for (short i = itemIndex; i < shop->itemCount - 1; i++)
    {
        shop->items[i] = shop->items[i + 1];
    }
    shop->itemCount--;
    
    UI::UI_DisplaySuccessMessage("Purchase successful!");
    return true;
}

bool ShopSellItem(Shop* shop, Player* player, Inventory* inventory, short itemID)
{
    if (shop == nullptr || player == nullptr || inventory == nullptr)
    {
        return false;
    }
    
    ItemData* item = InventoryFindItem(inventory, itemID);
    
    if (item == nullptr)
    {
        UI::UI_DisplayErrorMessage("Item not found in inventory!");
        return false;
    }
    
    int sellPrice = item->cost / 2;
    
    player->gold += sellPrice;
    InventoryRemoveItem(inventory, itemID);
    
    printf("Sold %s for %d gold!\n", item->name, sellPrice);
    UI::UI_DisplaySuccessMessage("Sale complete!");
    
    return true;
}

void ShopMenu(Shop* shop, Player* player, Inventory* inventory)
{
    if (shop == nullptr || player == nullptr || inventory == nullptr)
    {
        return;
    }
    
    bool shopping = true;
    
    while (shopping)
    {
        CLEAR_SCREEN();
        ShopDisplay(shop);
        
        printf("\nYour Gold: %d\n", player->gold);
        printf("\n1. Buy Item\n");
        printf("2. Sell Item\n");
        printf("3. Leave Shop\n");
        UI::UI_PrintDivider();
        
        unsigned short choice = UI::UI_GetMenuInput(1, 3);
        
        switch (choice)
        {
            case 1:
            {
                if (shop->itemCount == 0)
                {
                    UI::UI_DisplayWarningMessage("Nothing to buy!");
                    UI::UI_TimedPause(1000);
                    break;
                }
                
                printf("Enter item number to buy (0 to cancel): ");
                short itemNum;
                scanf_s("%hd", &itemNum);
                while (getchar() != '\n') {}
                
                if (itemNum > 0 && itemNum <= shop->itemCount)
                {
                    short itemID = shop->items[itemNum - 1].itemID;
                    ShopBuyItem(shop, player, inventory, itemID);
                }
                
                UI::UI_PauseScreen();
                break;
            }
            
            case 2:
            {
                if (inventory->itemCount == 0)
                {
                    UI::UI_DisplayWarningMessage("Nothing to sell!");
                    UI::UI_TimedPause(1000);
                    break;
                }
                
                CLEAR_SCREEN();
                InventoryDisplay(inventory);
                
                printf("\nEnter item ID to sell (0 to cancel): ");
                short itemID;
                scanf_s("%hd", &itemID);
                while (getchar() != '\n') {}
                
                if (itemID > 0)
                {
                    ShopSellItem(shop, player, inventory, itemID);
                }
                
                UI::UI_PauseScreen();
                break;
            }
            
            case 3:
            {
                shopping = false;
                UI::UI_DisplayInfoMessage("Thanks for visiting!");
                UI::UI_TimedPause(1000);
                break;
            }
        }
    }
}

//--------------------
// FILE I/O FUNCTIONS
//--------------------

bool FileSaveGame(Player* player, Inventory* inventory, QuestLog* questLog, GameStatistics* stats, Dungeon* dungeon)
{
    FILE* file;
    errno_t err = fopen_s(&file, "savegame.txt", "w");
    
    if (err != 0 || file == nullptr)
    {
        printf("ERROR - Failed to open save file\n");
        return false;
    }
    
    FileWritePlayer(file, player);
    FileWriteInventory(file, inventory);
    FileWriteQuests(file, questLog);
    FileWriteStats(file, stats);
    FileWriteDungeon(file, dungeon);
    
    fclose(file);
    return true;
}

bool FileLoadGame(Player** player, Inventory** inventory, QuestLog** questLog, GameStatistics** stats, Dungeon** dungeon)
{
    FILE* file;
    errno_t err = fopen_s(&file, "savegame.txt", "r");
    
    if (err != 0 || file == nullptr)
    {
        printf("ERROR - Failed to open save file\n");
        return false;
    }
    
    FileReadPlayer(file, player);
    FileReadInventory(file, inventory);
    FileReadQuests(file, questLog);
    FileReadStats(file, stats);
    FileReadDungeon(file, dungeon);
    
    fclose(file);
    return true;
}

void FileWritePlayer(FILE* file, Player* player)
{
    if (file == nullptr || player == nullptr) return;
    
    fprintf_s(file, "PLAYER\n");
    fprintf_s(file, "%s\n", player->name);
    fprintf_s(file, "%hu %hu %hu %hu %hu %hu %d %hu\n",
            player->health, player->maxHealth, player->attack, player->defense,
            player->exp, player->level, player->gold, player->currentRoom);
    fprintf_s(file, "%d %d\n", player->trait, player->difficulty);
    fprintf_s(file, "%f %f\n", player->goldMultiplier, player->expMultiplier);
    fprintf_s(file, "%hu %d\n", player->abilityCount, player->canCharmEnemies);
}

void FileReadPlayer(FILE* file, Player** player)
{
    if (file == nullptr) return;
    
    *player = (Player*)malloc(sizeof(Player));
    if (*player == nullptr) return;
    
    char buffer[256];
    fgets(buffer, 256, file);
    
    fgets((*player)->name, MAX_NAME_LENGTH, file);
    (*player)->name[strcspn((*player)->name, "\n")] = 0;
    
    fgets(buffer, 256, file);
    sscanf_s(buffer, "%hu %hu %hu %hu %hu %hu %d %hu",
           &(*player)->health, &(*player)->maxHealth, &(*player)->attack, &(*player)->defense,
           &(*player)->exp, &(*player)->level, &(*player)->gold, &(*player)->currentRoom);
    
    fgets(buffer, 256, file);
    int trait, difficulty;
    sscanf_s(buffer, "%d %d", &trait, &difficulty);
    (*player)->trait = (PlayerTrait)trait;
    (*player)->difficulty = (DifficultyLevel)difficulty;
    
    fgets(buffer, 256, file);
    sscanf_s(buffer, "%f %f", &(*player)->goldMultiplier, &(*player)->expMultiplier);
    
    fgets(buffer, 256, file);
    int canCharm;
    sscanf_s(buffer, "%hu %d", &(*player)->abilityCount, &canCharm);
    (*player)->canCharmEnemies = (bool)canCharm;
    (*player)->statusEffectCount = 0;
}

void FileWriteInventory(FILE* file, Inventory* inventory)
{
    if (file == nullptr || inventory == nullptr) return;

    fprintf_s(file, "INVENTORY\n");
    fprintf_s(file, "%hd\n", inventory->itemCount);

    InventoryNode* current = inventory->head;
    while (current != nullptr)
    {
        fprintf_s(file, "%hd|%s|%hd|%d|%d|%hd|%hd\n",
            current->item.itemID,
            current->item.name,
            current->item.value,
            (int)current->item.rarity,
            (int)current->item.type,
            current->item.cost,
            current->item.quantity);
        current = current->next;
    }
}

void FileReadInventory(FILE* file, Inventory** inventory)
{
    if (file == nullptr) return;

    *inventory = InventoryCreate();
    if (*inventory == nullptr) return;

    char buffer[256];
    fgets(buffer, 256, file);

    short itemCount = 0;
    fgets(buffer, 256, file);
    sscanf_s(buffer, "%hd", &itemCount);

    for (short i = 0; i < itemCount; i++)
    {
        ItemData item{};
        int rarity = 0, type = 0;

        if (!fgets(buffer, 256, file)) break;

        sscanf_s(buffer, "%hd|%49[^|]|%hd|%d|%d|%hd|%hd",
            &item.itemID,
            item.name, (unsigned)sizeof(item.name),
            &item.value,
            &rarity,
            &type,
            &item.cost,
            &item.quantity);

        item.rarity = (ItemRarity)rarity;
        item.type = (ItemType)type;

        sprintf_s(item.description, sizeof(item.description), "Loaded item");

        InventoryAddItem(*inventory, item);
    }
}

void FileWriteQuests(FILE* file, QuestLog* questLog)
{
    if (file == nullptr || questLog == nullptr) return;
    
    fprintf_s(file, "QUESTS\n");
    fprintf_s(file, "%hd\n", questLog->questCount);
    
    for (short i = 0; i < questLog->questCount; i++)
    {
        Quest* quest = &questLog->quests[i];
        fprintf_s(file, "%hd %d %hd %hd %hd %d\n",
                quest->questID, quest->objectiveType, quest->targetValue,
                quest->currentProgress, quest->rewardGold, quest->completed);
    }
}

void FileReadQuests(FILE* file, QuestLog** questLog)
{
    if (file == nullptr) return;
    
    *questLog = QuestInit();
    if (*questLog == nullptr) return;
    
    char buffer[256];
    fgets(buffer, 256, file);
    
    fgets(buffer, 256, file);
    sscanf_s(buffer, "%hd", &(*questLog)->questCount);
    
    for (short i = 0; i < (*questLog)->questCount; i++)
    {
        Quest* quest = &(*questLog)->quests[i];
        int objType, completed;
        fgets(buffer, 256, file);
        sscanf_s(buffer, "%hd %d %hd %hd %hd %d",
               &quest->questID, &objType, &quest->targetValue,
               &quest->currentProgress, &quest->rewardGold, &completed);
        quest->objectiveType = (QuestObjectiveType)objType;
        quest->completed = (bool)completed;
    }
}

void FileWriteStats(FILE* file, GameStatistics* stats)
{
    if (file == nullptr || stats == nullptr) return;
    
    fprintf_s(file, "STATS\n");
    fprintf_s(file, "%hu %hu %hu %hu %hu %hu %hu %hu\n",
            stats->totalEnemiesDefeated, stats->totalGoldEarned,
            stats->totalDamageDealt, stats->totalDamageTaken,
            stats->itemsCollected, stats->questsCompleted,
            stats->totalPlaytime, stats->deathCount);
}

void FileReadStats(FILE* file, GameStatistics** stats)
{
    if (file == nullptr) return;
    
    *stats = (GameStatistics*)malloc(sizeof(GameStatistics));
    if (*stats == nullptr) return;
    
    char buffer[256];
    fgets(buffer, 256, file);
    
    fgets(buffer, 256, file);
    sscanf_s(buffer, "%hu %hu %hu %hu %hu %hu %hu %hu",
           &(*stats)->totalEnemiesDefeated, &(*stats)->totalGoldEarned,
           &(*stats)->totalDamageDealt, &(*stats)->totalDamageTaken,
           &(*stats)->itemsCollected, &(*stats)->questsCompleted,
           &(*stats)->totalPlaytime, &(*stats)->deathCount);
}

void FileWriteDungeon(FILE* file, Dungeon* dungeon)
{
    if (file == nullptr || dungeon == nullptr) return;
    
    fprintf_s(file, "DUNGEON\n");
    fprintf_s(file, "%hd\n", dungeon->totalRooms);
    
    for (short i = 0; i < dungeon->totalRooms; i++)
    {
        fprintf_s(file, "%hd %d %d %d %d\n",
                dungeon->rooms[i].roomID, dungeon->rooms[i].encounterType,
                dungeon->rooms[i].hasShop, dungeon->rooms[i].hasBoss,
                dungeon->rooms[i].explored);
    }
}

void FileReadDungeon(FILE* file, Dungeon** dungeon)
{
    if (file == nullptr) return;
    
    *dungeon = (Dungeon*)malloc(sizeof(Dungeon));
    if (*dungeon == nullptr) return;
    
    char buffer[256];
    fgets(buffer, 256, file);
    
    fgets(buffer, 256, file);
    sscanf_s(buffer, "%hd", &(*dungeon)->totalRooms);
    
    for (short i = 0; i < (*dungeon)->totalRooms; i++)
    {
        int encounterType, hasShop, hasBoss, explored;
        fgets(buffer, 256, file);
        sscanf_s(buffer, "%hd %d %d %d %d",
               &(*dungeon)->rooms[i].roomID, &encounterType,
               &hasShop, &hasBoss, &explored);
        (*dungeon)->rooms[i].encounterType = (EncounterType)encounterType;
        (*dungeon)->rooms[i].hasShop = (bool)hasShop;
        (*dungeon)->rooms[i].hasBoss = (bool)hasBoss;
        (*dungeon)->rooms[i].explored = (bool)explored;
    }
}

//--------------------
// UTILITY FUNCTIONS
//--------------------

float RandomFloat(float min, float max)
{
    

    if (min > max)
    {
        float temp = min;
        min = max;
        max = temp;
    }

    float r = rand() % 10000; // NOLINT
    r = r / 10000.0f; 

    return min + r * (max - min);
}

short RandomShort(short min, short max)
{
    if (min > max)
    {
        short temp = min;
        min = max;
        max = temp;
    }
    return (short)min + rand() % (max - min + 1);
}

bool RandomChance(float prob)
{
    if (prob <= 0.0f)
        return false;
    if (prob >= 1.0f)
        return true;
    return RandomFloat(0.0f, 1.0f) < prob;
}

short CountExploredRooms(const Dungeon* dungeon)
{
    if (dungeon == nullptr)
    {
        return 0;
    }
    short count = 0;
    for (short i = 0; i < MAX_ROOMS; i++)
    {
        if (dungeon->rooms[i].explored)
            count++;
    }
    return count;
}

