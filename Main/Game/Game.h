#ifndef GAME_H
#define GAME_H

#include <cstdio>

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
#define MAX_ROOMS 35 // NOLINT(modernize-macro-to-enum)
#define MAX_QUESTS 20 // NOLINT(modernize-macro-to-enum)
#define MAX_INVENTORY 50 // NOLINT(modernize-macro-to-enum)
#define MAX_ENEMIES 10 // NOLINT(modernize-macro-to-enum)
#define DUNGEON_ROWS 7 // NOLINT(modernize-macro-to-enum)
#define DUNGEON_COLS 5 // NOLINT(modernize-macro-to-enum)

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

typedef struct StatusEffect //NOLINT
{
    StatusEffectType type;
    unsigned short duration;
    unsigned short damagePerTurn;
    short statModifier;
}StatusEffect;

//
typedef struct Ability
{
    unsigned short abilityId;
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    unsigned short unlockedAtLevel;
    float damageMultiplier; //NOLINT
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
    int gold;// InitStats NOLINT
    unsigned short currentRoom; // InitStats
    PlayerTrait trait; // Character creation NOLINT
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

typedef struct Enemy //NOLINT
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
    ItemRarity lootRarity; //NOLINT
    StatusEffect statusEffect[10];
    short statusEffectCount;
}Enemy;
//Item struct - data
typedef struct ItemData//NOLINT
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

typedef struct Inventory //NOLINT
{
    InventoryNode* head;
    short itemCount;
}Inventory;

typedef struct Room //NOLINT
{
    short roomID;
    char description[MAX_DESCRIPTION_LENGTH];
    EncounterType encounterType; //NOLINT
    short connections[4];
    bool hasShop;
    bool hasBoss;
    bool explored;
}Room;

typedef struct Dungeon //NOLINT
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

typedef struct QuestLog //NOLINT
{
    Quest quests[MAX_QUESTS];
    short questCount;
}QuestLog;

typedef struct Shop
{
    Item items[MAX_INVENTORY];
    short itemCount;
}Shop;

typedef struct GameStatistics
{
    unsigned short totalEnemiesDefeated;
    unsigned short totalGoldEarned;
    unsigned short totalDamageDealt;
    unsigned short totalDamageTaken;
    unsigned short itemsCollected;
    unsigned short questsCompleted;
    unsigned short totalPlaytime;
    unsigned short deathCount;
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
void GameHandleEncounter(GameInstance* game);
void GameInitializeEnemies(GameInstance* game);
void GameInitializeAbilities(GameInstance* game);

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
void PlayerApplyStatusEffects(Player* player, StatusEffect effect);
void PlayerUpdateStatusEffects(Player* player);
void PlayerGainExperience(Player* player, unsigned short exp);
void PlayerGainGold(Player* player, unsigned short gold);

//--------------------
// ENEMY FUNCTIONS
//--------------------

Enemy* EnemyInit(GameInstance* game, short enemyID);
Enemy* EnemyGenerateForLevel(GameInstance* game, unsigned short playerLevel);
void EnemyDisplayStats(Enemy* enemy);
bool EnemyIsAlive(Enemy* enemy);
void EnemyUpdateStatusEffects(Enemy* enemy);
void EnemyApplyStatusEffect(Enemy* enemy, StatusEffect effect);
void EnemyDamage(Enemy* enemy, unsigned short damage);
void EnemyHeal(Enemy* enemy, short heal);

//--------------------
// ITEM FUNCTIONS
//--------------------

ItemData ItemCreate(short itemID, const char* name, ItemType type, ItemRarity rarity, short value, short cost);
void ItemApplyEffect(ItemData* item, Player* player);
const char* ItemGetTypeName(ItemType type);
const char* ItemGetRarityName(ItemRarity rarity);
void ItemDisplay(ItemData* item);
ItemData ItemGenerateTreasure(unsigned short playerLevel);
ItemData ItemGenerateRandom(ItemRarity rarity, ItemType type);


//--------------------
// DUNGEON FUNCTIONS
//--------------------

Dungeon* DungeonInit();
void DungeonFree(Dungeon* dungeon);
void DungeonDisplayRoom(Player* player, Dungeon* dungeon);
void DungeonDisplayActionMenu();
void DungeonMoveToRoom(Player* player, Dungeon* dungeon, Direction direction);
Direction DungeonGetDirectionInput();
const char* DungeonGetDirectionName(Direction dir);
void DungeonDisplayMap(Player* player, Dungeon* dungeon);
short DungeonGetRoomIndex(short row, short col);
void DungeonGenerateRooms(Dungeon* dungeon);
void DungeonGenerateConnections(Dungeon* dungeon);

//--------------------
// COMBAT FUNCTIONS
//--------------------

CombatResult CombatStart(Player* player, Enemy* enemy, GameInstance* game);
void CombatAwardVictory(Player* player, Enemy* enemy, GameInstance* game);
void CombatDisplayMenu(Player* player, Enemy* enemy);
void CombatPlayerAttack(Player* player, Enemy* enemy, GameInstance* game);
void CombatEnemyAttack(Player* player, Enemy* enemy, GameInstance* game);
void CombatUseAbility(Player* player, Enemy* enemy, GameInstance* game);
void CombatUseItem(Player* player, Enemy* enemy, GameInstance* game);
bool CombatAttemptEscape(Player* player);
void CombatUpdateCooldowns(Player* player);
unsigned short CombatCalculateDamage(unsigned short attack, unsigned short defense, float multiplier);

//--------------------
// INVENTORY FUNCTIONS
//--------------------

Inventory* InventoryCreate();
void InventoryFree(Inventory* inventory);
bool InventoryAddItem(Inventory* inventory, ItemData item);
bool InventoryRemoveItem(Inventory* inventory, short itemID);
ItemData* InventoryFindItem(Inventory* inventory, short itemID);
void InventoryDisplay(Inventory* inventory);
void InventoryUseItem(Inventory* inventory, Player* player, short itemID);
bool InventoryIsFull(Inventory* inventory);
unsigned short InventoryGetTotalValue(Inventory* inventory);

//--------------------
// QUEST FUNCTIONS
//--------------------

QuestLog* QuestInit();
void QuestFree(QuestLog* questLog);
void QuestGenerate(QuestLog* questLog, unsigned short playerLevel);
void QuestDisplay(QuestLog* questLog, Player* player);
void QuestCheckCompletion(QuestLog* questLog, Player* player, GameInstance* game);
void QuestUpdateProgress(QuestLog* questLog, QuestObjectiveType type, short amount);
void QuestAwardReward(QuestLog* questLog, Player* player, short questIndex);
bool QuestIsComplete(QuestData* quest);

//--------------------
// ABILITY SYSTEM FUNCTIONS
//--------------------

void AbilityDisplayList(Player* player);
void AbilityCheckUnlocks(Player* player, GameInstance* game);
bool AbilityIsUnlocked(Player* player, int abilityId);
Ability* AbilityGetById(Player* player, int abilityId);
void AbilityUse(Player* player, Enemy* enemy, Ability* ability, GameInstance* game);
bool AbilityCanUse(Ability* ability);

//--------------------
// SHOP FUNCTIONS
//--------------------

Shop* ShopInit();
void ShopFree(Shop* shop);
void ShopGenerateItems(Shop* shop, unsigned short playerLevel);
void ShopDisplay(Shop* shop);
bool ShopBuyItem(Shop* shop, Player* player, Inventory* inventory, short itemID);
bool ShopSellItem(Shop* shop, Player* player, Inventory* inventory, short itemID);
void ShopMenu(Shop* shop, Player* player, Inventory* inventory);

//--------------------
// FILE I/O FUNCTIONS
//--------------------

bool FileSaveGame(Player* player, Inventory* inventory, QuestLog* questLog, GameStatistics* stats, Dungeon* dungeon);
bool FileLoadGame(Player** player, Inventory** inventory, QuestLog** questLog, GameStatistics** stats, Dungeon** dungeon);
void FileWritePlayer(FILE* file, Player* player);
void FileReadPlayer(FILE* file, Player** player);
void FileWriteInventory(FILE* file, Inventory* inventory);
void FileReadInventory(FILE* file, Inventory** inventory);
void FileWriteQuests(FILE* file, QuestLog* questLog);
void FileReadQuests(FILE* file, QuestLog** questLog);
void FileWriteStats(FILE* file, GameStatistics* stats);
void FileReadStats(FILE* file, GameStatistics** stats);
void FileWriteDungeon(FILE* file, Dungeon* dungeon);
void FileReadDungeon(FILE* file, Dungeon** dungeon);

//--------------------
// UTILITY FUNCTIONS
//--------------------

float RandomFloat(float min, float max);
short RandomShort(short min, short max);
bool RandomChance(float prob);
short CountExploredRooms(const Dungeon* dungeon);

#endif