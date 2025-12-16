#include "Game.h"
#include "../UI/UI.h"
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
                game->dungeon = DungeonInit();
                //game->questLog = QuestInit();
                //game->inventory = CreateInventory();
                game->player->currentRoom = 0;
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
    UI::UI_PrintDivider();
    
    // Display Dungeon room here.
    PlayerDisplayStatusBar(game->player);
    
    //Check for boss room here.
    /*dungeon_display_action_menu()
    choice = UI::GetCharInput()
    
    SWITCH choice
        CASE '1':
            direction = UI::GetDirectionInput()
            dungeon_move_to_room(game.player, direction, game.dungeon)
            game_handle_encounter(game)
        
        CASE '2':
            CLEAR_SCREEN()
            player_display_stats(game.player)
            UI::PauseScreen()
        
        CASE '3':
            CLEAR_SCREEN()
            inventory_display(game.inventory)
            UI::PauseScreen()
        
        CASE '4':
            CLEAR_SCREEN()
            quest_display_log(game.questLog, game.player)
            UI::PauseScreen()
        
        CASE '5':
            CLEAR_SCREEN()
            ability_display_list(game.player)
            UI::PauseScreen()
        
        CASE '6':
            CLEAR_SCREEN()
            dungeon_display_map(game.player, game.dungeon)
            UI::PauseScreen()
        
        CASE '7':
            IF file_save_game(game.player, game.inventory, game.questLog, game.stats)
                UI::DisplaySuccessMessage("Game saved!")
            ELSE
                UI::DisplayErrorMessage("Save failed!")
            ENDIF
            UI::PauseScreen()
        
        CASE '8':
            game.currentState = PAUSE_MENU
    END SWITCH
    
    IF game.player.experience >= (game.player.level * XP_PER_LEVEL)
        player_levelup(game.player)
        ability_check_unlocks(game.player, game)
    ENDIF
    
    quest_check_completion(game.questLog, game.player)*/
    
}

void GameHandlePauseMenu(GameInstance* game)
{
    printf("Pause Menu - todo\n");
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
    
    printf("%s[LVL: %hu]\n", player->name, player->level);
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
    for (short i = 0; i < MAX_ROOMS - 1; i++)
    {
        errno_t err = strcpy_s(dungeon->rooms[i].description, sizeof(dungeon->rooms[i].description), roomDescriptions[i]);
        if (err!=0)
        {
            printf("Failed to copy room description data.\n");
        }
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
        }
        float roll = RandomFloat(0.0f, 1.0f);
        if (roll < 0.40f)
        {
            dungeon->rooms[i].encounterType = ENEMY;
        }
        else if (roll < 0.60f)
        {
            dungeon->rooms[i].encounterType = TREASURE;
        }
        else if (roll < 0.75f)
        {
            dungeon->rooms[i].encounterType = QUEST;
        }
        else if (roll < 0.85f)
        {
            dungeon->rooms[i].encounterType = EMPTY;
            dungeon->rooms[i].hasShop = true;
        }else
        {
            dungeon->rooms[i].encounterType = EMPTY;
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
