#ifndef GAME_H
#define GAME_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

//--------------------
// COLOR CODES FOR TERMINAL
//--------------------

#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"

//--------------------
// Clearing Screen
//--------------------

#define CLEAR_SCREEN() system("cls")

//--------------------
// CONSTANTS
//--------------------

#define MAX_NAME_LENGTH 50  // NOLINT(modernize-macro-to-enum)
#define MAX_DESCRIPTION_LENGTH 200 // NOLINT(modernize-macro-to-enum)
#define MAX_STRING_LENGTH 256 // NOLINT(modernize-macro-to-enum)
#define MAX_ABILITIES 15 // NOLINT(modernize-macro-to-enum)
#define MAX_ROOMS 20 // NOLINT(modernize-macro-to-enum)
#define MAX_QUESTS 20 // NOLINT(modernize-macro-to-enum)
#define MAX_INVENTORY 50 // NOLINT(modernize-macro-to-enum)
#define MAX_ENEMIES 10 // NOLINT(modernize-macro-to-enum)
#define DUNGEON_ROWS 5 // NOLINT(modernize-macro-to-enum)
#define DUNGEON_COLS 4 // NOLINT(modernize-macro-to-enum)

//--------------------
// PLAYER STARTING STATS
//--------------------

#define STARTING_HEALTH 100  // NOLINT(modernize-macro-to-enum)
#define STARTING_ATTACK 10  // NOLINT(modernize-macro-to-enum)
#define STARTING_DEFENCE 5  // NOLINT(modernize-macro-to-enum)
#define STARTING_GOLD 50  // NOLINT(modernize-macro-to-enum)
#define STARTING_XP 0  // NOLINT(modernize-macro-to-enum)

//--------------------
// LEVEL UP CONSTANTS
//--------------------

#define XP_PER_LEVEL 100 // NOLINT(modernize-macro-to-enum)
#define HP_LEVEL_GAIN 20 // NOLINT(modernize-macro-to-enum)
#define ATTACK_LEVEL_GAIN 5// NOLINT(modernize-macro-to-enum)
#define DEFENCE_LEVEL_GAIN 3// NOLINT(modernize-macro-to-enum)
#define MAX_LEVEL 10// NOLINT(modernize-macro-to-enum)

//--------------------
// ENUMS
//--------------------

typedef enum  // NOLINT(performance-enum-size)
{
    EASY = 0,
    NORMAL = 1,
    HARD = 2,
    INSANE = 3,
    
}DifficultyLevel;

typedef enum
{
    WEAPON = 0,
    ARMOR = 1,
    POTION = 2,
    
}ItemType;

typedef enum
{
    COMMON = 0,
    UNCOMMON = 1,
    RARE = 2,
    LEGENDARY = 3,
    
}ItemRarity;

typedef enum
{
    POISON = 0,
    STUN = 1,
    BLEED = 2,
    FORTIFIED = 3,
    WEAKENED = 4

}StatusEffectType;

typedef enum
{
    EMPTY = 0,
    ENEMY = 1,
    TREASURE = 2,
    QUEST = 3,
    BOSS = 4
    
}EncounterType;

typedef enum
{
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3,
}Direction;

typedef enum
{
    MAIN_MENU = 0,
    CHARACTER_CREATION = 1,
    DIFFICULTY_SELECT = 2,
    NEW_GAME = 3, 
    GAME_LOOP = 4,
    PAUSE_MENU = 5,
    GAME_OVER = 6,
    LOAD_GAME = 7
    
}GameState;

typedef enum
{
    KILL_ENEMIES = 0,
    COLLECT_ITEMS = 1,
    REACH_LEVEL = 2,
    
}QuestObjectiveType;

typedef enum
{
    TRAIT_HEAVY_ARMOUR = 0,
    TRAIT_QUICK_HANDS = 1,
    TRAIT_FORTUNATE = 2,
    TRAIT_SCHOLARLY = 3,
    TRAIT_BEAST_MASTER = 4
    
}PlayerTrait;

typedef enum
{
    COMBAT_VICTORY = 0,
    COMBAT_DEFEAT = 1,
    COMBAT_ESCAPE = 2,
    
}CombatResult;

typedef enum
{
    GAME_CONTINUE = 0,
    GAME_WON = 1,
    GAME_LOST = 2,
}GameStatus;

//--------------------
// STRUCTS
//--------------------

typedef struct StatusEffect
{
    StatusEffectType type;
    int duration;
    int damagePerItem;
}StatusEffect;

//
typedef struct Ability
{
    int abilityId;
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int unlockedAtLevel;
    float damageMultiplier;
    int cooldown;
    int cooldownRemaining;
}Ability;

//Player Struct
typedef struct Player
{
    char name[MAX_NAME_LENGTH]; // Character creation
    unsigned short health; // InitStats
    unsigned short maxHealth;// InitStats
    unsigned short attack;// InitStats
    unsigned short defense;// InitStats
    unsigned short exp;// InitStats
    unsigned short level;// InitStats
    int gold;// InitStats
    unsigned short currentRoom; // InitStats
    PlayerTrait trait; // Character creation
    float goldMultiplier;//PlayerCreate()
    float expMultiplier; //PlayerCreate()
    StatusEffect statusEffect[10]; 
    unsigned short statusEffectCount; //PlayerCreate()
    Ability unlockedAbilities[MAX_ABILITIES];  // NOLINT(clang-diagnostic-padded)
    unsigned short abilityCount; //PlayerCreate()
    bool canCharmEnemies; //PlayerCreate()
    DifficultyLevel difficulty; // Difficulty select NOLINT(clang-diagnostic-padded)
}Player;
//Enemy Struct

typedef struct Enemy
{
    short enemyID;
    char name[MAX_NAME_LENGTH];
    short baseHealth;
    short health;
    short attack;
    short defense;
    short expReward;
    short goldReward;
    short difficulty;
    ItemRarity lootRarity;
    StatusEffect statusEffect[10];
    short statusEffectCount;
}Enemy;
//Item struct - data
typedef struct ItemData
{
    short itemID;
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    ItemRarity rarity;
    ItemType type;
    short value;
    short cost;
    short quantity;
    
}Item;

typedef struct InventoryNode
{
    Item item;
    struct InventoryNode* next;
}InventoryNode;

typedef struct Inventory
{
    InventoryNode* head;
    short itemCount;
}Inventory;

typedef struct Room
{
    short roomID;
    char description[MAX_DESCRIPTION_LENGTH];
    EncounterType encounterType;
    short connections[4];
    bool hasShop;
    bool hasBoss;
    bool explored;
}Room;

typedef struct Dungeon
{
    Room rooms[MAX_ROOMS];
    short totalRooms;
}Dungeon;

typedef struct QuestData  // NOLINT(clang-diagnostic-padded)
{
    short questID;
    char title[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    QuestObjectiveType objectiveType;
    short targetValue;
    short currentProgress;
    short rewardGold;
    bool completed;
}Quest;

typedef struct QuestLog
{
    Quest quests[MAX_QUESTS];
}QuestLog;

typedef struct Shop
{
    Item items[MAX_INVENTORY];
    short itemCount;
}Shop;

typedef struct GameStatistics
{
    short totalEnemiesDefeated;
    short totalGoldEarned;
    short totalDamageDealt;
    short totalDamageTaken;
    short itemsCollected;
    short questsCompleted;
    short totalPlaytime;
    short deathCount;
}GameStats;

struct GameInstance //NOLINT(clang-diagnostic-padded)
{
    GameState currentState;
    Player* player; //NOLINT(clang-diagnostic-padded)
    Dungeon* dungeon;
    Inventory* inventory;
    QuestLog* questLog;
    Shop* shop;
    GameStats* stats;
    Enemy enemyList[MAX_ENEMIES];
    short enemyCount;
    Ability abilityList[MAX_ABILITIES];  // NOLINT(clang-diagnostic-padded)
    short abilityCount;
    bool isRunning;
};

//--------------------
// GAME FUNCTION PROTOTYPES
//--------------------

GameInstance* GameInit();
void GameFree(GameInstance* game);
void GameRun(GameInstance* game);
GameState GameShowMainMenu();
void GameHandleCharacterCreation(GameInstance* game);
void GameHandleGameDifficultySelection(GameInstance* game);
void GameHandleGameLoop(GameInstance* game);
void GameHandlePauseMenu(GameInstance* game);
GameStatus GameCheckGameStatus(GameInstance* game);

//--------------------
// PLAYER FUNCTIONS
//--------------------
Player* PlayerCreate();
void PlayerFree(Player* player);
void PlayerInitStats(Player* player);
void PlayerDisplayStats(Player* player);
void PlayerDisplayStatusBar(Player* player);
void PlayerLevelup(Player* player);
void PlayerDamage(Player* player, unsigned short damage);
void PlayerHeal(Player* player, unsigned short heal);
void PlayerApplyTrait(Player* player, PlayerTrait trait);
void PlayerSelectTrait(Player* player);
const char* PlayerGetTraitName(PlayerTrait trait);

//--------------------
// DUNGEON FUNCTIONS
//--------------------

Dungeon* DungeonInit();
void DungeonFree(Dungeon* dungeon);
void DungeonDisplayRoom(Player* player, Dungeon* dungeon);
void DungeonDisplayActionMenu();
void DungeonMoveToRoom(Player* player, Dungeon* dungeon, short direction);
Direction DungeonGetDirectionInput();
char* DungeonGetDirectionName(Direction dir);
void DungeonDisplayMap(Player* player, Dungeon* dungeon);
EncounterType DungeonGenerateEncounter(unsigned short playerLevel);














#endif