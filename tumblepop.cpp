#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>

using namespace sf;
using namespace std;

// Screen dimensions
int screen_x = 1150;
int screen_y = 896;

// Enemy types
#define TYPE_GHOST 0
#define TYPE_SKELETON 1
#define TYPE_INVISIBLE 2
#define TYPE_CHELNOV 3

// Power-up types
#define POWERUP_SPEED 0
#define POWERUP_RANGE 1
#define POWERUP_POWER 2
#define POWERUP_LIFE 3

// Player global variables
float playerX, playerY;
float playerVelocityX, playerVelocityY;
bool playerOnGround, playerFacingRight, playerIsAlive, playerIsJumping, playerIsVacuuming, playerTookDamage;
int playerLives, playerHeight, playerWidth, playerCharacterType, playerTotalPoints, playerComboStreak;
int playerKillStreak, playerAerialDefeats;
float playerSpeed, playerBaseSpeed, playerJumpStrength, playerLevelStartTime, playerLastKillTime;
Sprite playerSprite;
Texture playerTexture;

// Enemy arrays (using arrays for multiple enemies)
const int MAX_ENEMIES = 50;
float enemyX[MAX_ENEMIES], enemyY[MAX_ENEMIES], enemyVelocityX[MAX_ENEMIES], enemyVelocityY[MAX_ENEMIES], enemyOriginalY[MAX_ENEMIES];
float enemyShootTimer[MAX_ENEMIES], enemyPauseTimer[MAX_ENEMIES], enemyPauseDuration[MAX_ENEMIES];
float enemyInvisTimer[MAX_ENEMIES], enemyInvisDuration[MAX_ENEMIES];
float enemyWindupTimer[MAX_ENEMIES];
bool enemyActive[MAX_ENEMIES], enemyFacingRight[MAX_ENEMIES], enemyPaused[MAX_ENEMIES];
bool enemyVisible[MAX_ENEMIES], enemyCanBeCaptured[MAX_ENEMIES];
bool enemyInWindup[MAX_ENEMIES], enemyIsBeingSucked[MAX_ENEMIES];
int enemyType[MAX_ENEMIES], enemyHeight[MAX_ENEMIES], enemyWidth[MAX_ENEMIES];
Sprite enemySprite[MAX_ENEMIES];
Texture enemyTexture[MAX_ENEMIES];

// Bullet arrays
const int MAX_BULLETS = 100;
float bulletX[MAX_BULLETS], bulletY[MAX_BULLETS], bulletVelocityX[MAX_BULLETS], bulletVelocityY[MAX_BULLETS];
bool bulletActive[MAX_BULLETS];
int bulletDirection[MAX_BULLETS], bulletWidth[MAX_BULLETS], bulletHeight[MAX_BULLETS];
Sprite bulletSprite[MAX_BULLETS];
Texture bulletTexture[MAX_BULLETS];

// LevelGrid global variables
char **levelGrid;
int levelHeight, levelWidth, levelCellSize;
Texture levelBlockTexture, levelBackgroundTexture, levelPlatformTexture;
Sprite levelBlockSprite, levelBackgroundSprite, levelPlatformSprite;

// Physics global variables
float physicsGravity, physicsTerminalVelocity, physicsFriction, physicsBounceFactor;

// GameUI global variables
Font uiFont;
Text uiPlayerInfo, uiLevelInfo, uiScoreInfo, uiComboInfo;

// Vacuum global variables
int *vacuumCapturedEnemyTypes;
int vacuumMaxCapacity, vacuumCurrentCount;
bool vacuumIsFull, vacuumIsActive;
float vacuumRange, vacuumBaseRange, vacuumSuctionForce, vacuumBaseSuctionForce, vacuumCaptureDistance, vacuumAngle;
Texture vacuumTexture;
Sprite vacuumSprite;

// ThrownEnemy arrays
const int MAX_THROWN_ENEMIES = 20;
int thrownEnemyType[MAX_THROWN_ENEMIES], thrownEnemyWidth[MAX_THROWN_ENEMIES], thrownEnemyHeight[MAX_THROWN_ENEMIES];
float thrownEnemyX[MAX_THROWN_ENEMIES], thrownEnemyY[MAX_THROWN_ENEMIES];
float thrownEnemyVelocityX[MAX_THROWN_ENEMIES], thrownEnemyVelocityY[MAX_THROWN_ENEMIES];
bool thrownEnemyActive[MAX_THROWN_ENEMIES], thrownEnemyIsRolling[MAX_THROWN_ENEMIES];
Sprite thrownEnemySprite[MAX_THROWN_ENEMIES];
Texture thrownEnemyTexture[MAX_THROWN_ENEMIES];

// ThrownEnemyCluster global variables
int *thrownClusterEnemyTypes, thrownClusterEnemyCount, thrownClusterMaxEnemies;
float thrownClusterX, thrownClusterY, thrownClusterVelocityX, thrownClusterVelocityY, thrownClusterRadius;
float thrownClusterBounceTimer, thrownClusterRotationAngle, thrownClusterRotationSpeed;
bool thrownClusterActive, thrownClusterIsBouncing;
Texture thrownClusterThrowTextures[4];

// PowerUp arrays
const int MAX_POWERUPS = 10;
float powerUpX[MAX_POWERUPS], powerUpY[MAX_POWERUPS], powerUpFloatTimer[MAX_POWERUPS];
float powerUpDuration[MAX_POWERUPS], powerUpTimer[MAX_POWERUPS];
int powerUpType[MAX_POWERUPS], powerUpWidth[MAX_POWERUPS], powerUpHeight[MAX_POWERUPS];
bool powerUpActive[MAX_POWERUPS];
Sprite powerUpSprite[MAX_POWERUPS];
Texture powerUpTexture[MAX_POWERUPS];

// LevelState global variables
int levelStateCurrentLevel, levelStateLevel1EnemiesKilled, levelStateLevel1TotalEnemies;
bool levelStateLevel1Complete, levelStateLevel2Complete;
float levelStateLevelTime;

// ============================================================================
// BOSS LEVEL SYSTEM
// ============================================================================

// Boss Level Configuration global variables
int bossLevelRows;
int bossLevelCols;
int bossLevelCellSize;
bool bossLevelIsActive;

// Boss State Variables
float bossX = 575.0f;
float bossY = 150.0f;
int bossWidth = 200;
int bossHeight = 180;
int bossHealth = 10;
int bossMaxHealth = 10;
bool bossIsAngry = false;

// Player HP System
int playerHP = 3;
int playerMaxHP = 3;
float enemyTouchCooldown = 0.0f;
bool bossDefeated = false;
float bossMoveSpeed = 30.0f;
bool bossMovingUp = true;

// Boss Visual
Texture bossTextureNormal, bossTextureAngry;
Sprite bossSprite;

// Boss Spawning System (Balanced Wave-Based)
Clock bossSpawnClock;
Clock minionWaveClock;
float bossSpawnInterval = 3.0f;  // Legacy, will be replaced
int minionsPerBatch = 3;  // Legacy, will be replaced

// Balanced Minion Spawning
int maxMinionLimit = 6;
int minionsPerWave = 2;
float minionWaveInterval = 5.0f;
int currentAliveMinions = 0;
int bossPhase = 1;

// Minion System (Dynamic Arrays)
float* minionX = nullptr;
float* minionY = nullptr;
float* minionVelX = nullptr;
float* minionVelY = nullptr;
int* minionHealth = nullptr;
bool* minionActive = nullptr;
bool* minionFollowMode = nullptr;
bool* minionCaptured = nullptr;
bool* minionIsRolling = nullptr;
int minionCount = 0;

// Minion Visual
Texture minionTextureNormal, minionTextureAngry;
int minionWidth = 48;
int minionHeight = 48;

// Tentacle System (Dynamic Arrays)
float* tentacleX = nullptr;
float* tentacleY = nullptr;
float* tentacleDuration = nullptr;
float* tentacleTimer = nullptr;
bool* tentacleActive = nullptr;
int tentacleCount = 0;

// Tentacle Visual
Texture tentacleTexture;
int tentacleWidth = 80;
int tentacleHeight = 120;

// Tentacle Spawning (Balanced)
Clock tentacleSpawnClock;
float tentacleSpawnInterval = 4.5f;
float tentacleLifetime = 4.0f;
int maxTentacleLimit = 3;
bool tentacleWarning = false;
float tentacleWarningTime = 0.5f;

// Water System (Angry State)
bool waterRising = false;
float waterLevel = 896.0f;
float waterTargetLevel = 420.0f;
float waterRiseSpeed = 50.0f;
RectangleShape waterRect;

// Menu System - GameState constants (using #define instead of enum)
#define MAIN_MENU 0
#define CHARACTER_SELECT 1
#define PLAYING_LEVEL1 2
#define PLAYING_LEVEL2 3
#define PLAYING_BOSS_LEVEL 4
#define LEADERBOARD 5
#define SETTINGS 6
#define GAME_OVER 7

// Character Type constants (using #define instead of enum)
#define YELLOW_TUMBLE 0
#define GREEN_TUMBLE 1

// MenuOption arrays
const int MAX_MENU_OPTIONS = 10;
string menuOptionText[MAX_MENU_OPTIONS];
Vector2f menuOptionPosition[MAX_MENU_OPTIONS];
bool menuOptionIsSelected[MAX_MENU_OPTIONS];

// Character arrays
const int MAX_CHARACTERS = 5;
string characterName[MAX_CHARACTERS];
string characterAbilityName[MAX_CHARACTERS];
string characterAbilityDesc[MAX_CHARACTERS];
int characterSpeedStars[MAX_CHARACTERS];
int characterPowerStars[MAX_CHARACTERS];
Texture characterTexture[MAX_CHARACTERS];
Sprite characterSprite[MAX_CHARACTERS];
bool characterIsSelected[MAX_CHARACTERS];
int characterType[MAX_CHARACTERS];

const int MENU_OPTION_COUNT = 5;
const int CHARACTER_COUNT = 2;

// Function forward declarations
void cleanupVacuum();
void cleanupThrownEnemyCluster();
void cleanupLevelGrid();
int getEnemyCapturePoints(int enemyType);
int getEnemyDefeatPoints(int enemyType);
int getMultiKillBonus(int killCount);
void addPoints(int points, const string &reason);
void checkMultiKill();
void calculateLevelBonus();
void initVacuum(int capacity);
void initThrownEnemyCluster(int maxEnemies);
void initUI();
void orientSpriteFacingRight(Sprite &s, int width);
void orientSpriteFacingLeft(Sprite &s, int width);
void initPlayer(int characterChoice);
void initEnemy(int enemyIndex, int type, float x, float y);
void initBullet(int bulletIndex);
void initThrownEnemy(int thrownIndex, int type, float x, float y, float vx, float vy);
void initPowerUp(int powerupIndex, int type, float x, float y);
void initLevelGrid(int cellSize = 64);
void createLevel1();
void createLevel2();
void displayLevel(RenderWindow &window);
bool tileSolid(int gridY, int gridX);
bool clearLineBetween(int row, int colA, int colB);
void updateEnemyPhysics(int enemyIndex);
void updatePlayerPhysics(Clock &levelClock);
void updateEnemies(int enemyCount, int maxBullets, float deltaTime);
void updateBullets(int maxBullets, float deltaTime);
bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2);
bool checkCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2);
void handlePlayerInput();
void applyVacuumEffect(int enemyCount, float deltaTime);
void applyVacuumEffectToMinions(int minionCount, float deltaTime);
void throwSingleEnemy(int maxThrown, int direction = -1);
void throwAllEnemies();
void updateThrownEnemies(int maxThrown, int enemyCount, float deltaTime);
void updateThrownEnemyCluster(int enemyCount, float deltaTime);
void updatePowerUps(int powerupCount, float deltaTime);
void drawThrownEnemyCluster(RenderWindow &window);
void drawCapturedEnemies(RenderWindow &window);

// Menu System Functions
void displayMainMenu(RenderWindow& window, Texture& bgTexture, Font& font, int selectedIndex);
void handleMenuInput(int& selectedIndex, int optionCount, 
                     int& currentState, RenderWindow& window, bool& keyPressed);
void drawMenuOption(RenderWindow& window, string text, Vector2f position, 
                    bool isSelected, Font& font, float scale = 1.0f);
void displayCharacterSelect(RenderWindow& window, int selectedIndex, Texture& bgTexture, Font& font, float time);
void drawCharacterBox(RenderWindow& window, int characterIndex, 
                      Vector2f position, bool isSelected, Font& font, float time);
void handleCharacterSelectInput(int& selectedIndex, int& currentState, 
                                bool& keyPressed, int& playerCharacter);
int getSelectedCharacter(int index);
void initializeCharacters();
void initializeMenuOptions(Font& font);
void displayGameOver(RenderWindow& window, Texture& bgTexture, Font& font, 
                     int selectedIndex, int finalScore);
void handleGameOverInput(int& selectedIndex, int& currentState, 
                         RenderWindow& window, bool& keyPressed);

// Boss Level Functions
void initBossLevel(char**& level);
void generateBossLevel(char** level, int rows, int cols);
void cleanupBossLevel(char** level, int rows);
void initBoss();
void updateBoss(float deltaTime);
void updateBossPhase(int bossHealth, bool& bossIsAngry);
void drawBoss(RenderWindow& window, Font& font);
void spawnMinionWave();
void updateBossSpawning(float deltaTime, int rows, int cols, int cellSize);
void updateMinions(float deltaTime, float playerX, float playerY, 
                   char** level, int rows, int cols, int cellSize);
void drawMinions(RenderWindow& window);
void spawnTentacle(int rows, int cols, int cellSize);
void updateTentacles(float deltaTime, int rows, int cols, int cellSize);
void drawTentacles(RenderWindow& window);
void checkMinionTentacleCollision();
void checkBossDamage();
void updateWater(float deltaTime);
void drawWater(RenderWindow& window, int screenWidth, int screenHeight);
void cleanupBossData();

int main()
{
    // Setup random number generator
    srand((unsigned)time(0));

    // Create game window
    RenderWindow window(VideoMode(screen_x, screen_y), "Tumble-POP");
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Load menu assets
    Texture menuBgTex;
    if (!menuBgTex.loadFromFile("Data/menu_bg.png")) {
        cerr << "Warning: Could not load Data/menu_bg.png - using default background" << std::endl;
        Image defaultBg;
        defaultBg.create(screen_x, screen_y, Color(30, 30, 50));
        menuBgTex.loadFromImage(defaultBg);
    }
    menuBgTex.setSmooth(true);

    Font menuFont;
    // Try to load font in order of preference
    if (!menuFont.loadFromFile("Data/font.ttf")) {
        if (!menuFont.loadFromFile("Assets/arial.ttf")) {
            if (!menuFont.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
                cerr << "Warning: Could not load font. Text may not display correctly." << std::endl;
            }
        }
    }

    // Load character textures for menu
    Texture yellowCharTex, greenCharTex;
    if (!yellowCharTex.loadFromFile("Assets/yellow.png")) {
        cerr << "Warning: Could not load Assets/yellow.png" << std::endl;
        Image yellowImg;
        yellowImg.create(250, 300, Color(255, 255, 0));
        yellowCharTex.loadFromImage(yellowImg);
    }
    if (!greenCharTex.loadFromFile("Assets/green.png")) {
        cerr << "Warning: Could not load Assets/green.png" << std::endl;
        Image greenImg;
        greenImg.create(250, 300, Color(0, 255, 0));
        greenCharTex.loadFromImage(greenImg);
    }

    // Load menu music (optional)
    Music menuMusic;
    if (menuMusic.openFromFile("Data/menu_music.ogg")) {
        menuMusic.setLoop(true);
        menuMusic.play();
    }

    // Initialize menu system
    int currentState = MAIN_MENU;
    int menuSelection = 0;
    int charSelection = 0;
    int gameOverSelection = 0; // For game over menu (0 = Back to Menu, 1 = Quit)
    int playerCharacterType = YELLOW_TUMBLE;
    int characterChoice = 1; // Will be set based on menu selection
    int finalScore = 0; // Store final score for game over screen
    bool keyPressed = false;
    Clock menuClock;
    float menuTime = 0.0f;

    // Initialize menu options and characters
    initializeMenuOptions(menuFont);
    initializeCharacters();
    characterTexture[0] = yellowCharTex;
    characterSprite[0].setTexture(characterTexture[0]);
    // Scale character sprite to fit in box (250x300 target size)
    float yellowScaleX = 250.0f / characterTexture[0].getSize().x;
    float yellowScaleY = 300.0f / characterTexture[0].getSize().y;
    characterSprite[0].setScale(yellowScaleX, yellowScaleY);
    
    characterTexture[1] = greenCharTex;
    characterSprite[1].setTexture(characterTexture[1]);
    float greenScaleX = 250.0f / characterTexture[1].getSize().x;
    float greenScaleY = 300.0f / characterTexture[1].getSize().y;
    characterSprite[1].setScale(greenScaleX, greenScaleY);

    // Game objects (using global variables - no structs needed)
    bool gameInitialized = false;
    bool bossLevelInitialized = false;
    
    // Boss level variables
    char** bossLevelGrid = nullptr;
    bool bossLevelComplete = false;

    // Game arrays (using global arrays)
    const int maxEnemies = 40;
    const int maxBullets = 20;
    const int maxThrownEnemies = 10;
    const int maxPowerUps = 10;
    int enemyCount = 0;
    int powerupCount = 0;
    int enemyHeight[4] = {60, 64, 64, 64};

    // Game timing
    Clock gameClock;
    Clock levelClock;
    bool levelCompleted = false;
    bool gameCompleted = false;

    // Background music for game
    Music backgroundMusic;
    bool gameMusicPlaying = false;

    // Main game loop
    while (window.isOpen())
    {
        menuTime = menuClock.getElapsedTime().asSeconds();
        float deltaTime = gameClock.restart().asSeconds();
        Event event;

        // Handle events
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::Escape)
                {
                    if (currentState == PLAYING_LEVEL1 || currentState == PLAYING_LEVEL2)
                    {
                        // Return to main menu from game
                        currentState = MAIN_MENU;
                        gameInitialized = false;
                        if (gameMusicPlaying)
                        {
                            backgroundMusic.stop();
                            gameMusicPlaying = false;
                        }
                        if (menuMusic.getStatus() != Music::Playing)
                        {
                            menuMusic.play();
                        }
                    }
                    else
                    {
                        window.close();
                    }
                }
                else if ((currentState == PLAYING_LEVEL1 || currentState == PLAYING_LEVEL2) && !gameCompleted)
                {
                    if (event.key.code == Keyboard::Z)
                    {
                        throwSingleEnemy(maxThrownEnemies);
                    }
                    else if (event.key.code == Keyboard::X)
                    {
                        throwAllEnemies();
                    }
                }
                else if (currentState == PLAYING_BOSS_LEVEL)
                {
                    if (event.key.code == Keyboard::Z)
                    {
                        throwSingleEnemy(maxThrownEnemies);
                    }
                    else if (event.key.code == Keyboard::X)
                    {
                        throwAllEnemies();
                    }
                }
            }
        }

        // Handle menu input
        switch (currentState)
        {
            case MAIN_MENU:
                handleMenuInput(menuSelection, MENU_OPTION_COUNT, currentState, 
                               window, keyPressed);
                break;

            case CHARACTER_SELECT:
                handleCharacterSelectInput(charSelection, currentState, keyPressed, 
                                          playerCharacterType);
                // Convert CharacterType to characterChoice when selected
                if (currentState == PLAYING_LEVEL1)
                {
                    characterChoice = (playerCharacterType == YELLOW_TUMBLE) ? 1 : 2;
                }
                break;

            case GAME_OVER:
                handleGameOverInput(gameOverSelection, currentState, window, keyPressed);
                break;

            default:
                break;
        }

        // Initialize game when entering PLAYING_LEVEL1
        if (currentState == PLAYING_LEVEL1 && !gameInitialized)
        {
            // Stop menu music
            menuMusic.stop();
            
            // Initialize game objects
            physicsGravity = 1.0f;
            physicsTerminalVelocity = 20.0f;
            physicsFriction = 0.35f;
            physicsBounceFactor = 0.7f;

            levelStateCurrentLevel = 1;
            levelStateLevel1EnemiesKilled = 0;
            levelStateLevel1TotalEnemies = 12;
            levelStateLevel1Complete = false;
            levelStateLevel2Complete = false;
            levelStateLevelTime = 0;
            
            // Set HP for level 1
            playerMaxHP = 3;
            playerHP = 3;
            enemyTouchCooldown = 0.0f;

            playerX = 64;
            playerY = 704;
            initPlayer(characterChoice);

            initLevelGrid(64);
            createLevel1();
            initUI();

            initVacuum(3);
            initThrownEnemyCluster(3);

            // Initialize arrays
            for (int e = 0; e < maxEnemies; e++)
            {
                enemyActive[e] = false;
            }
            for (int i = 0; i < maxBullets; i++)
            {
                initBullet(i);
            }
            for (int i = 0; i < maxThrownEnemies; i++)
            {
                thrownEnemyActive[i] = false;
            }
            for (int i = 0; i < maxPowerUps; i++)
            {
                powerUpActive[i] = false;
            }

            // Create level 1 enemies
            enemyCount = 0;
            initEnemy(enemyCount++, TYPE_GHOST, 4 * levelCellSize + 10, 3 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 8 * levelCellSize + 10, 6 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 2 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 9 * levelCellSize + 10, 8 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 5 * levelCellSize + 10, 3 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 12 * levelCellSize + 10, 3 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 3 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_GHOST]);
            initEnemy(enemyCount++, TYPE_GHOST, 14 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_GHOST]);

            initEnemy(enemyCount++, TYPE_SKELETON, 2 * levelCellSize + 10, 7 * levelCellSize - enemyHeight[TYPE_SKELETON]);
            initEnemy(enemyCount++, TYPE_SKELETON, 15 * levelCellSize + 10, 7 * levelCellSize - enemyHeight[TYPE_SKELETON]);
            initEnemy(enemyCount++, TYPE_SKELETON, 8 * levelCellSize + 10, 3 * levelCellSize - enemyHeight[TYPE_SKELETON]);
            initEnemy(enemyCount++, TYPE_SKELETON, 9 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_SKELETON]);

            // Create power-ups
            powerupCount = 2;
            initPowerUp(0, POWERUP_SPEED, 6 * levelCellSize, 2 * levelCellSize);
            initPowerUp(1, POWERUP_LIFE, 10 * levelCellSize, 9 * levelCellSize);

            // Setup background music
            if (backgroundMusic.openFromFile("Assets/mus.ogg"))
            {
                backgroundMusic.setVolume(20);
                backgroundMusic.play();
                backgroundMusic.setLoop(true);
                gameMusicPlaying = true;
            }

            playerLevelStartTime = levelClock.getElapsedTime().asSeconds();
            levelCompleted = false;
            gameCompleted = false;
            gameInitialized = true;

            std::cout << "\n=== GAME STARTED ===" << std::endl;
            std::cout << "Character: " << (characterChoice == 1 ? "Yellow (Fast)" : "Green (Strong)") << std::endl;
        }

        // Initialize boss level when entering PLAYING_BOSS_LEVEL
        if (currentState == PLAYING_BOSS_LEVEL && !bossLevelInitialized)
        {
            // Stop menu music
            menuMusic.stop();
            
            // Initialize player if not already initialized (when coming from menu)
            if (!gameInitialized)
            {
                // Initialize game objects
                physicsGravity = 1.0f;
                physicsTerminalVelocity = 20.0f;
                physicsFriction = 0.35f;
                physicsBounceFactor = 0.7f;
                
                // Use default character (Yellow) if not selected
                if (characterChoice == 0)
                {
                    characterChoice = 1;
                }
                
                initPlayer(characterChoice);
                initVacuum(5);
                initUI();
                gameInitialized = true;
            }
            
            // Stop game music
            if (gameMusicPlaying)
            {
                backgroundMusic.stop();
                gameMusicPlaying = false;
            }

            // Initialize level grid textures for boss level (use level 2 tile style - boss_block.png)
            // Load boss level block texture (level 2 style)
            if (!levelBlockTexture.loadFromFile("Assets/boss_block.png"))
            {
                // Fallback to block1.png if boss_block.png doesn't exist
                if (!levelBlockTexture.loadFromFile("Assets/block1.png"))
                {
                    std::cout << "Error loading boss block texture!" << std::endl;
                }
            }
            levelBlockSprite.setTexture(levelBlockTexture);
            
            if (!levelPlatformTexture.loadFromFile("Assets/platform.png"))
            {
                levelPlatformTexture = levelBlockTexture;
            }
            levelPlatformSprite.setTexture(levelPlatformTexture);
            
            if (!levelBackgroundTexture.loadFromFile("Assets/boss_bg.png"))
            {
                // Fallback to regular bg.png if boss_bg.png doesn't exist
                if (!levelBackgroundTexture.loadFromFile("Assets/bg.png"))
                {
                    std::cout << "Error loading boss background texture!" << std::endl;
                }
            }
            levelBackgroundSprite.setTexture(levelBackgroundTexture);
            
            // Initialize boss level
            initBossLevel(bossLevelGrid);
            generateBossLevel(bossLevelGrid, bossLevelRows, bossLevelCols);
            initBoss();
            
            // Reset player position
            playerX = 64;
            playerY = bossLevelRows * bossLevelCellSize - 200;
            playerLives = 3;
            playerTookDamage = false;
            playerComboStreak = 0;
            
            // Set HP for boss level (use level 2 HP)
            playerMaxHP = 5;
            playerHP = 5;
            enemyTouchCooldown = 0.0f;
            
            // Initialize level grid for rendering (reuse existing level structure)
            levelCellSize = bossLevelCellSize;
            levelHeight = bossLevelRows;
            levelWidth = bossLevelCols;
            
            bossSpawnClock.restart();
            tentacleSpawnClock.restart();
            minionWaveClock.restart();
            bossLevelInitialized = true;
            bossLevelComplete = false;
            
            std::cout << "\n=== BOSS LEVEL ===" << std::endl;
            std::cout << "Defeat the Octopus Boss!" << std::endl;
            std::cout << "Suck minions and throw them at the boss head!" << std::endl;
        }

        // Reset key pressed flag
        if (!Keyboard::isKeyPressed(Keyboard::Up) && 
            !Keyboard::isKeyPressed(Keyboard::Down) &&
            !Keyboard::isKeyPressed(Keyboard::W) &&
            !Keyboard::isKeyPressed(Keyboard::S) &&
            !Keyboard::isKeyPressed(Keyboard::Left) &&
            !Keyboard::isKeyPressed(Keyboard::Right) &&
            !Keyboard::isKeyPressed(Keyboard::A) &&
            !Keyboard::isKeyPressed(Keyboard::D) &&
            !Keyboard::isKeyPressed(Keyboard::Enter) &&
            !Keyboard::isKeyPressed(Keyboard::Space) &&
            !Keyboard::isKeyPressed(Keyboard::Escape)) {
            keyPressed = false;
        }

        // Clear window
        window.clear();

        // Render based on current state
        if (currentState == MAIN_MENU)
        {
            displayMainMenu(window, menuBgTex, menuFont, menuSelection);
            window.display();
        }
        else if (currentState == CHARACTER_SELECT)
        {
            displayCharacterSelect(window, charSelection, menuBgTex, menuFont, menuTime);
            window.display();
        }
        else if (currentState == GAME_OVER)
        {
            displayGameOver(window, menuBgTex, menuFont, gameOverSelection, finalScore);
            window.display();
        }
        else if (currentState == PLAYING_LEVEL1 || currentState == PLAYING_LEVEL2)
        {
            // Game logic
            int activeCount = 0;
            if (!gameCompleted)
        {
            // Update game state
            handlePlayerInput();
            updatePlayerPhysics(levelClock);
            updateEnemies(enemyCount, maxBullets, deltaTime);
            updateBullets(maxBullets, deltaTime);
            updateThrownEnemies(maxThrownEnemies, enemyCount, deltaTime);
            updateThrownEnemyCluster(enemyCount, deltaTime);
            updatePowerUps(powerupCount, deltaTime);

            // Update kill streak timer
            playerLastKillTime += deltaTime;
            if (playerLastKillTime > 2.0f)
            {
                checkMultiKill();
                playerLastKillTime = 0;
            }

            // Handle vacuum effects
            if (vacuumIsActive)
            {
                applyVacuumEffect(enemyCount, deltaTime);

                // Position vacuum sprite
                float vacX = playerX + playerWidth / 2;
                float vacY = playerY + playerHeight / 2;
                float angleRad = vacuumAngle * 3.14159f / 180.0f;
                float offsetX = cos(angleRad) * 40.0f;
                float offsetY = sin(angleRad) * 40.0f;

                vacuumSprite.setPosition(vacX + offsetX, vacY + offsetY);
                vacuumSprite.setRotation(vacuumAngle + 90);
            }
            else
            {
                // Reset suction state for all enemies
                for (int i = 0; i < enemyCount; i++)
                {
                    enemyIsBeingSucked[i] = false;
                }
            }

            // Update enemy touch cooldown
            if (enemyTouchCooldown > 0.0f)
            {
                enemyTouchCooldown -= deltaTime;
            }

            // Check for player-enemy collisions
            for (int i = 0; i < enemyCount; i++)
            {
                if (enemyActive[i] && !enemyIsBeingSucked[i])
                {
                    if (checkCollision(playerX, playerY, playerWidth, playerHeight, enemyX[i], enemyY[i], enemyWidth[i], enemyHeight[i]))
                    {
                        if (enemyTouchCooldown <= 0.0f)
                        {
                            playerHP--;
                            enemyTouchCooldown = 0.5f;
                            playerTookDamage = true;
                            playerComboStreak = 0;
                            playerTotalPoints -= 100;
                            
                            std::cout << "Player hit! HP: " << playerHP << "/" << playerMaxHP << std::endl;
                            
                            if (playerHP <= 0)
                            {
                                playerLives = 0;
                                std::cout << "Player killed by enemy! GAME OVER!" << std::endl;
                                playerIsAlive = false;
                                break;
                            }
                        }
                    }
                }
            }

            // Check for player-bullet collisions
            for (int i = 0; i < maxBullets; i++)
            {
                if (bulletActive[i] && checkCollision(playerX, playerY, playerWidth, playerHeight,
                                                        bulletX[i], bulletY[i], bulletWidth[i], bulletHeight[i]))
                {
                    playerLives--;
                    playerTookDamage = true;
                    playerComboStreak = 0;
                    playerTotalPoints -= 50;

                    std::cout << "Player hit by bullet! Lives: " << playerLives << " | Points: " << playerTotalPoints << std::endl;
                    if (playerLives <= 0)
                    {
                        playerIsAlive = false;
                    }
                    bulletActive[i] = false;
                }
            }

            // Count active enemies
            bool allDead = true;
            for (int i = 0; i < enemyCount; i++)
            {
                if (enemyActive[i])
                {
                    allDead = false;
                    activeCount++;
                }
            }

            // Level completion checks
            if (allDead && !levelCompleted && levelStateCurrentLevel == 1)
            {
                levelCompleted = true;
                levelStateLevel1Complete = true;
                levelStateLevelTime = levelClock.getElapsedTime().asSeconds() - playerLevelStartTime;

                calculateLevelBonus();

                std::cout << "\n=== LEVEL 1 COMPLETE! ===" << std::endl;
                std::cout << "All enemies defeated!" << std::endl;
                std::cout << "Time: " << levelStateLevelTime << " seconds" << std::endl;
                std::cout << "Score: " << playerTotalPoints << " points!" << std::endl;
                std::cout << "\nPress L to load Level 2" << std::endl;
            }

            if (allDead && !levelCompleted && levelStateCurrentLevel == 2)
            {
                levelCompleted = true;
                levelStateLevel2Complete = true;
                levelStateLevelTime = levelClock.getElapsedTime().asSeconds() - playerLevelStartTime;

                calculateLevelBonus();
                addPoints(3000, "Level 2 Completion Bonus");

                std::cout << "\n=== LEVEL 2 COMPLETE! ===" << std::endl;
                std::cout << "All enemies defeated!" << std::endl;
                std::cout << "Time: " << levelStateLevelTime << " seconds" << std::endl;
                std::cout << "Score: " << playerTotalPoints << " points!" << std::endl;
                std::cout << "\n=== BOSS LEVEL UNLOCKED! ===" << std::endl;
                std::cout << "Press B to enter Boss Level" << std::endl;
            }

            // Load level 2 when L is pressed
            if (levelCompleted && Keyboard::isKeyPressed(Keyboard::L) && levelStateCurrentLevel == 1)
            {
                currentState = PLAYING_LEVEL2;
                levelStateCurrentLevel = 2;
                levelStateLevelTime = 0;
                playerTookDamage = false;
                playerComboStreak = 0;
                playerLives = 3;
                playerX = 64;
                playerY = 704;
                
                // Set HP for level 2
                playerMaxHP = 5;
                playerHP = 5;
                enemyTouchCooldown = 0.0f;

                // Reset player stats
                playerSpeed = playerBaseSpeed;
                vacuumRange = vacuumBaseRange;
                vacuumSuctionForce = vacuumBaseSuctionForce;

                // Reinitialize vacuum
                cleanupVacuum();
                initVacuum(5);

                // Create level 2
                cleanupLevelGrid();
                initLevelGrid(64);
                createLevel2();

                // Create level 2 enemies
                enemyCount = 0;
                initEnemy(enemyCount++, TYPE_GHOST, 3 * levelCellSize + 10, 2 * levelCellSize - enemyHeight[TYPE_GHOST]);
                initEnemy(enemyCount++, TYPE_GHOST, 12 * levelCellSize + 10, 2 * levelCellSize - enemyHeight[TYPE_GHOST]);
                initEnemy(enemyCount++, TYPE_GHOST, 5 * levelCellSize + 10, 12 * levelCellSize - enemyHeight[TYPE_GHOST]);
                initEnemy(enemyCount++, TYPE_GHOST, 12 * levelCellSize + 10, 12 * levelCellSize - enemyHeight[TYPE_GHOST]);

                initEnemy(enemyCount++, TYPE_SKELETON, 4 * levelCellSize + 10, 4 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 6 * levelCellSize + 10, 4 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 8 * levelCellSize + 10, 4 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 10 * levelCellSize + 10, 4 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 12 * levelCellSize + 10, 4 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 5 * levelCellSize + 10, 8 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 7 * levelCellSize + 10, 8 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 9 * levelCellSize + 10, 8 * levelCellSize - enemyHeight[TYPE_SKELETON]);
                initEnemy(enemyCount++, TYPE_SKELETON, 11 * levelCellSize + 10, 8 * levelCellSize - enemyHeight[TYPE_SKELETON]);

                initEnemy(enemyCount++, TYPE_INVISIBLE, 6 * levelCellSize + 10, 6 * levelCellSize - enemyHeight[TYPE_INVISIBLE]);
                initEnemy(enemyCount++, TYPE_INVISIBLE, 9 * levelCellSize + 10, 6 * levelCellSize - enemyHeight[TYPE_INVISIBLE]);
                initEnemy(enemyCount++, TYPE_INVISIBLE, 12 * levelCellSize + 10, 6 * levelCellSize - enemyHeight[TYPE_INVISIBLE]);

                initEnemy(enemyCount++, TYPE_CHELNOV, 4 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_CHELNOV]);
                initEnemy(enemyCount++, TYPE_CHELNOV, 7 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_CHELNOV]);
                initEnemy(enemyCount++, TYPE_CHELNOV, 10 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_CHELNOV]);
                initEnemy(enemyCount++, TYPE_CHELNOV, 13 * levelCellSize + 10, 10 * levelCellSize - enemyHeight[TYPE_CHELNOV]);

                // Create level 2 power-ups
                powerupCount = 4;
                initPowerUp(0, POWERUP_SPEED, 3 * levelCellSize, 2 * levelCellSize);
                initPowerUp(1, POWERUP_RANGE, 9 * levelCellSize, 5 * levelCellSize);
                initPowerUp(2, POWERUP_POWER, 6 * levelCellSize, 9 * levelCellSize);
                initPowerUp(3, POWERUP_LIFE, 12 * levelCellSize, 9 * levelCellSize);

                levelCompleted = false;
                playerLevelStartTime = levelClock.getElapsedTime().asSeconds();

                std::cout << "\n=== LEVEL 2 ===" << std::endl;
                std::cout << "Enemies: 3 Invisible Man, 4 Chelnov, 9 Skeletons, 4 Ghosts" << std::endl;
                std::cout << "Vacuum capacity: 5 enemies" << std::endl;
                std::cout << "Defeat all enemies!" << std::endl;
            }

            // Load boss level when B is pressed after level 2 completion
            if (levelCompleted && Keyboard::isKeyPressed(Keyboard::B) && levelStateCurrentLevel == 2)
            {
                currentState = PLAYING_BOSS_LEVEL;
                bossLevelInitialized = false;
                levelCompleted = false;
            }
        }

            // Update UI text
            string levelText = "LEVEL: " + to_string(levelStateCurrentLevel);
            if (levelStateCurrentLevel == 1)
                levelText += "\nEnemies: " + to_string(activeCount) + "/12";
            else
                levelText += "\nEnemies: " + to_string(activeCount) + "/20";

            if (levelCompleted && levelStateCurrentLevel == 1)
                levelText += "\nCOMPLETE! Press L";
            else if (gameCompleted)
                levelText += "\nGAME COMPLETE!";

            uiLevelInfo.setString(levelText);
            uiScoreInfo.setString("SCORE: " + to_string(playerTotalPoints));

            string comboText = "COMBO: " + to_string(playerComboStreak);
            if (playerComboStreak >= 3)
                comboText += " (x" + to_string((playerComboStreak >= 5) ? 2.0f : 1.5f) + ")";
            uiComboInfo.setString(comboText);

            string info = "Position: (" + to_string((int)playerX) + ", " + to_string((int)playerY) + ")";
            info += "\nLives: " + to_string(playerLives);
            info += "\nHP: " + to_string(playerHP) + "/" + to_string(playerMaxHP);
            info += "\nSpeed: " + to_string(playerSpeed).substr(0, 4);
            if (playerIsVacuuming)
                info += "\nVACUUMING (WASD=Direction)";
            info += "\nVacuum: " + to_string(vacuumCurrentCount) + "/" + to_string(vacuumMaxCapacity);
            if (vacuumIsFull)
                info += " (FULL - Can't Suck)";
            info += "\nRange: " + to_string((int)vacuumRange);
            info += "\nPower: " + to_string(vacuumSuctionForce).substr(0, 4);
            info += "\nDirection: ";
            switch ((int)vacuumAngle)
            {
            case 0:
                info += "RIGHT";
                break;
            case 90:
                info += "DOWN";
                break;
            case 180:
                info += "LEFT";
                break;
            case 270:
                info += "UP";
                break;
            }
            info += "\nAerial Defeats: " + to_string(playerAerialDefeats);
            info += "\nMulti-kill: " + to_string(playerKillStreak);
            uiPlayerInfo.setString(info);

            // Draw everything
            window.clear(Color(30, 30, 50));
            displayLevel(window);

        // Draw power-ups
        for (int i = 0; i < powerupCount; i++)
        {
            if (powerUpActive[i])
            {
                powerUpSprite[i].setPosition(powerUpX[i], powerUpY[i]);
                window.draw(powerUpSprite[i]);
            }
        }

        // Draw enemies
        for (int i = 0; i < enemyCount; i++)
        {
            if (enemyActive[i])
            {
                if (enemyIsBeingSucked[i])
                {
                    enemySprite[i].setColor(Color(200, 200, 255, 200));
                }
                window.draw(enemySprite[i]);
                enemySprite[i].setColor(Color::White);
            }
        }

        // Draw bullets
        for (int i = 0; i < maxBullets; i++)
        {
            if (bulletActive[i])
            {
                bulletSprite[i].setPosition(bulletX[i] - bulletWidth[i] / 2, bulletY[i] - bulletHeight[i] / 2);
                window.draw(bulletSprite[i]);
            }
        }

        // Draw thrown enemies
        for (int i = 0; i < maxThrownEnemies; i++)
        {
            if (thrownEnemyActive[i])
            {
                window.draw(thrownEnemySprite[i]);
            }
        }

        // Draw thrown enemy cluster
        drawThrownEnemyCluster(window);

        // Draw vacuum if active
        if (vacuumIsActive)
        {
            window.draw(vacuumSprite);
        }

        // Draw captured enemies orbiting player
        drawCapturedEnemies(window);

        // Draw player and UI
        window.draw(playerSprite);
        window.draw(uiPlayerInfo);
        window.draw(uiLevelInfo);
        window.draw(uiScoreInfo);
        window.draw(uiComboInfo);

            window.display();

            // Game over check
            if (!playerIsAlive)
            {
                finalScore = playerTotalPoints; // Store final score
                std::cout << "\n=== GAME OVER ===" << std::endl;
                std::cout << "You ran out of lives!" << std::endl;
                std::cout << "Final Score: " << playerTotalPoints << " points" << std::endl;
                currentState = GAME_OVER;
                gameOverSelection = 0; // Reset to first option
                gameInitialized = false;
                if (gameMusicPlaying)
                {
                    backgroundMusic.stop();
                    gameMusicPlaying = false;
                }
                if (menuMusic.getStatus() != Music::Playing)
                {
                    menuMusic.play();
                }
            }
        }
        else if (currentState == PLAYING_BOSS_LEVEL)
        {
            if (bossLevelInitialized)
            {
                float deltaTime = gameClock.restart().asSeconds();
                
                // Update boss
                updateBoss(deltaTime);
                
                // Update boss spawning system (balanced wave-based)
                updateBossSpawning(deltaTime, bossLevelRows, bossLevelCols, bossLevelCellSize);
                
                // Update minions
                updateMinions(deltaTime, playerX, playerY, 
                             bossLevelGrid, bossLevelRows, bossLevelCols, bossLevelCellSize);
                
                // Update tentacles (lifetimes)
                updateTentacles(deltaTime, bossLevelRows, bossLevelCols, bossLevelCellSize);
                
                // Update water
                updateWater(deltaTime);
                
                // Handle player input
                handlePlayerInput();
                
                // Temporarily swap level grid for boss level
                char** tempGrid = levelGrid;
                int tempCellSize = levelCellSize;
                int tempHeight = levelHeight;
                int tempWidth = levelWidth;
                levelGrid = bossLevelGrid;
                levelCellSize = bossLevelCellSize;
                levelHeight = bossLevelRows;
                levelWidth = bossLevelCols;
                
                updatePlayerPhysics(levelClock);
                
                // Update thrown enemies (like level 2) - use boss level grid
                updateThrownEnemies(maxThrownEnemies, 0, deltaTime); // enemyCount = 0 for boss level
                updateThrownEnemyCluster(0, deltaTime); // enemyCount = 0 for boss level
                
                // Restore original level grid
                levelGrid = tempGrid;
                levelCellSize = tempCellSize;
                levelHeight = tempHeight;
                levelWidth = tempWidth;
                
                // Handle vacuum effects on minions (like level 2)
                if (vacuumIsActive)
                {
                    applyVacuumEffectToMinions(minionCount, deltaTime);
                    
                    // Position vacuum sprite
                    float vacX = playerX + playerWidth / 2;
                    float vacY = playerY + playerHeight / 2;
                    float angleRad = vacuumAngle * 3.14159f / 180.0f;
                    float offsetX = cos(angleRad) * 40.0f;
                    float offsetY = sin(angleRad) * 40.0f;
                    
                    vacuumSprite.setPosition(vacX + offsetX, vacY + offsetY);
                    vacuumSprite.setRotation(vacuumAngle + 90);
                }
                else
                {
                    // Reset suction state for all minions
                    for (int i = 0; i < minionCount; i++)
                    {
                        if (minionActive[i] && minionCaptured[i])
                        {
                            // Keep captured state, but reset if needed
                        }
                    }
                }
                
                // Check minion-tentacle collision
                checkMinionTentacleCollision();
                
                // Check boss damage
                checkBossDamage();
                
                // Check boss defeat
                if (bossDefeated && !bossLevelComplete)
                {
                    bossLevelComplete = true;
                    addPoints(10000, "Boss Defeated!");
                    std::cout << "\n=== BOSS DEFEATED! ===" << std::endl;
                    std::cout << "Final Score: " << playerTotalPoints << " points!" << std::endl;
                    std::cout << "\n=== GAME COMPLETE! ===" << std::endl;
                }
                
                // Render boss level
                window.clear();
                
                // Draw level background
                if (levelBackgroundTexture.getSize().x > 0)
                {
                    levelBackgroundSprite.setScale((float)screen_x / levelBackgroundTexture.getSize().x,
                                                    (float)screen_y / levelBackgroundTexture.getSize().y);
                    window.draw(levelBackgroundSprite);
                }
                
                // Draw level grid
                for (int i = 0; i < bossLevelRows; i++)
                {
                    for (int j = 0; j < bossLevelCols; j++)
                    {
                        if (bossLevelGrid[i][j] == '#')
                        {
                            levelBlockSprite.setPosition(j * bossLevelCellSize, i * bossLevelCellSize);
                            window.draw(levelBlockSprite);
                        }
                    }
                }
                
                // Draw water
                drawWater(window, screen_x, screen_y);
                
                // Draw tentacles
                drawTentacles(window);
                
                // Draw minions
                drawMinions(window);
                
                // Draw vacuum sprite (like level 2)
                if (vacuumIsActive)
                {
                    window.draw(vacuumSprite);
                }
                
                // Draw thrown enemies (like level 2)
                for (int i = 0; i < maxThrownEnemies; i++)
                {
                    if (thrownEnemyActive[i])
                    {
                        window.draw(thrownEnemySprite[i]);
                    }
                }
                
                // Draw thrown enemy cluster (like level 2)
                if (thrownClusterActive)
                {
                    drawThrownEnemyCluster(window);
                }
                
                // Draw captured enemies orbiting player (like level 2)
                drawCapturedEnemies(window);
                
                // Draw boss
                drawBoss(window, uiFont);
                
                // Draw player
                window.draw(playerSprite);
                
                // Draw UI
                string bossText = "BOSS LEVEL\nBoss Health: " + to_string(bossHealth) + "/" + to_string(bossMaxHealth);
                if (bossDefeated)
                    bossText += "\nBOSS DEFEATED!";
                uiLevelInfo.setString(bossText);
                uiScoreInfo.setString("SCORE: " + to_string(playerTotalPoints));
                window.draw(uiPlayerInfo);
                window.draw(uiLevelInfo);
                window.draw(uiScoreInfo);
                window.draw(uiComboInfo);
                
                window.display();
                
                // Game over check
                if (!playerIsAlive)
                {
                    finalScore = playerTotalPoints;
                    std::cout << "\n=== GAME OVER ===" << std::endl;
                    std::cout << "You ran out of lives!" << std::endl;
                    std::cout << "Final Score: " << playerTotalPoints << " points" << std::endl;
                    currentState = GAME_OVER;
                    gameOverSelection = 0;
                    bossLevelInitialized = false;
                    cleanupBossLevel(bossLevelGrid, bossLevelRows);
                    cleanupBossData();
                    if (gameMusicPlaying)
                    {
                        backgroundMusic.stop();
                        gameMusicPlaying = false;
                    }
                    if (menuMusic.getStatus() != Music::Playing)
                    {
                        menuMusic.play();
                    }
                }
            }
        }
    }

    // Cleanup
    if (gameMusicPlaying)
    {
        backgroundMusic.stop();
    }
    menuMusic.stop();
    if (gameInitialized)
    {
        cleanupLevelGrid();
        cleanupVacuum();
        cleanupThrownEnemyCluster();
    }
    if (bossLevelInitialized)
    {
        cleanupBossLevel(bossLevelGrid, bossLevelRows);
        cleanupBossData();
    }

    std::cout << "Game ended successfully!" << std::endl;
    return 0;
}

// Clean up vacuum memory
void cleanupVacuum()
{
    if (vacuumCapturedEnemyTypes != nullptr)
    {
        delete[] vacuumCapturedEnemyTypes;
        vacuumCapturedEnemyTypes = nullptr;
    }
}

// Clean up thrown enemy cluster memory
void cleanupThrownEnemyCluster()
{
    if (thrownClusterEnemyTypes != nullptr)
    {
        delete[] thrownClusterEnemyTypes;
        thrownClusterEnemyTypes = nullptr;
    }
}

// Get points for capturing an enemy
int getEnemyCapturePoints(int enemyType)
{
    switch (enemyType)
    {
    case TYPE_GHOST:
        return 50;
    case TYPE_SKELETON:
        return 75;
    case TYPE_INVISIBLE:
        return 150;
    case TYPE_CHELNOV:
        return 200;
    default:
        return 0;
    }
}

// Get points for defeating an enemy
int getEnemyDefeatPoints(int enemyType)
{
    return getEnemyCapturePoints(enemyType) * 2;
}

// Calculate multi-kill bonus
int getMultiKillBonus(int killCount)
{
    if (killCount >= 5)
        return 500;
    if (killCount >= 4)
        return 300;
    if (killCount >= 3)
        return 150;
    if (killCount >= 2)
        return 50;
    return 0;
}

// Add points to player score with combo multiplier
void addPoints(int points, const string &reason)
{
    float multiplier = 1.0f;
    if (playerComboStreak >= 5)
        multiplier = 2.0f;
    else if (playerComboStreak >= 3)
        multiplier = 1.5f;

    int finalPoints = (int)(points * multiplier);
    playerTotalPoints += finalPoints;

    playerKillStreak++;
    playerLastKillTime = 0;

    std::cout << reason << ": +" << finalPoints << " points";
    if (multiplier > 1.0f)
        std::cout << " (x" << multiplier << " combo)";
    std::cout << " | Total: " << playerTotalPoints << std::endl;
}

// Check and apply multi-kill bonus
void checkMultiKill()
{
    if (playerKillStreak >= 2)
    {
        int bonus = getMultiKillBonus(playerKillStreak);
        if (bonus > 0)
        {
            playerTotalPoints += bonus;
            std::cout << "MULTI-KILL BONUS! " << playerKillStreak << " kills: +" << bonus << " points!" << std::endl;
        }
    }
    playerKillStreak = 0;
}

// Calculate level completion bonuses
void calculateLevelBonus()
{
    float levelTime = levelStateLevelTime;
    int levelNumber = levelStateCurrentLevel;

    // Level clear bonus
    int levelBonus = (levelNumber == 1) ? 1000 : 2000;
    addPoints(levelBonus, "Level Clear Bonus");

    // No damage bonus
    if (!playerTookDamage)
    {
        int noDamageBonus = (levelNumber == 1) ? 1500 : 2500;
        addPoints(noDamageBonus, "No Damage Bonus");
    }

    // Speed bonus based on completion time
    if (levelNumber == 1)
    {
        if (levelTime < 30)
            addPoints(2000, "Speed Demon (<30s)");
        else if (levelTime < 45)
            addPoints(1000, "Fast Clear (<45s)");
        else if (levelTime < 60)
            addPoints(500, "Good Speed (<60s)");
    }
    else
    {
        if (levelTime < 60)
            addPoints(3000, "Speed Demon (<60s)");
        else if (levelTime < 90)
            addPoints(1500, "Fast Clear (<90s)");
        else if (levelTime < 120)
            addPoints(750, "Good Speed (<120s)");
    }

    // All enemies defeated bonus
    if (levelNumber == 1 && levelStateLevel1EnemiesKilled >= levelStateLevel1TotalEnemies)
    {
        addPoints(500, "All Enemies Defeated Bonus");
    }
}

// Initialize vacuum
void initVacuum(int capacity)
{
    vacuumCapturedEnemyTypes = new int[capacity];
    for (int i = 0; i < capacity; i++)
    {
        vacuumCapturedEnemyTypes[i] = -1;
    }
    vacuumMaxCapacity = capacity;
    vacuumCurrentCount = 0;
    vacuumIsFull = false;
    vacuumRange = 220.f;
    vacuumBaseRange = 220.f;
    vacuumSuctionForce = 5.0f;
    vacuumBaseSuctionForce = 5.0f;
    vacuumCaptureDistance = 30.f;
    vacuumAngle = 0.f;
    vacuumIsActive = false;

    if (!vacuumTexture.loadFromFile("Assets/vaccum.png"))
    {
        std::cout << "Warning: Could not load Assets/vaccum.png" << std::endl;
    }
    vacuumSprite.setTexture(vacuumTexture);
    vacuumSprite.setOrigin(vacuumTexture.getSize().x / 2.f, vacuumTexture.getSize().y / 2.f);
    vacuumSprite.setScale(1.f, 1.f);
}

// Initialize thrown enemy cluster
void initThrownEnemyCluster(int maxEnemies)
{
    thrownClusterEnemyTypes = new int[maxEnemies];
    for (int i = 0; i < maxEnemies; i++)
    {
        thrownClusterEnemyTypes[i] = -1;
    }
    thrownClusterMaxEnemies = maxEnemies;
    thrownClusterEnemyCount = 0;
    thrownClusterActive = false;
    thrownClusterX = 0;
    thrownClusterY = 0;
    thrownClusterVelocityX = 0;
    thrownClusterVelocityY = 0;
    thrownClusterRadius = 30.0f;
    thrownClusterIsBouncing = false;
    thrownClusterBounceTimer = 0;
    thrownClusterRotationAngle = 0;
    thrownClusterRotationSpeed = 2.0f;

    // Load throw textures for each enemy type
    if (!thrownClusterThrowTextures[TYPE_GHOST].loadFromFile("Assets/ghostThrow.png"))
    {
        std::cout << "Warning: Could not load ghostThrow.png" << std::endl;
    }
    if (!thrownClusterThrowTextures[TYPE_SKELETON].loadFromFile("Assets/skeltonThrow.png"))
    {
        std::cout << "Warning: Could not load skeltonThrow.png" << std::endl;
    }
    if (!thrownClusterThrowTextures[TYPE_INVISIBLE].loadFromFile("Assets/invisibleThrow.png"))
    {
        std::cout << "Warning: Could not load invisibleThrow.png" << std::endl;
    }
    if (!thrownClusterThrowTextures[TYPE_CHELNOV].loadFromFile("Assets/chelnovThrow.png"))
    {
        std::cout << "Warning: Could not load chelnovThrow.png" << std::endl;
    }
}

// Initialize UI elements
void initUI()
{
    if (uiFont.loadFromFile("Assets/arial.ttf"))
    {
        uiPlayerInfo.setFont(uiFont);
        uiPlayerInfo.setCharacterSize(18);
        uiPlayerInfo.setFillColor(Color::White);
        uiPlayerInfo.setOutlineColor(Color::Black);
        uiPlayerInfo.setOutlineThickness(1);
        uiPlayerInfo.setPosition(10, 10);

        uiLevelInfo.setFont(uiFont);
        uiLevelInfo.setCharacterSize(20);
        uiLevelInfo.setFillColor(Color::Yellow);
        uiLevelInfo.setOutlineColor(Color::Black);
        uiLevelInfo.setOutlineThickness(1);
        uiLevelInfo.setPosition(screen_x - 200, 10);

        uiScoreInfo.setFont(uiFont);
        uiScoreInfo.setCharacterSize(24);
        uiScoreInfo.setFillColor(Color::Cyan);
        uiScoreInfo.setOutlineColor(Color::Black);
        uiScoreInfo.setOutlineThickness(1);
        uiScoreInfo.setPosition(screen_x / 2 - 100, 10);

        uiComboInfo.setFont(uiFont);
        uiComboInfo.setCharacterSize(22);
        uiComboInfo.setFillColor(Color::Magenta);
        uiComboInfo.setOutlineColor(Color::Black);
        uiComboInfo.setOutlineThickness(1);
        uiComboInfo.setPosition(screen_x / 2 - 100, 40);
    }
}

// Make sprite face right by flipping it
void orientSpriteFacingRight(Sprite &s, int width)
{
    Vector2f sc = s.getScale();
    sc.x = -fabs(sc.x);
    s.setScale(sc);
    s.setOrigin((float)width, 0.f);
}

// Make sprite face left (normal orientation)
void orientSpriteFacingLeft(Sprite &s, int width)
{
    Vector2f sc = s.getScale();
    sc.x = fabs(sc.x);
    s.setScale(sc);
    s.setOrigin(0.f, 0.f);
}

// Initialize player with chosen character
void initPlayer(int characterChoice)
{
    playerIsAlive = true;
    playerLives = 3;
    playerOnGround = false;
    playerFacingRight = true;
    playerIsJumping = false;
    playerVelocityX = 0;
    playerVelocityY = 0;
    playerCharacterType = (characterChoice == 1) ? YELLOW_TUMBLE : GREEN_TUMBLE;
    playerTotalPoints = 0;
    playerComboStreak = 0;
    playerTookDamage = false;
    playerLevelStartTime = 0;
    playerKillStreak = 0;
    playerLastKillTime = 0;
    playerAerialDefeats = 0;
    playerIsVacuuming = false;

    if (characterChoice == 1) // Yellow character (fast)
    {
        playerSpeed = 7.5f;
        playerBaseSpeed = 7.5f;
        playerJumpStrength = -22.0f;
        playerHeight = 64;
        playerWidth = 40;
        if (playerTexture.loadFromFile("Assets/yellow.png"))
        {
            playerSprite.setTexture(playerTexture);
            playerSprite.setTextureRect(sf::IntRect(0, 0, 40, 64));
        }
        else
        {
            std::cout << "Warning: Could not load yellow.png" << std::endl;
        }
    }
    else // Green character (strong)
    {
        playerSpeed = 5.0f;
        playerBaseSpeed = 5.0f;
        playerJumpStrength = -20.0f;
        playerHeight = 64;
        playerWidth = 50;
        if (playerTexture.loadFromFile("Assets/green.png"))
        {
            playerSprite.setTexture(playerTexture);
            playerSprite.setTextureRect(sf::IntRect(0, 0, 50, 64));
        }
        else
        {
            std::cout << "Warning: Could not load green.png" << std::endl;
        }
    }

    playerSprite.setScale(1.2f, 1.2f);
    if (playerFacingRight)
        orientSpriteFacingRight(playerSprite, playerWidth);
    else
        orientSpriteFacingLeft(playerSprite, playerWidth);
    playerSprite.setPosition(playerX, playerY);
}

// Initialize enemy with specific type and position
void initEnemy(int enemyIndex, int type, float x, float y)
{
    enemyX[enemyIndex] = x;
    enemyY[enemyIndex] = y;
    enemyOriginalY[enemyIndex] = y;
    enemyVelocityX[enemyIndex] = 0.0f;
    enemyVelocityY[enemyIndex] = 0.0f;
    enemyActive[enemyIndex] = true;
    enemyFacingRight[enemyIndex] = true;
    enemyType[enemyIndex] = type;
    enemyShootTimer[enemyIndex] = 0.f;
    enemyPauseTimer[enemyIndex] = 0.f;
    enemyPauseDuration[enemyIndex] = 0.f;
    enemyPaused[enemyIndex] = false;
    enemyInvisTimer[enemyIndex] = 0.f;
    enemyInvisDuration[enemyIndex] = 0.f;
    enemyVisible[enemyIndex] = true;
    enemyCanBeCaptured[enemyIndex] = true;
    enemyWindupTimer[enemyIndex] = 0.f;
    enemyInWindup[enemyIndex] = false;
    enemyIsBeingSucked[enemyIndex] = false;

    // Set properties based on enemy type
    if (type == TYPE_GHOST)
    {
        enemyWidth[enemyIndex] = 63;
        enemyHeight[enemyIndex] = 60;
        enemyVelocityX[enemyIndex] = 0.8f;
        if (enemyTexture[enemyIndex].loadFromFile("Assets/ghost.png"))
        {
            enemySprite[enemyIndex].setTexture(enemyTexture[enemyIndex]);
            enemySprite[enemyIndex].setTextureRect(sf::IntRect(0, 0, enemyWidth[enemyIndex], enemyHeight[enemyIndex]));
            enemySprite[enemyIndex].setScale(0.9f, 0.9f);
        }
        else
        {
            std::cout << "Warning: Could not load ghost.png" << std::endl;
        }
    }
    else if (type == TYPE_SKELETON)
    {
        enemyWidth[enemyIndex] = 43;
        enemyHeight[enemyIndex] = 64;
        enemyVelocityX[enemyIndex] = 1.6f;
        if (enemyTexture[enemyIndex].loadFromFile("Assets/skelton.png"))
        {
            enemySprite[enemyIndex].setTexture(enemyTexture[enemyIndex]);
            enemySprite[enemyIndex].setTextureRect(sf::IntRect(0, 0, enemyWidth[enemyIndex], enemyHeight[enemyIndex]));
            enemySprite[enemyIndex].setScale(1.1f, 1.1f);
        }
        else
        {
            std::cout << "Warning: Could not load skelton.png" << std::endl;
        }
    }
    else if (type == TYPE_INVISIBLE)
    {
        enemyWidth[enemyIndex] = 39;
        enemyHeight[enemyIndex] = 64;
        enemyVelocityX[enemyIndex] = 1.2f;
        if (enemyTexture[enemyIndex].loadFromFile("Assets/invisible.png"))
        {
            enemySprite[enemyIndex].setTexture(enemyTexture[enemyIndex]);
            enemySprite[enemyIndex].setTextureRect(sf::IntRect(0, 0, enemyWidth[enemyIndex], enemyHeight[enemyIndex]));
            enemySprite[enemyIndex].setScale(1.1f, 1.1f);
        }
        else
        {
            std::cout << "Warning: Could not load invisible.png" << std::endl;
        }
        enemyInvisDuration[enemyIndex] = 2.0f + (rand() % 3);
    }
    else if (type == TYPE_CHELNOV)
    {
        enemyWidth[enemyIndex] = 43;
        enemyHeight[enemyIndex] = 64;
        enemyVelocityX[enemyIndex] = 1.4f;
        if (enemyTexture[enemyIndex].loadFromFile("Assets/chelnov.png"))
        {
            enemySprite[enemyIndex].setTexture(enemyTexture[enemyIndex]);
            enemySprite[enemyIndex].setTextureRect(sf::IntRect(0, 0, enemyWidth[enemyIndex], enemyHeight[enemyIndex]));
            enemySprite[enemyIndex].setScale(1.1f, 1.0f);
        }
        else
        {
            std::cout << "Warning: Could not load chelnov.png" << std::endl;
        }
    }

    // Set sprite orientation and position
    if (enemyFacingRight[enemyIndex])
        orientSpriteFacingRight(enemySprite[enemyIndex], enemyWidth[enemyIndex]);
    else
        orientSpriteFacingLeft(enemySprite[enemyIndex], enemyWidth[enemyIndex]);
    enemySprite[enemyIndex].setPosition(enemyX[enemyIndex], enemyY[enemyIndex]);
}

// Initialize bullet
void initBullet(int bulletIndex)
{
    bulletActive[bulletIndex] = false;
    bulletX[bulletIndex] = 0;
    bulletY[bulletIndex] = 0;
    bulletVelocityX[bulletIndex] = 0;
    bulletVelocityY[bulletIndex] = 0;
    bulletDirection[bulletIndex] = 0;
    bulletWidth[bulletIndex] = 15;
    bulletHeight[bulletIndex] = 15;

    if (!bulletTexture[bulletIndex].loadFromFile("Assets/bullet.png"))
    {
        std::cout << "Warning: Could not load bullet.png - using placeholder" << std::endl;
        bulletSprite[bulletIndex].setTextureRect(sf::IntRect(0, 0, bulletWidth[bulletIndex], bulletHeight[bulletIndex]));
        bulletSprite[bulletIndex].setColor(Color::Red);
    }
    else
    {
        bulletSprite[bulletIndex].setTexture(bulletTexture[bulletIndex]);
        bulletSprite[bulletIndex].setScale(0.8f, 0.8f);
    }
}

// Initialize thrown enemy projectile
void initThrownEnemy(int thrownIndex, int type, float x, float y, float vx, float vy)
{
    thrownEnemyType[thrownIndex] = type;
    thrownEnemyX[thrownIndex] = x;
    thrownEnemyY[thrownIndex] = y;
    thrownEnemyVelocityX[thrownIndex] = vx;
    thrownEnemyVelocityY[thrownIndex] = vy;
    thrownEnemyActive[thrownIndex] = true;
    thrownEnemyWidth[thrownIndex] = 30;
    thrownEnemyHeight[thrownIndex] = 30;
    thrownEnemyIsRolling[thrownIndex] = false;

    // Load appropriate texture based on enemy type
    if (type == TYPE_GHOST)
    {
        thrownEnemyTexture[thrownIndex].loadFromFile("Assets/ghostThrow.png");
    }
    else if (type == TYPE_SKELETON)
    {
        thrownEnemyTexture[thrownIndex].loadFromFile("Assets/skeltonThrow.png");
    }
    else if (type == TYPE_INVISIBLE)
    {
        thrownEnemyTexture[thrownIndex].loadFromFile("Assets/invisibleThrow.png");
    }
    else if (type == TYPE_CHELNOV)
    {
        thrownEnemyTexture[thrownIndex].loadFromFile("Assets/chelnovThrow.png");
    }

    thrownEnemySprite[thrownIndex].setTexture(thrownEnemyTexture[thrownIndex]);
    thrownEnemySprite[thrownIndex].setScale(0.8f, 0.8f);
}

// Initialize power-up
void initPowerUp(int powerupIndex, int type, float x, float y)
{
    powerUpX[powerupIndex] = x;
    powerUpY[powerupIndex] = y;
    powerUpType[powerupIndex] = type;
    powerUpActive[powerupIndex] = true;
    powerUpFloatTimer[powerupIndex] = 0;
    powerUpWidth[powerupIndex] = 32;
    powerUpHeight[powerupIndex] = 32;
    powerUpDuration[powerupIndex] = 10.0f;
    powerUpTimer[powerupIndex] = 0;

    // Determine texture file based on power-up type
    string textureFile;
    switch (type)
    {
    case POWERUP_SPEED:
        textureFile = "Assets/speed.png";
        break;
    case POWERUP_RANGE:
        textureFile = "Assets/range.png";
        break;
    case POWERUP_POWER:
        textureFile = "Assets/power.png";
        break;
    case POWERUP_LIFE:
        textureFile = "Assets/life.png";
        break;
    }

    // Load texture or create colored placeholder
    if (!powerUpTexture[powerupIndex].loadFromFile(textureFile))
    {
        std::cout << "Warning: Could not load " << textureFile << " - using colored square" << std::endl;
        powerUpSprite[powerupIndex].setTextureRect(sf::IntRect(0, 0, 32, 32));
        switch (type)
        {
        case POWERUP_SPEED:
            powerUpSprite[powerupIndex].setColor(Color::Green);
            break;
        case POWERUP_RANGE:
            powerUpSprite[powerupIndex].setColor(Color::Blue);
            break;
        case POWERUP_POWER:
            powerUpSprite[powerupIndex].setColor(Color::Red);
            break;
        case POWERUP_LIFE:
            powerUpSprite[powerupIndex].setColor(Color::Yellow);
            break;
        }
    }
    else
    {
        powerUpSprite[powerupIndex].setTexture(powerUpTexture[powerupIndex]);
    }

    powerUpSprite[powerupIndex].setScale(0.5, 0.5);
}

// Initialize level grid
void initLevelGrid(int cellSize)
{
    levelHeight = 14;
    levelWidth = 18;
    levelCellSize = cellSize;

    // Allocate memory for grid
    levelGrid = new char *[levelHeight];
    for (int i = 0; i < levelHeight; i++)
    {
        levelGrid[i] = new char[levelWidth];
        for (int j = 0; j < levelWidth; j++)
        {
            levelGrid[i][j] = ' ';
        }
    }

    // Load textures
    if (!levelBlockTexture.loadFromFile("Assets/block1.png"))
    {
        std::cout << "Error loading block texture!" << std::endl;
    }
    levelBlockSprite.setTexture(levelBlockTexture);

    if (!levelPlatformTexture.loadFromFile("Assets/platform.png"))
    {
        levelPlatformTexture = levelBlockTexture;
    }
    levelPlatformSprite.setTexture(levelPlatformTexture);

    if (!levelBackgroundTexture.loadFromFile("Assets/bg.png"))
    {
        std::cout << "Error loading background texture!" << std::endl;
    }
    levelBackgroundSprite.setTexture(levelBackgroundTexture);
}

// Create level 1 layout
void createLevel1()
{
    // Border walls
    for (int j = 0; j < levelWidth; j++)
    {
        levelGrid[0][j] = '#';
        levelGrid[levelHeight - 1][j] = '#';
    }
    for (int i = 1; i < levelHeight - 1; i++)
    {
        levelGrid[i][0] = '#';
        levelGrid[i][levelWidth - 1] = '#';
    }

    // Horizontal platforms
    for (int j = 3; j <= 14; j++)
    {
        levelGrid[3][j] = '#';
        levelGrid[10][j] = '#';
    }

    // Additional platforms
    levelGrid[5][1] = '#';
    levelGrid[5][16] = '#';

    levelGrid[7][1] = '#';
    levelGrid[7][2] = '#';
    levelGrid[7][3] = '#';
    levelGrid[7][4] = '#';

    levelGrid[8][7] = '#';
    levelGrid[8][8] = '#';
    levelGrid[8][9] = '#';
    levelGrid[8][10] = '#';
    levelGrid[6][7] = '#';
    levelGrid[6][8] = '#';
    levelGrid[6][9] = '#';
    levelGrid[6][10] = '#';

    levelGrid[7][13] = '#';
    levelGrid[7][14] = '#';
    levelGrid[7][15] = '#';
    levelGrid[7][16] = '#';

    levelGrid[12][1] = '#';
    levelGrid[12][2] = '#';
    levelGrid[12][15] = '#';
    levelGrid[12][16] = '#';
}

// Create level 2 layout
void createLevel2()
{
    // Clear grid
    for (int i = 0; i < levelHeight; i++)
    {
        for (int j = 0; j < levelWidth; j++)
        {
            levelGrid[i][j] = ' ';
        }
    }

    // Border walls
    for (int j = 0; j < levelWidth; j++)
    {
        levelGrid[0][j] = '#';
        levelGrid[levelHeight - 1][j] = '#';
    }
    for (int i = 1; i < levelHeight - 1; i++)
    {
        levelGrid[i][0] = '#';
        levelGrid[i][levelWidth - 1] = '#';
    }

    // Horizontal platforms
    for (int j = 4; j <= 13; j++)
    {
        levelGrid[2][j] = '#';
        levelGrid[4][j] = '#';
        levelGrid[6][j] = '#';
        levelGrid[8][j] = '#';
        levelGrid[10][j] = '#';
        levelGrid[12][j] = '#';
    }

    // Diagonal platforms
    for (int i = 0; i < 4; i++)
    {
        int startRow = 5 + i;
        int startCol = 4 + i;
        for (int j = 0; j < 6; j++)
        {
            if (startRow + j < levelHeight - 1 && startCol + j < levelWidth - 1)
            {
                levelGrid[startRow + j][startCol + j] = '#';
            }
        }
    }

    // Additional platforms
    levelGrid[3][3] = '#';
    levelGrid[5][1] = '#';
    levelGrid[10][1] = '#';
    levelGrid[8][3] = '#';
    levelGrid[11][14] = '#';
    levelGrid[11][15] = '#';

    levelGrid[4][16] = '#';
    levelGrid[8][16] = '#';
    levelGrid[6][14] = '#';
}

// Display level on screen
void displayLevel(RenderWindow &window)
{
    window.draw(levelBackgroundSprite);

    for (int i = 0; i < levelHeight; i++)
    {
        for (int j = 0; j < levelWidth; j++)
        {
            if (levelGrid[i][j] == '#')
            {
                if (i > 0 && i < levelHeight - 1)
                {
                    levelPlatformSprite.setPosition(j * levelCellSize, i * levelCellSize);
                    window.draw(levelPlatformSprite);
                }
                else
                {
                    levelBlockSprite.setPosition(j * levelCellSize, i * levelCellSize);
                    window.draw(levelBlockSprite);
                }
            }
        }
    }
}

// Check if a tile is solid (contains a platform/wall)
bool tileSolid(int gridY, int gridX)
{
    if (gridY < 0 || gridY >= levelHeight || gridX < 0 || gridX >= levelWidth)
        return false;
    return levelGrid[gridY][gridX] == '#';
}

// Check if line between two columns on same row is clear of obstacles
bool clearLineBetween(int row, int colA, int colB)
{
    if (row < 0 || row >= levelHeight)
        return false;

    int start = min(colA, colB) + 1;
    int end = max(colA, colB) - 1;

    for (int c = start; c <= end; c++)
    {
        if (c >= 0 && c < levelWidth && levelGrid[row][c] == '#')
            return false;
    }
    return true;
}

// Update enemy physics (movement and platform boundaries)
void updateEnemyPhysics(int enemyIndex)
{
    if (!enemyActive[enemyIndex] || enemyIsBeingSucked[enemyIndex])
        return;

    float newX = enemyX[enemyIndex] + enemyVelocityX[enemyIndex];
    enemyY[enemyIndex] = enemyOriginalY[enemyIndex]; // Keep original Y position

    // Reverse direction at screen edges
    if (enemyVelocityX[enemyIndex] < 0 && newX < 0)
    {
        enemyVelocityX[enemyIndex] = -enemyVelocityX[enemyIndex];
        enemyFacingRight[enemyIndex] = true;
        newX = 0;
    }
    else if (enemyVelocityX[enemyIndex] > 0 && newX + enemyWidth[enemyIndex] > screen_x)
    {
        enemyVelocityX[enemyIndex] = -enemyVelocityX[enemyIndex];
        enemyFacingRight[enemyIndex] = false;
        newX = screen_x - enemyWidth[enemyIndex];
    }

    // Check platform below enemy
    int gridYBelow = (int)((enemyOriginalY[enemyIndex] + enemyHeight[enemyIndex]) / levelCellSize);
    if (gridYBelow < 0 || gridYBelow >= levelHeight)
    {
        enemyX[enemyIndex] = newX;
        enemySprite[enemyIndex].setPosition(enemyX[enemyIndex], enemyY[enemyIndex]);
        return;
    }

    // Check if platform exists under left and right sides
    int leftGridX = (int)(newX / levelCellSize);
    int rightGridX = (int)((newX + enemyWidth[enemyIndex]) / levelCellSize);

    bool hasPlatformUnderLeft = false;
    bool hasPlatformUnderRight = false;

    if (leftGridX >= 0 && leftGridX < levelWidth)
        hasPlatformUnderLeft = (levelGrid[gridYBelow][leftGridX] == '#');

    if (rightGridX >= 0 && rightGridX < levelWidth)
        hasPlatformUnderRight = (levelGrid[gridYBelow][rightGridX] == '#');

    // Reverse direction if reaching platform edge
    if (enemyVelocityX[enemyIndex] < 0 && !hasPlatformUnderLeft)
    {
        enemyVelocityX[enemyIndex] = -enemyVelocityX[enemyIndex];
        enemyFacingRight[enemyIndex] = true;
        newX = enemyX[enemyIndex];
    }
    else if (enemyVelocityX[enemyIndex] > 0 && !hasPlatformUnderRight)
    {
        enemyVelocityX[enemyIndex] = -enemyVelocityX[enemyIndex];
        enemyFacingRight[enemyIndex] = false;
        newX = enemyX[enemyIndex];
    }

    enemyX[enemyIndex] = newX;

    // Keep within screen bounds
    if (enemyX[enemyIndex] < 0)
        enemyX[enemyIndex] = 0;
    if (enemyX[enemyIndex] + enemyWidth[enemyIndex] > screen_x)
        enemyX[enemyIndex] = screen_x - enemyWidth[enemyIndex];

    // Update sprite position and orientation
    enemySprite[enemyIndex].setPosition(enemyX[enemyIndex], enemyY[enemyIndex]);
    if (enemyFacingRight[enemyIndex])
        orientSpriteFacingRight(enemySprite[enemyIndex], enemyWidth[enemyIndex]);
    else
        orientSpriteFacingLeft(enemySprite[enemyIndex], enemyWidth[enemyIndex]);
}

// Update player physics including movement, jumping, and collision
void updatePlayerPhysics(Clock &levelClock)
{
    if (!playerIsAlive)
        return;

    // Handle vacuuming state
    if (playerIsVacuuming)
    {
        playerVelocityX = 0;
        playerVelocityY = 0;
    }
    else
    {
        // Apply gravity when not on ground
        if (!playerOnGround)
        {
            playerVelocityY += physicsGravity;
            if (playerVelocityY > physicsTerminalVelocity)
                playerVelocityY = physicsTerminalVelocity;
        }
    }

    // Calculate new position
    float newX = playerX + playerVelocityX;
    float newY = playerY + playerVelocityY;

    // Check left collision
    if (playerVelocityX < 0)
    {
        int leftGridX = (int)newX / levelCellSize;
        int gridYTop = (int)(playerY + 10) / levelCellSize;
        int gridYBottom = (int)(playerY + playerHeight - 10) / levelCellSize;

        if (leftGridX >= 0 && (tileSolid(gridYTop, leftGridX) || tileSolid(gridYBottom, leftGridX)))
        {
            newX = (leftGridX + 1) * levelCellSize;
            playerVelocityX = 0;
        }
    }
    // Check right collision
    else if (playerVelocityX > 0)
    {
        int rightGridX = (int)(newX + playerWidth) / levelCellSize;
        int gridYTop = (int)(playerY + 10) / levelCellSize;
        int gridYBottom = (int)(playerY + playerHeight - 10) / levelCellSize;

        if (rightGridX < levelWidth && (tileSolid(gridYTop, rightGridX) || tileSolid(gridYBottom, rightGridX)))
        {
            newX = rightGridX * levelCellSize - playerWidth;
            playerVelocityX = 0;
        }
    }

    playerOnGround = false;

    // Check downward collision (landing on ground)
    if (playerVelocityY > 0)
    {
        int feetGridY = (int)(newY + playerHeight) / levelCellSize;
        int leftGridX = (int)(playerX + 5) / levelCellSize;
        int rightGridX = (int)(playerX + playerWidth - 5) / levelCellSize;

        if (feetGridY < levelHeight)
        {
            if ((leftGridX >= 0 && tileSolid(feetGridY, leftGridX)) ||
                (rightGridX < levelWidth && tileSolid(feetGridY, rightGridX)))
            {
                newY = feetGridY * levelCellSize - playerHeight;
                playerVelocityY = 0;
                playerOnGround = true;
                playerIsJumping = false;
            }
        }
    }
    // Check upward collision (hitting ceiling)
    else if (playerVelocityY < 0)
    {
        int headGridY = (int)newY / levelCellSize;
        int leftGridX = (int)(playerX + 5) / levelCellSize;
        int rightGridX = (int)(playerX + playerWidth - 5) / levelCellSize;

        if (headGridY >= 0)
        {
            if ((leftGridX >= 0 && tileSolid(headGridY, leftGridX)) ||
                (rightGridX < levelWidth && tileSolid(headGridY, rightGridX)))
            {
                newY = (headGridY + 1) * levelCellSize;
                playerVelocityY = 0;
            }
        }
    }

    playerX = newX;
    playerY = newY;

    // Keep player within screen bounds
    if (playerX < 0)
        playerX = 0;
    if (playerX + playerWidth > screen_x)
        playerX = screen_x - playerWidth;
    if (playerY < 0)
    {
        playerY = 0;
        playerVelocityY = 0;
    }
    if (playerY + playerHeight > screen_y)
    {
        playerY = screen_y - playerHeight;
        playerVelocityY = 0;
        playerOnGround = true;
    }

    // Level 2 sliding effect when on ground and not moving
    if (levelStateCurrentLevel == 2 && playerOnGround && !playerIsVacuuming)
    {
        // Check if player is not pressing movement keys
        bool notMoving = !Keyboard::isKeyPressed(Keyboard::Left) &&
                         !Keyboard::isKeyPressed(Keyboard::A) &&
                         !Keyboard::isKeyPressed(Keyboard::Right) &&
                         !Keyboard::isKeyPressed(Keyboard::D);

        if (notMoving)
        {
            // Apply sliding force to the right
            playerX += 0.6f;

            // Check wall collision during slide
            int rightGridX = (int)(playerX + playerWidth) / levelCellSize;
            int gridYTop = (int)(playerY + 10) / levelCellSize;
            int gridYBottom = (int)(playerY + playerHeight - 10) / levelCellSize;

            if (rightGridX < levelWidth && (tileSolid(gridYTop, rightGridX) || tileSolid(gridYBottom, rightGridX)))
            {
                playerX = rightGridX * levelCellSize - playerWidth;
            }

            // Keep within screen bounds
            if (playerX + playerWidth > screen_x)
                playerX = screen_x - playerWidth;
        }
    }

    // Update sprite position and orientation
    playerSprite.setPosition(playerX, playerY);
    if (playerFacingRight)
        orientSpriteFacingRight(playerSprite, playerWidth);
    else
        orientSpriteFacingLeft(playerSprite, playerWidth);
}

// Update all enemies including their AI and shooting
void updateEnemies(int enemyCount, int maxBullets, float deltaTime)
{
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemyActive[i])
            continue;

        if (!enemyIsBeingSucked[i])
        {
            // Ghost behavior: random direction changes
            if (enemyType[i] == TYPE_GHOST)
            {
                if (rand() % 800 < 4)
                {
                    enemyVelocityX[i] = -enemyVelocityX[i];
                    enemyFacingRight[i] = !enemyFacingRight[i];
                }
            }
            // Skeleton behavior: occasional pauses
            else if (enemyType[i] == TYPE_SKELETON)
            {
                if (!enemyPaused[i] && rand() % 1000 < 6)
                {
                    enemyPaused[i] = true;
                    enemyPauseDuration[i] = 0.4f + (rand() % 100) / 100.0f;
                    enemyPauseTimer[i] = 0.f;
                }
                if (enemyPaused[i])
                {
                    enemyPauseTimer[i] += deltaTime;
                    if (enemyPauseTimer[i] >= enemyPauseDuration[i])
                    {
                        enemyPaused[i] = false;
                        if (rand() % 2 == 0)
                            enemyVelocityX[i] = -enemyVelocityX[i];
                    }
                }
            }
            // Invisible Man behavior: teleportation and invisibility
            else if (enemyType[i] == TYPE_INVISIBLE)
            {
                enemyInvisTimer[i] += deltaTime;
                if (enemyInvisTimer[i] >= enemyInvisDuration[i])
                {
                    // Find platform to teleport to
                    int gridYBelow = (int)((enemyY[i] + enemyHeight[i]) / levelCellSize);
                    int centerCol = (int)(enemyX[i] + enemyWidth[i] / 2) / levelCellSize;
                    int leftCol = centerCol, rightCol = centerCol;

                    // Find platform boundaries
                    while (leftCol - 1 >= 0 && levelGrid[gridYBelow][leftCol - 1] == '#')
                        leftCol--;
                    while (rightCol + 1 < levelWidth && levelGrid[gridYBelow][rightCol + 1] == '#')
                        rightCol++;

                    // Teleport to random position on platform
                    if (rightCol > leftCol)
                    {
                        int targetCol = leftCol + (rand() % (rightCol - leftCol + 1));
                        float targetX = targetCol * levelCellSize + (levelCellSize - enemyWidth[i]) / 2.0f;
                        enemyX[i] = targetX;
                        enemyOriginalY[i] = enemyY[i];
                    }

                    // Reset invisibility timer
                    enemyInvisDuration[i] = 2.0f + (rand() % 3);
                    enemyInvisTimer[i] = 0.f;
                    enemyVisible[i] = true;
                }

                // Toggle visibility for blinking effect
                if ((int)(enemyInvisTimer[i] * 4) % 2 == 0)
                    enemyVisible[i] = false;
                else
                    enemyVisible[i] = true;
            }
            // Chelnov behavior: shooting at player
            else if (enemyType[i] == TYPE_CHELNOV)
            {
                // Increment shoot timer
                enemyShootTimer[i] += deltaTime;

                // Check if ready to shoot
                if (!enemyInWindup[i] && enemyShootTimer[i] >= 4.0f)
                {
                    // Reset timer immediately to prevent multiple checks
                    enemyShootTimer[i] = 0.f;

                    // Find platform below enemy
                    int enemyPlatformRow = -1;
                    int enemyFeetY = (int)((enemyY[i] + enemyHeight[i]) / levelCellSize);
                    if (enemyFeetY < levelHeight)
                    {
                        int enemyCenterX = (int)((enemyX[i] + enemyWidth[i] / 2) / levelCellSize);
                        if (enemyCenterX >= 0 && enemyCenterX < levelWidth &&
                            levelGrid[enemyFeetY][enemyCenterX] == '#')
                        {
                            enemyPlatformRow = enemyFeetY;
                        }
                    }

                    // Find platform below player
                    int playerPlatformRow = -1;
                    int playerFeetY = (int)((playerY + playerHeight) / levelCellSize);
                    if (playerFeetY < levelHeight)
                    {
                        int playerCenterX = (int)((playerX + playerWidth / 2) / levelCellSize);
                        if (playerCenterX >= 0 && playerCenterX < levelWidth &&
                            levelGrid[playerFeetY][playerCenterX] == '#')
                        {
                            playerPlatformRow = playerFeetY;
                        }
                    }

                    // Only shoot if on same platform as player
                    if (enemyPlatformRow == playerPlatformRow && enemyPlatformRow != -1)
                    {
                        // Check if facing player
                        bool facingPlayer = (enemyFacingRight[i] && playerX > enemyX[i]) ||
                                            (!enemyFacingRight[i] && playerX < enemyX[i]);

                        if (facingPlayer)
                        {
                            float distance = fabs(playerX - enemyX[i]);
                            if (distance <= 450.0f) // Shooting range
                            {
                                int enemyCol = (int)((enemyX[i] + enemyWidth[i] / 2) / levelCellSize);
                                int playerCol = (int)((playerX + playerWidth / 2) / levelCellSize);

                                // Check if line of sight is clear
                                if (clearLineBetween(enemyPlatformRow, enemyCol, playerCol))
                                {
                                    // Start windup for shooting
                                    enemyFacingRight[i] = (playerX > enemyX[i]);
                                    enemyInWindup[i] = true;
                                    enemyWindupTimer[i] = 0.f;
                                    enemyCanBeCaptured[i] = false;
                                    enemyVelocityX[i] = 0.f;
                                }
                            }
                        }
                    }
                }

                // Handle windup and shooting
                if (enemyInWindup[i])
                {
                    enemyWindupTimer[i] += deltaTime;
                    if (enemyWindupTimer[i] >= 1.0f)
                    {
                        // Find available bullet slot and fire
                        for (int b = 0; b < maxBullets; b++)
                        {
                            if (!bulletActive[b])
                            {
                                bulletActive[b] = true;
                                bulletX[b] = enemyX[i] + (enemyFacingRight[i] ? enemyWidth[i] : 0);
                                bulletY[b] = enemyY[i] + enemyHeight[i] / 2;
                                float bulletSpeed = 5.5f;

                                if (!bulletTexture[b].loadFromFile("Assets/bullet.png"))
                                {
                                    bulletSprite[b].setTextureRect(sf::IntRect(0, 0, 10, 10));
                                }
                                else
                                {
                                    bulletSprite[b].setTexture(bulletTexture[b]);
                                    bulletSprite[b].setScale(0.5f, 0.5f);
                                }

                                bulletVelocityY[b] = 0.f;
                                bulletVelocityX[b] = enemyFacingRight[i] ? bulletSpeed : -bulletSpeed;
                                bulletDirection[b] = enemyFacingRight[i] ? 0 : 2;
                                break;
                            }
                        }

                        // End windup
                        enemyInWindup[i] = false;
                        enemyCanBeCaptured[i] = true;
                        enemyShootTimer[i] = 0.f; // Reset timer after shooting
                        enemyVelocityX[i] = (enemyFacingRight[i] ? 1.4f : -1.4f);
                    }
                }
            }

            // Update physics (don't move during windup or pause)
            if (!enemyPaused[i] && !enemyIsBeingSucked[i] && !enemyInWindup[i])
            {
                updateEnemyPhysics(i);
            }
            else
            {
                enemySprite[i].setPosition(enemyX[i], enemyY[i]);
                if (enemyFacingRight[i])
                    orientSpriteFacingRight(enemySprite[i], enemyWidth[i]);
                else
                    orientSpriteFacingLeft(enemySprite[i], enemyWidth[i]);
            }
        }

        // Handle invisibility visual effect
        if (enemyType[i] == TYPE_INVISIBLE)
        {
            if (enemyVisible[i])
                enemySprite[i].setColor(Color(255, 255, 255, 255));
            else
                enemySprite[i].setColor(Color(255, 255, 255, 90));
        }
        else
        {
            enemySprite[i].setColor(Color::White);
        }
    }
}

// Update bullet positions and check collisions
void updateBullets(int maxBullets, float deltaTime)
{
    for (int i = 0; i < maxBullets; i++)
    {
        if (!bulletActive[i])
            continue;

        // Move bullet
        bulletX[i] += bulletVelocityX[i];
        bulletY[i] += bulletVelocityY[i];

        // Check collision with level geometry
        int gridX = (int)bulletX[i] / levelCellSize;
        int gridY = (int)bulletY[i] / levelCellSize;

        if (gridX >= 0 && gridX < levelWidth && gridY >= 0 && gridY < levelHeight)
        {
            if (levelGrid[gridY][gridX] == '#')
            {
                bulletActive[i] = false;
                continue;
            }
        }

        // Deactivate if outside screen bounds
        if (bulletX[i] < 0 || bulletX[i] > screen_x || bulletY[i] < 0 || bulletY[i] > screen_y)
        {
            bulletActive[i] = false;
        }

        // Update sprite position
        bulletSprite[i].setPosition(bulletX[i] - bulletWidth[i] / 2, bulletY[i] - bulletHeight[i] / 2);
    }
}

// Check rectangle collision between two objects
bool checkCollision(float x1, float y1, int w1, int h1, float x2, float y2, int w2, int h2)
{
    return (x1 < x2 + w2 && x1 + w1 > x2 && y1 < y2 + h2 && y1 + h1 > y2);
}

// Check circle collision between two objects
bool checkCircleCollision(float x1, float y1, float r1, float x2, float y2, float r2)
{
    float dx = x1 - x2;
    float dy = y1 - y2;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < (r1 + r2);
}

// Handle player keyboard input
void handlePlayerInput()
{
    if (!playerIsAlive)
        return;

    // Vacuum activation
    if (Keyboard::isKeyPressed(Keyboard::Space))
    {
        if (!vacuumIsFull)
        {
            playerIsVacuuming = true;
            vacuumIsActive = true;

            // Set vacuum direction based on WASD keys
            if (Keyboard::isKeyPressed(Keyboard::D))
                vacuumAngle = 0;
            else if (Keyboard::isKeyPressed(Keyboard::A))
                vacuumAngle = 180;
            else if (Keyboard::isKeyPressed(Keyboard::W))
                vacuumAngle = 270;
            else if (Keyboard::isKeyPressed(Keyboard::S))
                vacuumAngle = 90;

            playerVelocityX = 0; // Stop movement while vacuuming
        }
    }
    else
    {
        playerIsVacuuming = false;
        vacuumIsActive = false;

        float moveSpeed = playerSpeed;

        // Movement controls
        if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
        {
            playerVelocityX = -moveSpeed;
            playerFacingRight = false;
        }
        else if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
        {
            playerVelocityX = moveSpeed;
            playerFacingRight = true;
        }
        else
        {
            playerVelocityX = 0;
        }

        // Jumping
        if ((Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) && playerOnGround)
        {
            playerVelocityY = playerJumpStrength;
            playerOnGround = false;
            playerIsJumping = true;
        }
    }
}

// Apply vacuum suction effect to nearby enemies
void applyVacuumEffect(int enemyCount, float deltaTime)
{
    if (vacuumIsFull || !vacuumIsActive)
        return;

    // Player center position
    float px = playerX + playerWidth / 2.0f;
    float py = playerY + playerHeight / 2.0f;

    // Suction direction based on vacuum angle
    float angleRad = vacuumAngle * 3.14159f / 180.0f;
    float suctionDirX = cos(angleRad);
    float suctionDirY = sin(angleRad);

    // Reset suction state for all enemies
    for (int i = 0; i < enemyCount; i++)
    {
        enemyIsBeingSucked[i] = false;
    }

    // Apply suction to each enemy
    for (int i = 0; i < enemyCount; i++)
    {
        if (!enemyActive[i] || !enemyCanBeCaptured[i])
            continue;

        // Enemy center position
        float ex = enemyX[i] + enemyWidth[i] / 2.0f;
        float ey = enemyY[i] + enemyHeight[i] / 2.0f;
        float dx = ex - px;
        float dy = ey - py;

        // Check if enemy is within range
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > vacuumRange)
            continue;

        // Check if enemy is within vacuum cone
        float angleToEnemy = atan2(dy, dx) * 180.0f / 3.14159f;
        if (angleToEnemy < 0)
            angleToEnemy += 360;

        float angleDiff = fabs(angleToEnemy - vacuumAngle);
        if (angleDiff > 180)
            angleDiff = 360 - angleDiff;

        if (angleDiff > 45) // 45 degree cone
            continue;

        // Apply suction force
        float pull = vacuumSuctionForce * deltaTime * 60.f;
        float nx = (px - ex) / dist;
        float ny = (py - ey) / dist;
        enemyX[i] += nx * pull;
        enemyY[i] += ny * pull;
        enemySprite[i].setPosition(enemyX[i], enemyY[i]);

        enemyIsBeingSucked[i] = true;

        // Capture enemy if close enough
        if (dist <= vacuumCaptureDistance)
        {
            // Find empty slot in vacuum
            for (int slot = 0; slot < vacuumMaxCapacity; slot++)
            {
                if (vacuumCapturedEnemyTypes[slot] == -1)
                {
                    vacuumCapturedEnemyTypes[slot] = enemyType[i];

                    // Award points for capture
                    int capturePoints = getEnemyCapturePoints(enemyType[i]);
                    playerComboStreak++;
                    addPoints(capturePoints, "Enemy Captured");

                    // Deactivate enemy
                    enemyActive[i] = false;
                    enemyVisible[i] = false;
                    enemyPaused[i] = true;
                    enemyVelocityX[i] = 0;
                    enemyVelocityY[i] = 0;
                    enemyIsBeingSucked[i] = false;

                    // Update vacuum state
                    vacuumCurrentCount++;
                    if (vacuumCurrentCount >= vacuumMaxCapacity)
                    {
                        vacuumIsFull = true;
                    }
                    break;
                }
            }
        }
    }
}

// Apply vacuum effect to minions (boss level) - same logic as level 2
void applyVacuumEffectToMinions(int minionCount, float deltaTime)
{
    if (vacuumIsFull || !vacuumIsActive)
        return;

    // Player center position
    float px = playerX + playerWidth / 2.0f;
    float py = playerY + playerHeight / 2.0f;

    // Suction direction based on vacuum angle
    float angleRad = vacuumAngle * 3.14159f / 180.0f;
    float suctionDirX = cos(angleRad);
    float suctionDirY = sin(angleRad);

    // Apply suction to each minion
    for (int i = 0; i < minionCount; i++)
    {
        if (!minionActive[i] || minionCaptured[i])
            continue;

        // Minion center position
        float mx = minionX[i] + minionWidth / 2.0f;
        float my = minionY[i] + minionHeight / 2.0f;
        float dx = mx - px;
        float dy = my - py;

        // Check if minion is within range
        float dist = sqrt(dx * dx + dy * dy);
        if (dist > vacuumRange)
            continue;

        // Check if minion is within vacuum cone
        float angleToMinion = atan2(dy, dx) * 180.0f / 3.14159f;
        if (angleToMinion < 0)
            angleToMinion += 360;

        float angleDiff = fabs(angleToMinion - vacuumAngle);
        if (angleDiff > 180)
            angleDiff = 360 - angleDiff;

        if (angleDiff > 45) // 45 degree cone
            continue;

        // Apply suction force
        float pull = vacuumSuctionForce * deltaTime * 60.f;
        float nx = (px - mx) / dist;
        float ny = (py - my) / dist;
        minionX[i] += nx * pull;
        minionY[i] += ny * pull;

        // Capture minion if close enough
        if (dist <= vacuumCaptureDistance)
        {
            // Find empty slot in vacuum
            for (int slot = 0; slot < vacuumMaxCapacity; slot++)
            {
                if (vacuumCapturedEnemyTypes[slot] == -1)
                {
                    // Store minion as TYPE_SKELETON for vacuum (or use a minion type)
                    vacuumCapturedEnemyTypes[slot] = TYPE_SKELETON; // Use skeleton type for minions

                    // Award points for capture
                    int capturePoints = 75; // Same as skeleton
                    playerComboStreak++;
                    addPoints(capturePoints, "Minion Captured");

                    // Mark minion as captured
                    minionCaptured[i] = true;
                    minionActive[i] = false;

                    // Update vacuum state
                    vacuumCurrentCount++;
                    if (vacuumCurrentCount >= vacuumMaxCapacity)
                    {
                        vacuumIsFull = true;
                    }
                    break;
                }
            }
        }
    }
}

// Throw a single captured enemy as projectile
void throwSingleEnemy(int maxThrown, int direction)
{
    if (vacuumCurrentCount <= 0)
        return;

    // Find first captured enemy
    int enemyType = -1;
    int slotIndex = -1;
    for (int i = 0; i < vacuumMaxCapacity; i++)
    {
        if (vacuumCapturedEnemyTypes[i] != -1)
        {
            enemyType = vacuumCapturedEnemyTypes[i];
            slotIndex = i;
            break;
        }
    }

    if (enemyType == -1)
        return;

    // Find available thrown enemy slot
    for (int i = 0; i < maxThrown; i++)
    {
        if (!thrownEnemyActive[i])
        {
            float startX = playerX + playerWidth / 2;
            float startY = playerY + playerHeight / 2;
            float vx = 0, vy = 0;

            // Set velocity based on direction
            if (direction == -1) // Default: throw in facing direction
            {
                vx = playerFacingRight ? 12.0f : -12.0f;
                vy = -5.0f;
                startX = playerX + (playerFacingRight ? playerWidth + 10 : -20);
                startY = playerY + 20;
            }
            else
            {
                switch (direction)
                {
                case 0: // Right
                    vx = 12.0f;
                    vy = 0;
                    break;
                case 1: // Up
                    vx = 0;
                    vy = -12.0f;
                    startY -= 20;
                    break;
                case 2: // Left
                    vx = -12.0f;
                    vy = 0;
                    break;
                case 3: // Down
                    vx = 0;
                    vy = 12.0f;
                    startY += 20;
                    break;
                }
            }

            // Initialize thrown enemy
            initThrownEnemy(i, enemyType, startX, startY, vx, vy);

            // Remove from vacuum
            vacuumCapturedEnemyTypes[slotIndex] = -1;
            vacuumCurrentCount--;
            vacuumIsFull = false;
            break;
        }
    }
}

// Throw all captured enemies as a cluster
void throwAllEnemies()
{
    if (vacuumCurrentCount <= 0)
        return;

    // Count captured enemies
    int count = 0;
    for (int i = 0; i < vacuumMaxCapacity; i++)
    {
        if (vacuumCapturedEnemyTypes[i] != -1)
        {
            count++;
        }
    }

    if (count == 0)
        return;

    // Initialize cluster
    thrownClusterActive = true;
    thrownClusterEnemyCount = 0;

    // Transfer enemies from vacuum to cluster
    for (int i = 0; i < vacuumMaxCapacity; i++)
    {
        if (vacuumCapturedEnemyTypes[i] != -1)
        {
            thrownClusterEnemyTypes[thrownClusterEnemyCount] = vacuumCapturedEnemyTypes[i];
            thrownClusterEnemyCount++;
            vacuumCapturedEnemyTypes[i] = -1;
        }
    }

    // Set cluster position and velocity
    thrownClusterX = playerX + playerWidth / 2;
    thrownClusterY = playerY + playerHeight / 2;
    thrownClusterVelocityX = playerFacingRight ? 10.0f : -10.0f;
    thrownClusterVelocityY = -3.0f;
    thrownClusterRadius = 20.0f + (count * 8.0f);

    // Clear vacuum
    vacuumCurrentCount = 0;
    vacuumIsFull = false;
}

// Update thrown enemy projectiles
void updateThrownEnemies(int maxThrown, int enemyCount, float deltaTime)
{
    for (int i = 0; i < maxThrown; i++)
    {
        if (!thrownEnemyActive[i])
            continue;

        // Apply gravity
        thrownEnemyVelocityY[i] += physicsGravity;

        // Update position
        thrownEnemyX[i] += thrownEnemyVelocityX[i];
        thrownEnemyY[i] += thrownEnemyVelocityY[i];

        // Deactivate if outside horizontal bounds
        if (thrownEnemyX[i] < 0 || thrownEnemyX[i] > screen_x - thrownEnemyWidth[i])
        {
            thrownEnemyActive[i] = false;
            continue;
        }

        // Bounce off bottom of screen
        if (thrownEnemyY[i] + thrownEnemyHeight[i] >= screen_y)
        {
            thrownEnemyY[i] = screen_y - thrownEnemyHeight[i];
            thrownEnemyVelocityY[i] = -thrownEnemyVelocityY[i] * physicsBounceFactor;
            thrownEnemyIsRolling[i] = true;
        }

        // Bounce off top of screen
        if (thrownEnemyY[i] < 0)
        {
            thrownEnemyY[i] = 0;
            thrownEnemyVelocityY[i] = -thrownEnemyVelocityY[i] * physicsBounceFactor;
        }

        // Check collision with level geometry
        int gridX = (int)(thrownEnemyX[i] + thrownEnemyWidth[i] / 2) / levelCellSize;
        int gridY = (int)(thrownEnemyY[i] + thrownEnemyHeight[i] / 2) / levelCellSize;

        int gridXLeft = (int)thrownEnemyX[i] / levelCellSize;
        int gridXRight = (int)(thrownEnemyX[i] + thrownEnemyWidth[i]) / levelCellSize;

        if (gridXLeft >= 0 && gridXLeft < levelWidth && gridXRight >= 0 && gridXRight < levelWidth)
        {
            if (tileSolid(gridY, gridXLeft) || tileSolid(gridY, gridXRight))
            {
                thrownEnemyActive[i] = false;
                continue;
            }
        }

        // Check collision with platform below
        int gridYTop = (int)thrownEnemyY[i] / levelCellSize;
        int gridYBottom = (int)(thrownEnemyY[i] + thrownEnemyHeight[i]) / levelCellSize;

        if (gridYBottom >= 0 && gridYBottom < levelHeight)
        {
            if (tileSolid(gridYBottom, gridX))
            {
                thrownEnemyY[i] = gridYBottom * levelCellSize - thrownEnemyHeight[i];
                thrownEnemyVelocityY[i] = -thrownEnemyVelocityY[i] * physicsBounceFactor;
                thrownEnemyIsRolling[i] = true;
            }
        }

        // Check collision with platform above
        if (gridYTop >= 0 && gridYTop < levelHeight)
        {
            if (tileSolid(gridYTop, gridX))
            {
                thrownEnemyY[i] = (gridYTop + 1) * levelCellSize;
                thrownEnemyVelocityY[i] = -thrownEnemyVelocityY[i] * physicsBounceFactor;
            }
        }

        // Check collision with enemies
        for (int e = 0; e < enemyCount; e++)
        {
            if (enemyActive[e] &&
                checkCollision(thrownEnemyX[i], thrownEnemyY[i],
                               thrownEnemyWidth[i], thrownEnemyHeight[i],
                               enemyX[e], enemyY[e],
                               enemyWidth[e], enemyHeight[e]))
            {
                // Check for aerial defeat bonus
                bool aerialDefeat = (thrownEnemyVelocityY[i] < 0) || (playerY + playerHeight < enemyY[e]);
                if (aerialDefeat)
                {
                    playerAerialDefeats++;
                    addPoints(100, "Aerial Defeat Bonus");
                }

                // Award points for defeating enemy
                int defeatPoints = getEnemyDefeatPoints(enemyType[e]);
                playerComboStreak++;
                addPoints(defeatPoints, "Enemy Defeated by Projectile");

                // Deactivate both thrown enemy and hit enemy
                enemyActive[e] = false;
                thrownEnemyActive[i] = false;
                break;
            }
        }

        // Update sprite position
        thrownEnemySprite[i].setPosition(thrownEnemyX[i], thrownEnemyY[i]);
    }
}

// Update thrown enemy cluster
void updateThrownEnemyCluster(int enemyCount, float deltaTime)
{
    if (!thrownClusterActive || thrownClusterEnemyCount <= 0)
        return;

    // Rotate cluster
    thrownClusterRotationAngle += thrownClusterRotationSpeed * deltaTime;
    if (thrownClusterRotationAngle > 360.0f)
        thrownClusterRotationAngle -= 360.0f;

    // Apply gravity
    thrownClusterVelocityY += physicsGravity * 0.5f;

    // Calculate new position
    float newX = thrownClusterX + thrownClusterVelocityX;
    float newY = thrownClusterY + thrownClusterVelocityY;

    // Bounce off screen edges
    if (newX - thrownClusterRadius < 0 || newX + thrownClusterRadius > screen_x)
    {
        if (thrownClusterEnemyCount >= 3)
        {
            addPoints(300, "Vacuum Burst Bonus");
        }

        if (thrownClusterEnemyCount > 1)
        {
            thrownClusterEnemyCount--;
            thrownClusterRadius = 20.0f + (thrownClusterEnemyCount * 8.0f);
            newX = thrownClusterX;
            thrownClusterVelocityX = -thrownClusterVelocityX * physicsBounceFactor;
        }
        else
        {
            thrownClusterActive = false;
            thrownClusterEnemyCount = 0;
            return;
        }
    }

    // Bounce off top and bottom
    if (newY - thrownClusterRadius < 0)
    {
        newY = thrownClusterRadius;
        thrownClusterVelocityY = -thrownClusterVelocityY * physicsBounceFactor;
    }
    else if (newY + thrownClusterRadius > screen_y)
    {
        newY = screen_y - thrownClusterRadius;
        thrownClusterVelocityY = -thrownClusterVelocityY * physicsBounceFactor;
    }

    // Check collision with level geometry
    int centerGridX = (int)(newX / levelCellSize);
    int centerGridY = (int)(newY / levelCellSize);

    for (int y = max(0, centerGridY - 2); y <= min(levelHeight - 1, centerGridY + 2); y++)
    {
        for (int x = max(0, centerGridX - 2); x <= min(levelWidth - 1, centerGridX + 2); x++)
        {
            if (levelGrid[y][x] == '#')
            {
                float tileX = x * levelCellSize + levelCellSize / 2.0f;
                float tileY = y * levelCellSize + levelCellSize / 2.0f;

                float dx = newX - tileX;
                float dy = newY - tileY;
                float distance = sqrt(dx * dx + dy * dy);

                if (distance < (thrownClusterRadius + levelCellSize / 2.0f))
                {
                    if (abs(dx) > abs(dy)) // Horizontal collision
                    {
                        if (thrownClusterEnemyCount > 1)
                        {
                            thrownClusterEnemyCount--;
                            thrownClusterRadius = 20.0f + (thrownClusterEnemyCount * 8.0f);
                            newX = thrownClusterX;
                            thrownClusterVelocityX = -thrownClusterVelocityX * physicsBounceFactor;
                        }
                        else
                        {
                            thrownClusterActive = false;
                            thrownClusterEnemyCount = 0;
                            return;
                        }
                    }
                    else // Vertical collision
                    {
                        thrownClusterVelocityY = -thrownClusterVelocityY * physicsBounceFactor;
                        newY = thrownClusterY;
                    }
                }
            }
        }
    }

    // Update cluster position
    thrownClusterX = newX;
    thrownClusterY = newY;

    // Check collision with enemies
    for (int i = 0; i < enemyCount; i++)
    {
        if (enemyActive[i] && enemyCanBeCaptured[i] && !enemyIsBeingSucked[i])
        {
            float enemyCenterX = enemyX[i] + enemyWidth[i] / 2.0f;
            float enemyCenterY = enemyY[i] + enemyHeight[i] / 2.0f;
            float enemyRadius = max(enemyWidth[i], enemyHeight[i]) / 2.0f;

            if (checkCircleCollision(thrownClusterX, thrownClusterY, thrownClusterRadius,
                                     enemyCenterX, enemyCenterY, enemyRadius))
            {
                // Award points for defeating enemy
                int defeatPoints = getEnemyDefeatPoints(enemyType[i]);
                playerComboStreak++;

                if (thrownClusterEnemyCount >= 3)
                {
                    addPoints(300, "Vacuum Burst Bonus");
                }
                addPoints(defeatPoints, "Enemy Defeated by Cluster");

                enemyActive[i] = false;

                // Reduce cluster size
                if (thrownClusterEnemyCount > 1)
                {
                    thrownClusterEnemyCount--;
                    thrownClusterRadius = 20.0f + (thrownClusterEnemyCount * 8.0f);
                }
                else
                {
                    thrownClusterActive = false;
                    thrownClusterEnemyCount = 0;
                    break;
                }
            }
        }
    }

    // Deactivate cluster if empty
    if (thrownClusterEnemyCount <= 0)
    {
        thrownClusterActive = false;
    }
}

// Update power-ups
void updatePowerUps(int powerupCount, float deltaTime)
{
    for (int i = 0; i < powerupCount; i++)
    {
        if (!powerUpActive[i])
            continue;

        // Floating animation
        powerUpFloatTimer[i] += deltaTime;
        powerUpY[i] += sin(powerUpFloatTimer[i] * 3.0f) * 0.5f;

        // Check collision with player
        if (checkCollision(playerX, playerY, playerWidth, playerHeight,
                           powerUpX[i], powerUpY[i], powerUpWidth[i], powerUpHeight[i]))
        {
            // Apply power-up effect based on type
            switch (powerUpType[i])
            {
            case POWERUP_SPEED:
                playerSpeed = playerBaseSpeed * 2.0f;
                powerUpTimer[i] = 0;
                addPoints(100, "Speed Power-up Collected");
                std::cout << "Speed doubled for 10 seconds!" << std::endl;
                break;

            case POWERUP_RANGE:
                vacuumRange = vacuumBaseRange * 1.5f;
                powerUpTimer[i] = 0;
                addPoints(100, "Range Power-up Collected");
                std::cout << "Vacuum range increased for 10 seconds!" << std::endl;
                break;

            case POWERUP_POWER:
                vacuumSuctionForce = vacuumBaseSuctionForce * 1.5f;
                powerUpTimer[i] = 0;
                addPoints(100, "Power Power-up Collected");
                std::cout << "Vacuum power increased for 10 seconds!" << std::endl;
                break;

            case POWERUP_LIFE:
                playerLives++;
                addPoints(200, "Extra Life Collected");
                std::cout << "Extra life gained!" << std::endl;
                powerUpActive[i] = false;
                break;
            }

            // Reset timer for timed power-ups
            if (powerUpType[i] != POWERUP_LIFE)
                powerUpTimer[i] = 0;
            else
                powerUpActive[i] = false;
        }

        // Handle power-up duration
        if (powerUpActive[i] && powerUpType[i] != POWERUP_LIFE)
        {
            powerUpTimer[i] += deltaTime;
            if (powerUpTimer[i] >= powerUpDuration[i])
            {
                // Remove power-up effect
                switch (powerUpType[i])
                {
                case POWERUP_SPEED:
                    playerSpeed = playerBaseSpeed;
                    std::cout << "Speed power-up expired!" << std::endl;
                    break;

                case POWERUP_RANGE:
                    vacuumRange = vacuumBaseRange;
                    std::cout << "Range power-up expired!" << std::endl;
                    break;

                case POWERUP_POWER:
                    vacuumSuctionForce = vacuumBaseSuctionForce;
                    std::cout << "Power power-up expired!" << std::endl;
                    break;
                }
                powerUpActive[i] = false;
            }
        }
    }
}

// Draw thrown enemy cluster on screen
void drawThrownEnemyCluster(RenderWindow &window)
{
    if (!thrownClusterActive || thrownClusterEnemyCount <= 0)
        return;

    // Calculate angle between enemies in cluster
    float angleStep = 360.0f / thrownClusterEnemyCount;

    // Draw each enemy in the cluster
    for (int i = 0; i < thrownClusterEnemyCount; i++)
    {
        if (thrownClusterEnemyTypes[i] == -1)
            continue;

        // Calculate position in circular formation
        float angle = (thrownClusterRotationAngle + i * angleStep) * 3.14159f / 180.0f;
        float offsetX = cos(angle) * thrownClusterRadius;
        float offsetY = sin(angle) * thrownClusterRadius;

        // Create and draw sprite
        Sprite tempSprite;
        tempSprite.setTexture(thrownClusterThrowTextures[thrownClusterEnemyTypes[i]]);
        tempSprite.setPosition(thrownClusterX + offsetX - 15, thrownClusterY + offsetY - 15);
        tempSprite.setScale(0.5f, 0.5f);
        tempSprite.setColor(Color(255, 255, 255, 200));

        window.draw(tempSprite);
    }

    // Draw cluster outline
    CircleShape outline(thrownClusterRadius);
    outline.setFillColor(Color::Transparent);
    outline.setOutlineColor(Color(255, 255, 255, 100));
    outline.setOutlineThickness(2.0f);
    outline.setPosition(thrownClusterX - thrownClusterRadius, thrownClusterY - thrownClusterRadius);
    window.draw(outline);
}

// Draw captured enemies orbiting player
void drawCapturedEnemies(RenderWindow &window)
{
    // Load throw textures (only once)
    static Texture throwTextures[4];
    static bool texturesLoaded = false;

    if (!texturesLoaded)
    {
        throwTextures[TYPE_GHOST].loadFromFile("Assets/ghostThrow.png");
        throwTextures[TYPE_SKELETON].loadFromFile("Assets/skeltonThrow.png");
        throwTextures[TYPE_INVISIBLE].loadFromFile("Assets/invisibleThrow.png");
        throwTextures[TYPE_CHELNOV].loadFromFile("Assets/chelnovThrow.png");
        texturesLoaded = true;
    }

    // Draw each captured enemy orbiting player
    int displayCount = 0;
    for (int i = 0; i < vacuumMaxCapacity; i++)
    {
        if (vacuumCapturedEnemyTypes[i] != -1)
        {
            Sprite throwSprite;
            throwSprite.setTexture(throwTextures[vacuumCapturedEnemyTypes[i]]);

            // Calculate orbit position
            float angle = (displayCount * 120.0f) * 3.14159f / 180.0f;
            float radius = 30.0f;
            float offsetX = cos(angle) * radius;
            float offsetY = sin(angle) * radius;

            // Set sprite position and properties
            throwSprite.setPosition(playerX + playerWidth / 2 + offsetX - 15,
                                    playerY + playerHeight / 2 + offsetY - 15);
            throwSprite.setScale(0.6f, 0.6f);
            throwSprite.setColor(Color(255, 255, 255, 180));

            window.draw(throwSprite);
            displayCount++;
        }
    }
}

// Clean up level grid memory
void cleanupLevelGrid()
{
    for (int i = 0; i < levelHeight; i++)
    {
        delete[] levelGrid[i];
    }
    delete[] levelGrid;
}

// ============================================================================
// BOSS LEVEL FUNCTIONS
// ============================================================================

// Initialize Boss Level Grid (21x27 with 42px cells)
void initBossLevel(char**& level) {
    bossLevelRows = 21;
    bossLevelCols = 27;
    bossLevelCellSize = 42;
    bossLevelIsActive = true;

    // Allocate level array
    level = new char*[bossLevelRows];
    for (int i = 0; i < bossLevelRows; i++) {
        level[i] = new char[bossLevelCols];
        for (int j = 0; j < bossLevelCols; j++) {
            level[i][j] = ' ';
        }
    }
}

// Generate Boss Level Layout
void generateBossLevel(char** level, int rows, int cols) {
    // Clear level
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            level[i][j] = ' ';
        }
    }

    // Walls (left and right)
    for (int i = 0; i < rows; i++) {
        level[i][0] = '#';
        level[i][cols-1] = '#';
    }

    // Floor (bottom row) and Ceiling
    for (int j = 0; j < cols; j++) {
        level[rows-1][j] = '#';
        level[0][j] = '#';
    }

    // Bottom platforms (row 17)
    for (int j = 3; j < 10; j++) level[17][j] = '#';
    for (int j = 17; j < 24; j++) level[17][j] = '#';

    // Middle-low platforms (row 15)
    for (int j = 2; j < 5; j++) level[15][j] = '#';
    for (int j = 22; j < 25; j++) level[15][j] = '#';

    // Middle platforms (row 13)
    for (int j = 5; j < 12; j++) level[13][j] = '#';
    for (int j = 15; j < 22; j++) level[13][j] = '#';

    // Middle-high platforms (row 11)
    for (int j = 3; j < 8; j++) level[11][j] = '#';
    for (int j = 19; j < 24; j++) level[11][j] = '#';

    // Upper platform (row 9)
    for (int j = 8; j < 19; j++) level[9][j] = '#';

    // High platforms (row 7)
    for (int j = 5; j < 10; j++) level[7][j] = '#';
    for (int j = 17; j < 22; j++) level[7][j] = '#';

    // Top platform (row 5)
    for (int j = 10; j < 17; j++) level[5][j] = '#';

    // Small side platforms
    level[19][2] = '#';
    level[19][24] = '#';
}

// Cleanup Boss Level
void cleanupBossLevel(char** level, int rows) {
    if (level != nullptr) {
        for (int i = 0; i < rows; i++) {
            delete[] level[i];
        }
        delete[] level;
        level = nullptr;
    }
}

// Initialize Boss
void initBoss() {
    bossX = 575.0f;
    bossY = 150.0f;
    bossHealth = 10;
    bossMaxHealth = 10;
    bossIsAngry = false;
    bossDefeated = false;
    bossMoveSpeed = 30.0f;
    bossMovingUp = true;
    waterRising = false;
    waterLevel = 896.0f;

    // Load textures
    if (!bossTextureNormal.loadFromFile("Assets/boss_octopus.png")) {
        std::cout << "Warning: Could not load boss_octopus.png - creating placeholder" << std::endl;
        Image img;
        img.create(200, 180, Color(200, 0, 200));
        bossTextureNormal.loadFromImage(img);
    }

    if (!bossTextureAngry.loadFromFile("Assets/boss_octopus_angry.png")) {
        std::cout << "Warning: Could not load boss_octopus_angry.png - using red version" << std::endl;
        Image img;
        img.create(200, 180, Color(255, 0, 0));
        bossTextureAngry.loadFromImage(img);
    }

    bossSprite.setTexture(bossTextureNormal);
    bossSprite.setOrigin(100, 90);
    bossSprite.setPosition(bossX, bossY);
    bossSpawnClock.restart();
}

// Update Boss
// Update Boss Phase (Balanced System)
void updateBossPhase(int bossHealth, bool& bossIsAngry) {
    // Phase transition at 2 HP
    if (bossHealth <= 2 && !bossIsAngry) {
        bossIsAngry = true;
        bossPhase = 2;
        bossSprite.setTexture(bossTextureAngry);
        
        // ANGRY MODE CHANGES:
        minionsPerWave = 3;              // More minions per wave
        minionWaveInterval = 4.0f;       // Faster waves
        maxMinionLimit = 8;              // Higher limit
        tentacleSpawnInterval = 3.5f;    // More tentacles
        waterRising = true;
        
        std::cout << "\n========================================" << std::endl;
        std::cout << "   BOSS ANGRY MODE ACTIVATED!" << std::endl;
        std::cout << "   Water Rising! Minions Following!" << std::endl;
        std::cout << "========================================\n" << std::endl;
    } else if (bossHealth > 2) {
        bossPhase = 1;
        // Phase 1 settings
        minionsPerWave = 2;
        minionWaveInterval = 5.0f;
        maxMinionLimit = 6;
        tentacleSpawnInterval = 4.5f;
    }
}

// Update Boss (Movement and Phase)
void updateBoss(float deltaTime) {
    if (bossDefeated) return;

    // Update boss phase based on health
    updateBossPhase(bossHealth, bossIsAngry);

    // Vertical movement (floating)
    if (bossMovingUp) {
        bossY -= bossMoveSpeed * deltaTime;
        if (bossY < 100) bossMovingUp = false;
    } else {
        bossY += bossMoveSpeed * deltaTime;
        if (bossY > 250) bossMovingUp = true;
    }

    // If angry, float with water
    if (bossIsAngry && waterRising) {
        if (waterLevel < 896.0f) {
            bossY = waterLevel - 200;
        }
    }

    bossSprite.setPosition(bossX, bossY);
}

// Draw Boss
void drawBoss(RenderWindow& window, Font& font) {
    if (bossDefeated) return;

    window.draw(bossSprite);

    // Health bar
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    float barX = bossX - barWidth / 2;
    float barY = bossY - 120;

    RectangleShape healthBg(Vector2f(barWidth, barHeight));
    healthBg.setPosition(barX, barY);
    healthBg.setFillColor(Color::Red);
    window.draw(healthBg);

    float healthPercent = (float)bossHealth / (float)bossMaxHealth;
    RectangleShape healthBar(Vector2f(barWidth * healthPercent, barHeight));
    healthBar.setPosition(barX, barY);
    healthBar.setFillColor(Color::Green);
    window.draw(healthBar);

    // Health text
    Text healthText(to_string(bossHealth) + "/" + to_string(bossMaxHealth), font, 16);
    healthText.setFillColor(Color::White);
    healthText.setPosition(barX + barWidth / 2 - healthText.getLocalBounds().width / 2, barY - 2);
    window.draw(healthText);
}

// Spawn Minion Wave (Balanced Wave-Based System)
void spawnMinionWave() {
    if (bossDefeated) return;
    
    // Count current alive minions
    currentAliveMinions = 0;
    for (int i = 0; i < minionCount; i++) {
        if (minionActive[i] && !minionCaptured[i]) currentAliveMinions++;
    }
    
    // DON'T SPAWN if at max limit (prevents spam)
    if (currentAliveMinions >= maxMinionLimit) {
        return;
    }
    
    // Calculate how many minions to spawn this wave
    int spawnCount = minionsPerWave;
    if (currentAliveMinions + spawnCount > maxMinionLimit) {
        spawnCount = maxMinionLimit - currentAliveMinions;
    }
    
    if (spawnCount <= 0) return;
    
    // Allocate new arrays (resize)
    int newTotal = minionCount + spawnCount;
    float* newX = new float[newTotal];
    float* newY = new float[newTotal];
    float* newVelX = new float[newTotal];
    float* newVelY = new float[newTotal];
    int* newHealth = new int[newTotal];
    bool* newActive = new bool[newTotal];
    bool* newFollow = new bool[newTotal];
    bool* newCaptured = new bool[newTotal];
    bool* newRolling = new bool[newTotal];
    
    // Copy existing minions
    for (int i = 0; i < minionCount; i++) {
        newX[i] = minionX[i];
        newY[i] = minionY[i];
        newVelX[i] = minionVelX[i];
        newVelY[i] = minionVelY[i];
        newHealth[i] = minionHealth[i];
        newActive[i] = minionActive[i];
        newFollow[i] = minionFollowMode[i];
        newCaptured[i] = minionCaptured[i];
        newRolling[i] = minionIsRolling[i];
    }
    
    // Delete old arrays
    if (minionCount > 0) {
        delete[] minionX;
        delete[] minionY;
        delete[] minionVelX;
        delete[] minionVelY;
        delete[] minionHealth;
        delete[] minionActive;
        delete[] minionFollowMode;
        delete[] minionCaptured;
        delete[] minionIsRolling;
    }
    
    // Assign new arrays
    minionX = newX;
    minionY = newY;
    minionVelX = newVelX;
    minionVelY = newVelY;
    minionHealth = newHealth;
    minionActive = newActive;
    minionFollowMode = newFollow;
    minionCaptured = newCaptured;
    minionIsRolling = newRolling;
    
    // Initialize new minions
    for (int i = minionCount; i < newTotal; i++) {
        // Spawn near boss, slightly spread out
        int offset = (i - minionCount) * 80 - 40;
        minionX[i] = bossX + offset;
        minionY[i] = bossY + 100;
        
        // Random horizontal velocity
        minionVelX[i] = (rand() % 3 == 0) ? 1.5f : -1.5f;
        minionVelY[i] = 2.0f;
        
        minionHealth[i] = 1;
        minionActive[i] = true;
        minionFollowMode[i] = bossIsAngry;  // Follow mode only when angry
        minionCaptured[i] = false;
        minionIsRolling[i] = false;
    }
    
    minionCount = newTotal;
    currentAliveMinions += spawnCount;
    
    std::cout << "Wave spawned! " << spawnCount << " minions | Total alive: " 
         << currentAliveMinions << "/" << maxMinionLimit << std::endl;
}

// Update Minions
void updateMinions(float deltaTime, float playerX, float playerY, 
                   char** level, int rows, int cols, int cellSize) {
    for (int i = 0; i < minionCount; i++) {
        if (!minionActive[i] || minionCaptured[i]) continue;

        // FOLLOW MODE (when boss angry)
        if (minionFollowMode[i] && bossIsAngry) {
            float dx = playerX - minionX[i];
            float dy = playerY - minionY[i];
            float dist = sqrt(dx*dx + dy*dy);
            if (dist > 5) {
                minionVelX[i] = (dx / dist) * 3.0f;
                minionVelY[i] = (dy / dist) * 3.0f;
            }
        }

        // Apply velocity
        minionX[i] += minionVelX[i];
        minionY[i] += minionVelY[i];

        // Gravity
        if (!minionFollowMode[i]) {
            minionVelY[i] += 0.5f;
            if (minionVelY[i] > 10) minionVelY[i] = 10;
        }

        // Platform collision
        int row = (int)(minionY[i] + minionHeight) / cellSize;
        int col = (int)(minionX[i] + minionWidth/2) / cellSize;
        if (row >= 0 && row < rows && col >= 0 && col < cols) {
            if (level[row][col] == '#') {
                minionY[i] = row * cellSize - minionHeight;
                minionVelY[i] = 0;
                minionVelX[i] *= 0.8f;
            }
        }

        // Bounce off walls
        if (minionX[i] < cellSize || minionX[i] > cols*cellSize - minionWidth) {
            minionVelX[i] *= -1;
        }

        // Keep in bounds
        if (minionX[i] < 0) minionX[i] = 0;
        if (minionX[i] + minionWidth > cols * cellSize) 
            minionX[i] = cols * cellSize - minionWidth;
        if (minionY[i] < 0) minionY[i] = 0;
        if (minionY[i] + minionHeight > rows * cellSize) 
            minionY[i] = rows * cellSize - minionHeight;
    }
}

// Draw Minions
void drawMinions(RenderWindow& window) {
    // Load textures (only once)
    static bool texturesLoaded = false;
    if (!texturesLoaded) {
        if (!minionTextureNormal.loadFromFile("Assets/minion.png")) {
            std::cout << "Warning: Could not load minion.png" << std::endl;
            Image img;
            img.create(48, 48, Color::Blue);
            minionTextureNormal.loadFromImage(img);
        }
        if (!minionTextureAngry.loadFromFile("Assets/minion_angry.png")) {
            std::cout << "Warning: Could not load minion_angry.png" << std::endl;
            Image img;
            img.create(72, 72, Color::Red);
            minionTextureAngry.loadFromImage(img);
        }
        texturesLoaded = true;
    }

    Sprite minionSprite;
    for (int i = 0; i < minionCount; i++) {
        if (minionActive[i] && !minionCaptured[i]) {
            if (bossIsAngry) {
                minionSprite.setTexture(minionTextureAngry);
                minionSprite.setScale(1.5f, 1.5f);
            } else {
                minionSprite.setTexture(minionTextureNormal);
                minionSprite.setScale(1.0f, 1.0f);
            }
            minionSprite.setPosition(minionX[i], minionY[i]);
            window.draw(minionSprite);
        }
    }
}

// Spawn Single Tentacle (Balanced System)
void spawnTentacle(int rows, int cols, int cellSize) {
    // Count active tentacles
    int activeTentacles = 0;
    for (int i = 0; i < tentacleCount; i++) {
        if (tentacleActive[i]) activeTentacles++;
    }
    
    // DON'T SPAWN if at max limit
    if (activeTentacles >= maxTentacleLimit) {
        return;
    }
    
    int newCount = tentacleCount + 1;

    // Allocate new arrays
    float* newX = new float[newCount];
    float* newY = new float[newCount];
    float* newDuration = new float[newCount];
    float* newTimer = new float[newCount];
    bool* newActive = new bool[newCount];

    // Copy existing
    for (int i = 0; i < tentacleCount; i++) {
        newX[i] = tentacleX[i];
        newY[i] = tentacleY[i];
        newDuration[i] = tentacleDuration[i];
        newTimer[i] = tentacleTimer[i];
        newActive[i] = tentacleActive[i];
    }

    // Delete old
    if (tentacleX != nullptr) {
        delete[] tentacleX;
        delete[] tentacleY;
        delete[] tentacleDuration;
        delete[] tentacleTimer;
        delete[] tentacleActive;
    }

    // Assign new
    tentacleX = newX;
    tentacleY = newY;
    tentacleDuration = newDuration;
    tentacleTimer = newTimer;
    tentacleActive = newActive;

    // Initialize new tentacle at STRATEGIC position
    // Avoid edges and corners (spawn in playable area)
    int spawnCol = 5 + (rand() % (cols - 10));  // Middle 2/3 of screen
    int spawnRow = 5 + (rand() % (rows - 10));
    
    int idx = tentacleCount;
    tentacleX[idx] = spawnCol * cellSize;
    tentacleY[idx] = spawnRow * cellSize;
    tentacleDuration[idx] = tentacleLifetime;
    tentacleTimer[idx] = 0;
    tentacleActive[idx] = true;
    tentacleCount++;
    
    std::cout << "Tentacle spawned! Active: " << (activeTentacles + 1) 
         << "/" << maxTentacleLimit << std::endl;
}

// Main Boss Spawning Update Function (Balanced System)
void updateBossSpawning(float deltaTime, int rows, int cols, int cellSize) {
    if (bossDefeated) return;
    
    // Update boss phase based on health
    updateBossPhase(bossHealth, bossIsAngry);
    
    // MINION WAVE SPAWNING (time-based, not continuous)
    if (minionWaveClock.getElapsedTime().asSeconds() >= minionWaveInterval) {
        spawnMinionWave();
        minionWaveClock.restart();
    }
    
    // TENTACLE SPAWNING (time-based, controlled)
    if (tentacleSpawnClock.getElapsedTime().asSeconds() >= tentacleSpawnInterval) {
        spawnTentacle(rows, cols, cellSize);
        tentacleSpawnClock.restart();
    }
}

// Update Tentacles
void updateTentacles(float deltaTime, int rows, int cols, int cellSize) {
    // Update timers and remove expired
    for (int i = 0; i < tentacleCount; i++) {
        if (tentacleActive[i]) {
            tentacleTimer[i] += deltaTime;
            if (tentacleTimer[i] >= tentacleDuration[i]) {
                // Remove this tentacle
                if (tentacleCount == 1) {
                    delete[] tentacleX;
                    delete[] tentacleY;
                    delete[] tentacleDuration;
                    delete[] tentacleTimer;
                    delete[] tentacleActive;
                    tentacleX = nullptr;
                    tentacleY = nullptr;
                    tentacleDuration = nullptr;
                    tentacleTimer = nullptr;
                    tentacleActive = nullptr;
                    tentacleCount = 0;
                    return;
                }

                // Create smaller arrays
                int newCount = tentacleCount - 1;
                float* newX = new float[newCount];
                float* newY = new float[newCount];
                float* newDuration = new float[newCount];
                float* newTimer = new float[newCount];
                bool* newActive = new bool[newCount];

                // Copy all except removed
                int newIdx = 0;
                for (int j = 0; j < tentacleCount; j++) {
                    if (j != i) {
                        newX[newIdx] = tentacleX[j];
                        newY[newIdx] = tentacleY[j];
                        newDuration[newIdx] = tentacleDuration[j];
                        newTimer[newIdx] = tentacleTimer[j];
                        newActive[newIdx] = tentacleActive[j];
                        newIdx++;
                    }
                }

                delete[] tentacleX;
                delete[] tentacleY;
                delete[] tentacleDuration;
                delete[] tentacleTimer;
                delete[] tentacleActive;

                tentacleX = newX;
                tentacleY = newY;
                tentacleDuration = newDuration;
                tentacleTimer = newTimer;
                tentacleActive = newActive;
                tentacleCount = newCount;
                i--;
            }
        }
    }

    // Tentacle spawning is now handled by updateBossSpawning
}

// Draw Tentacles
void drawTentacles(RenderWindow& window) {
    static bool textureLoaded = false;
    if (!textureLoaded) {
        if (!tentacleTexture.loadFromFile("Assets/tentacle.png")) {
            std::cout << "Warning: Could not load tentacle.png" << std::endl;
            Image img;
            img.create(80, 120, Color(100, 0, 100));
            tentacleTexture.loadFromImage(img);
        }
        textureLoaded = true;
    }

    Sprite tentacleSprite;
    tentacleSprite.setTexture(tentacleTexture);
    for (int i = 0; i < tentacleCount; i++) {
        if (tentacleActive[i]) {
            tentacleSprite.setPosition(tentacleX[i], tentacleY[i]);
            window.draw(tentacleSprite);
        }
    }
}

// Check minion-tentacle collision (respawn mechanic)
void checkMinionTentacleCollision() {
    for (int i = 0; i < minionCount; i++) {
        if (!minionActive[i] || minionCaptured[i]) continue;

        if (minionIsRolling[i]) {
            for (int j = 0; j < tentacleCount; j++) {
                if (!tentacleActive[j]) continue;

                // AABB collision
                bool collisionX = minionX[i] + minionWidth > tentacleX[j] &&
                                 minionX[i] < tentacleX[j] + tentacleWidth;
                bool collisionY = minionY[i] + minionHeight > tentacleY[j] &&
                                 minionY[i] < tentacleY[j] + tentacleHeight;

                if (collisionX && collisionY) {
                    // RESPAWN MINION at collision location
                    minionHealth[i] = 1;
                    minionVelX[i] = 0;
                    minionVelY[i] = 0;
                    minionIsRolling[i] = false;
                    std::cout << "Minion respawned at tentacle!" << std::endl;
                    break;
                }
            }
        }
    }
}

// Check boss damage (from minion projectiles hitting boss head)
void checkBossDamage() {
    if (bossDefeated) return;

    // Check rolling minions hitting boss (existing logic)
    for (int i = 0; i < minionCount; i++) {
        if (!minionActive[i]) continue;

        if (minionIsRolling[i]) {
            // Check collision with boss head (top 80px of boss)
            bool hitX = minionX[i] + minionWidth > bossX - bossWidth/2 &&
                       minionX[i] < bossX + bossWidth/2;
            bool hitY = minionY[i] + minionHeight > bossY - bossHeight/2 &&
                       minionY[i] < bossY - bossHeight/2 + 80;

            if (hitX && hitY) {
                bossHealth--;
                minionActive[i] = false;
                std::cout << "Boss hit by minion! Health: " << bossHealth << "/" << bossMaxHealth << std::endl;
                if (bossHealth <= 0) {
                    bossDefeated = true;
                    std::cout << "BOSS DEFEATED!" << std::endl;
                }
                break;
            }
        }
    }
    
    // Check thrown enemies hitting boss (like level 2)
    for (int i = 0; i < MAX_THROWN_ENEMIES; i++) {
        if (!thrownEnemyActive[i]) continue;
        
        // Check collision with boss head (top 80px of boss)
        bool hitX = thrownEnemyX[i] + thrownEnemyWidth[i] > bossX - bossWidth/2 &&
                   thrownEnemyX[i] < bossX + bossWidth/2;
        bool hitY = thrownEnemyY[i] + thrownEnemyHeight[i] > bossY - bossHeight/2 &&
                   thrownEnemyY[i] < bossY - bossHeight/2 + 80;
        
        if (hitX && hitY) {
            bossHealth--;
            thrownEnemyActive[i] = false;
            addPoints(200, "Boss Hit by Projectile");
            std::cout << "Boss hit by thrown enemy! Health: " << bossHealth << "/" << bossMaxHealth << std::endl;
            if (bossHealth <= 0) {
                bossDefeated = true;
                std::cout << "BOSS DEFEATED!" << std::endl;
            }
            break;
        }
    }
    
    // Check thrown enemy cluster hitting boss (like level 2)
    if (thrownClusterActive) {
        // Check collision with boss head (top 80px of boss)
        bool hitX = thrownClusterX + thrownClusterRadius > bossX - bossWidth/2 &&
                   thrownClusterX - thrownClusterRadius < bossX + bossWidth/2;
        bool hitY = thrownClusterY + thrownClusterRadius > bossY - bossHeight/2 &&
                   thrownClusterY - thrownClusterRadius < bossY - bossHeight/2 + 80;
        
        if (hitX && hitY) {
            // Damage boss based on number of enemies in cluster
            int damage = thrownClusterEnemyCount;
            bossHealth -= damage;
            thrownClusterActive = false;
            addPoints(200 * damage, "Boss Hit by Cluster");
            std::cout << "Boss hit by enemy cluster! Health: " << bossHealth << "/" << bossMaxHealth << std::endl;
            if (bossHealth <= 0) {
                bossDefeated = true;
                std::cout << "BOSS DEFEATED!" << std::endl;
            }
        }
    }
}

// Update Water
void updateWater(float deltaTime) {
    if (waterRising && waterLevel > waterTargetLevel) {
        waterLevel -= waterRiseSpeed * deltaTime;
        if (waterLevel <= waterTargetLevel) {
            waterLevel = waterTargetLevel;
            waterRising = false;
            std::cout << "Water level stabilized!" << std::endl;
        }
    }
}

// Draw Water
void drawWater(RenderWindow& window, int screenWidth, int screenHeight) {
    if (waterLevel < screenHeight) {
        waterRect.setSize(Vector2f(screenWidth, screenHeight - waterLevel));
        waterRect.setPosition(0, waterLevel);
        waterRect.setFillColor(Color(0, 100, 200, 150));
        window.draw(waterRect);
    }
}

// Cleanup Boss Data
void cleanupBossData() {
    // Cleanup minions
    if (minionX != nullptr) {
        delete[] minionX;
        delete[] minionY;
        delete[] minionVelX;
        delete[] minionVelY;
        delete[] minionHealth;
        delete[] minionActive;
        delete[] minionFollowMode;
        delete[] minionCaptured;
        delete[] minionIsRolling;
        minionX = nullptr;
        minionY = nullptr;
        minionVelX = nullptr;
        minionVelY = nullptr;
        minionHealth = nullptr;
        minionActive = nullptr;
        minionFollowMode = nullptr;
        minionCaptured = nullptr;
        minionIsRolling = nullptr;
        minionCount = 0;
    }

    // Cleanup tentacles
    if (tentacleX != nullptr) {
        delete[] tentacleX;
        delete[] tentacleY;
        delete[] tentacleDuration;
        delete[] tentacleTimer;
        delete[] tentacleActive;
        tentacleX = nullptr;
        tentacleY = nullptr;
        tentacleDuration = nullptr;
        tentacleTimer = nullptr;
        tentacleActive = nullptr;
        tentacleCount = 0;
    }
}

// Menu System Functions Implementation

// Initialize Menu Options
void initializeMenuOptions(Font& font) {
    float startY = 380.0f;
    float spacing = 100.0f;
    float centerX = screen_x / 2.0f;

    const char* optionTexts[MENU_OPTION_COUNT] = {"PLAY", "BOSS LEVEL", "LEADERBOARD", "SETTINGS", "EXIT"};
    
    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        menuOptionText[i] = optionTexts[i];
        // Position is center of where text will be (centered in box)
        menuOptionPosition[i] = Vector2f(centerX, startY + i * spacing);
        menuOptionIsSelected[i] = (i == 0);
    }
}

// Initialize Characters
void initializeCharacters() {
    // Yellow TumblePopper
    characterName[0] = "YELLOW TUMBLE";
    characterAbilityName[0] = "Speed Master";
    characterAbilityDesc[0] = "Higher movement speed\nQuick vacuum rotation";
    characterSpeedStars[0] = 4;
    characterPowerStars[0] = 3;
    characterIsSelected[0] = true;

    // Green TumblePopper
    characterName[1] = "GREEN TUMBLE";
    characterAbilityName[1] = "Power Striker";
    characterAbilityDesc[1] = "Stronger attacks\nLarger vacuum range";
    characterSpeedStars[1] = 3;
    characterPowerStars[1] = 4;
    characterIsSelected[1] = false;
}

// Display Main Menu
void displayMainMenu(RenderWindow& window, Texture& bgTexture, Font& font, int selectedIndex) {
    // Draw background
    Sprite bg(bgTexture);
    bg.setScale((float)screen_x / bgTexture.getSize().x, (float)screen_y / bgTexture.getSize().y);
    window.draw(bg);

    // Draw title in a styled box
    float titleBoxWidth = 600.0f;
    float titleBoxHeight = 100.0f;
    float titleBoxX = screen_x / 2.0f - titleBoxWidth / 2.0f;
    float titleBoxY = 120.0f;
    
    RectangleShape titleBox(Vector2f(titleBoxWidth, titleBoxHeight));
    titleBox.setPosition(titleBoxX, titleBoxY);
    titleBox.setFillColor(Color(0, 0, 0, 200)); // Semi-transparent black
    titleBox.setOutlineThickness(5.0f);
    titleBox.setOutlineColor(Color(255, 255, 0)); // Yellow border
    window.draw(titleBox);
    
    // Draw title text
    Text title("TUMBLE POP", font, 72);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(3.0f);
    title.setPosition(screen_x / 2.0f - title.getLocalBounds().width / 2, 150.0f);
    window.draw(title);

    // Draw menu options
    for (int i = 0; i < MENU_OPTION_COUNT; i++) {
        bool isSelected = (i == selectedIndex);
        float scale = isSelected ? 1.15f : 1.0f;
        drawMenuOption(window, menuOptionText[i], menuOptionPosition[i], isSelected, font, scale);
    }
}

// Draw Menu Option with Styled Box
void drawMenuOption(RenderWindow& window, string text, Vector2f position, 
                    bool isSelected, Font& font, float scale) {
    // Box dimensions
    float boxWidth = 400.0f;
    float boxHeight = 70.0f;
    float boxX = screen_x / 2.0f - boxWidth / 2.0f;
    float boxY = position.y - boxHeight / 2.0f + 10.0f;
    
    // Draw box background
    RectangleShape box(Vector2f(boxWidth, boxHeight));
    box.setPosition(boxX, boxY);
    
    if (isSelected) {
        // Selected box: brighter background with glow
        box.setFillColor(Color(255, 255, 200, 180)); // Light yellow background
        box.setOutlineThickness(4.0f);
        box.setOutlineColor(Color(255, 255, 0)); // Yellow border
    } else {
        // Non-selected box: darker background
        box.setFillColor(Color(50, 50, 50, 200)); // Dark gray background
        box.setOutlineThickness(2.0f);
        box.setOutlineColor(Color(150, 150, 150)); // Light gray border
    }
    window.draw(box);
    
    // Draw text centered in box
    Text optionText(text, font, 48);
    optionText.setPosition(screen_x / 2.0f, position.y);
    
    if (isSelected) {
        optionText.setFillColor(Color(255, 255, 0)); // Yellow
        optionText.setStyle(Text::Bold);
        optionText.setOutlineColor(Color::Black);
        optionText.setOutlineThickness(2.0f);
        
        // Apply scale
        Vector2f center = Vector2f(screen_x / 2.0f, position.y);
        optionText.setOrigin(optionText.getLocalBounds().width / 2,
                            optionText.getLocalBounds().height / 2);
        optionText.setScale(scale, scale);
        optionText.setPosition(center);
    } else {
        optionText.setFillColor(Color(220, 220, 220)); // Light gray
        optionText.setStyle(Text::Regular);
        optionText.setOutlineColor(Color::Black);
        optionText.setOutlineThickness(1.5f);
        
        // Center text
        optionText.setOrigin(optionText.getLocalBounds().width / 2,
                            optionText.getLocalBounds().height / 2);
    }
    
    window.draw(optionText);
}

// Handle Menu Input
void handleMenuInput(int& selectedIndex, int optionCount, 
                     int& currentState, RenderWindow& window, bool& keyPressed) {
    if (keyPressed) return;

    // Navigation
    if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) {
        selectedIndex = (selectedIndex - 1 + optionCount) % optionCount;
        keyPressed = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) {
        selectedIndex = (selectedIndex + 1) % optionCount;
        keyPressed = true;
    }

    // Selection
    if (Keyboard::isKeyPressed(Keyboard::Enter) || Keyboard::isKeyPressed(Keyboard::Space)) {
        keyPressed = true;
        switch (selectedIndex) {
            case 0: // PLAY
                currentState = CHARACTER_SELECT;
                break;
            case 1: // BOSS LEVEL
                currentState = PLAYING_BOSS_LEVEL;
                break;
            case 2: // LEADERBOARD
                // Placeholder - could show leaderboard screen
                break;
            case 3: // SETTINGS
                // Placeholder - could show settings screen
                break;
            case 4: // EXIT
                window.close();
                break;
        }
    }
}

// Display Character Selection
void displayCharacterSelect(RenderWindow& window, int selectedIndex, Texture& bgTexture, Font& font, float time) {
    // Draw background
    Sprite bg(bgTexture);
    bg.setScale((float)screen_x / bgTexture.getSize().x, (float)screen_y / bgTexture.getSize().y);
    window.draw(bg);

    // Draw title in a styled box
    float titleBoxWidth = 700.0f;
    float titleBoxHeight = 80.0f;
    float titleBoxX = screen_x / 2.0f - titleBoxWidth / 2.0f;
    float titleBoxY = 50.0f;
    
    RectangleShape titleBox(Vector2f(titleBoxWidth, titleBoxHeight));
    titleBox.setPosition(titleBoxX, titleBoxY);
    titleBox.setFillColor(Color(0, 0, 0, 200)); // Semi-transparent black
    titleBox.setOutlineThickness(4.0f);
    titleBox.setOutlineColor(Color(255, 255, 0)); // Yellow border
    window.draw(titleBox);
    
    // Draw title text
    Text title("CHARACTER SELECTION", font, 56);
    title.setFillColor(Color::White);
    title.setStyle(Text::Bold);
    title.setOutlineColor(Color::Black);
    title.setOutlineThickness(3.0f);
    title.setPosition(screen_x / 2.0f - title.getLocalBounds().width / 2, 80.0f);
    window.draw(title);

    // Character box dimensions and positions
    float boxWidth = 300.0f;
    float boxHeight = 400.0f;
    float centerY = 250.0f;
    float gap = 200.0f;
    float leftBoxX = screen_x / 2.0f - gap / 2 - boxWidth / 2;
    float rightBoxX = screen_x / 2.0f + gap / 2 - boxWidth / 2;

    // Draw character boxes
    for (int i = 0; i < CHARACTER_COUNT; i++) {
        bool isSelected = (i == selectedIndex);
        Vector2f boxPos = (i == 0) ? Vector2f(leftBoxX, centerY) : Vector2f(rightBoxX, centerY);
        drawCharacterBox(window, i, boxPos, isSelected, font, time);
    }

    // Draw instruction text in a box
    float instBoxWidth = 600.0f;
    float instBoxHeight = 50.0f;
    float instBoxX = screen_x / 2.0f - instBoxWidth / 2.0f;
    float instBoxY = 730.0f;
    
    RectangleShape instBox(Vector2f(instBoxWidth, instBoxHeight));
    instBox.setPosition(instBoxX, instBoxY);
    instBox.setFillColor(Color(0, 0, 0, 180)); // Semi-transparent black
    instBox.setOutlineThickness(2.0f);
    instBox.setOutlineColor(Color(200, 200, 200)); // Light gray border
    window.draw(instBox);
    
    Text instruction("Press ENTER to Select | ESC to Return", font, 26);
    instruction.setFillColor(Color::White);
    instruction.setOutlineColor(Color::Black);
    instruction.setOutlineThickness(1.5f);
    instruction.setPosition(screen_x / 2.0f - instruction.getLocalBounds().width / 2, 745.0f);
    window.draw(instruction);
}

// Draw Character Box
void drawCharacterBox(RenderWindow& window, int characterIndex, 
                      Vector2f position, bool isSelected, Font& font, float time) {
    float boxWidth = 300.0f;
    float boxHeight = 400.0f;
    float padding = 20.0f;

    // Calculate opacity and scale
    float opacity = isSelected ? 1.0f : 0.5f;
    float scale = isSelected ? 1.05f : 1.0f;
    
    // Pulsing effect for selected character
    if (isSelected) {
        scale += sin(time * 3.0f) * 0.02f;
    }

    // Draw box background
    RectangleShape boxBg(Vector2f(boxWidth, boxHeight));
    boxBg.setPosition(position);
    
    if (isSelected) {
        boxBg.setFillColor(Color(255, 255, 200, 150)); // Light yellow background
        float borderThickness = 6.0f + sin(time * 4.0f) * 2.0f;
        boxBg.setOutlineThickness(borderThickness);
        boxBg.setOutlineColor(Color(255, 255, 0)); // Yellow glow
    } else {
        boxBg.setFillColor(Color(30, 30, 30, 180)); // Dark background
        boxBg.setOutlineThickness(3.0f);
        boxBg.setOutlineColor(Color(100, 100, 100, 128)); // Dim gray
    }
    window.draw(boxBg);

    // Draw character sprite (from Assets/yellow.png or Assets/green.png)
    Sprite charSprite = characterSprite[characterIndex];
    // Center the sprite in the box
    float spriteWidth = characterTexture[characterIndex].getSize().x * charSprite.getScale().x;
    float spriteHeight = characterTexture[characterIndex].getSize().y * charSprite.getScale().y;
    charSprite.setPosition(position.x + boxWidth / 2 - spriteWidth / 2, 
                          position.y + padding);
    charSprite.setColor(Color(255, 255, 255, (Uint8)(255 * opacity)));
    window.draw(charSprite);

    // Draw character name in a small box
    float nameBoxWidth = boxWidth - 10.0f;
    float nameBoxHeight = 35.0f;
    RectangleShape nameBox(Vector2f(nameBoxWidth, nameBoxHeight));
    nameBox.setPosition(position.x + 5.0f, position.y + 315.0f);
    nameBox.setFillColor(Color(0, 0, 0, (Uint8)(200 * opacity)));
    nameBox.setOutlineThickness(2.0f);
    int charType = (characterIndex == 0) ? YELLOW_TUMBLE : GREEN_TUMBLE;
    nameBox.setOutlineColor((charType == YELLOW_TUMBLE) ? Color(255, 255, 0, (Uint8)(255 * opacity)) : Color(0, 255, 0, (Uint8)(255 * opacity)));
    window.draw(nameBox);
    
    Text nameText(characterName[characterIndex], font, 32);
    Color nameColor = (charType == YELLOW_TUMBLE) ? Color(255, 255, 0) : Color(0, 255, 0);
    nameColor.a = (Uint8)(255 * opacity);
    nameText.setFillColor(nameColor);
    nameText.setStyle(Text::Bold);
    nameText.setOutlineColor(Color::Black);
    nameText.setOutlineThickness(2.0f);
    nameText.setPosition(position.x + boxWidth / 2 - nameText.getLocalBounds().width / 2,
                        position.y + 325.0f);
    window.draw(nameText);

    // Draw ability name in a small box
    float abilityBoxWidth = boxWidth - 10.0f;
    float abilityBoxHeight = 30.0f;
    RectangleShape abilityBox(Vector2f(abilityBoxWidth, abilityBoxHeight));
    abilityBox.setPosition(position.x + 5.0f, position.y + 355.0f);
    abilityBox.setFillColor(Color(0, 0, 0, (Uint8)(180 * opacity)));
    abilityBox.setOutlineThickness(1.5f);
    abilityBox.setOutlineColor(Color(200, 200, 200, (Uint8)(255 * opacity)));
    window.draw(abilityBox);
    
    Text abilityNameText(characterAbilityName[characterIndex], font, 24);
    abilityNameText.setFillColor(Color(255, 255, 255, (Uint8)(255 * opacity)));
    abilityNameText.setStyle(Text::Bold);
    abilityNameText.setOutlineColor(Color::Black);
    abilityNameText.setOutlineThickness(1.5f);
    abilityNameText.setPosition(position.x + boxWidth / 2 - abilityNameText.getLocalBounds().width / 2,
                               position.y + 363.0f);
    window.draw(abilityNameText);

    // Draw ability description in a box (multi-line)
    float descBoxWidth = boxWidth - 10.0f;
    float descBoxHeight = 50.0f;
    RectangleShape descBox(Vector2f(descBoxWidth, descBoxHeight));
    descBox.setPosition(position.x + 5.0f, position.y + 390.0f);
    descBox.setFillColor(Color(0, 0, 0, (Uint8)(160 * opacity)));
    descBox.setOutlineThickness(1.5f);
    descBox.setOutlineColor(Color(150, 150, 150, (Uint8)(255 * opacity)));
    window.draw(descBox);
    
    string desc = characterAbilityDesc[characterIndex];
    size_t newlinePos = desc.find('\n');
    if (newlinePos != string::npos) {
        string line1 = desc.substr(0, newlinePos);
        string line2 = desc.substr(newlinePos + 1);
        
        Text descText1(line1, font, 18);
        descText1.setFillColor(Color(220, 220, 220, (Uint8)(255 * opacity)));
        descText1.setOutlineColor(Color::Black);
        descText1.setOutlineThickness(1.0f);
        descText1.setPosition(position.x + boxWidth / 2 - descText1.getLocalBounds().width / 2,
                             position.y + 400.0f);
        window.draw(descText1);
        
        Text descText2(line2, font, 18);
        descText2.setFillColor(Color(220, 220, 220, (Uint8)(255 * opacity)));
        descText2.setOutlineColor(Color::Black);
        descText2.setOutlineThickness(1.0f);
        descText2.setPosition(position.x + boxWidth / 2 - descText2.getLocalBounds().width / 2,
                             position.y + 420.0f);
        window.draw(descText2);
    } else {
        Text descText(desc, font, 18);
        descText.setFillColor(Color(220, 220, 220, (Uint8)(255 * opacity)));
        descText.setOutlineColor(Color::Black);
        descText.setOutlineThickness(1.0f);
        descText.setPosition(position.x + boxWidth / 2 - descText.getLocalBounds().width / 2,
                            position.y + 405.0f);
        window.draw(descText);
    }

    // Draw stats box (Speed and Power with stars)
    float statsBoxWidth = boxWidth - 10.0f;
    float statsBoxHeight = 50.0f;
    float statsY = position.y + 445.0f;
    
    RectangleShape statsBox(Vector2f(statsBoxWidth, statsBoxHeight));
    statsBox.setPosition(position.x + 5.0f, statsY);
    statsBox.setFillColor(Color(0, 0, 0, (Uint8)(180 * opacity)));
    statsBox.setOutlineThickness(2.0f);
    statsBox.setOutlineColor(Color(100, 100, 255, (Uint8)(255 * opacity))); // Light blue border
    window.draw(statsBox);
    
    // Speed stat
    string speedStr = "Speed: ";
    for (int i = 0; i < 5; i++) {
        speedStr += (i < characterSpeedStars[characterIndex]) ? "★" : "☆";
    }
    Text speedText(speedStr, font, 20);
    speedText.setFillColor(Color(255, 255, 255, (Uint8)(255 * opacity)));
    speedText.setOutlineColor(Color::Black);
    speedText.setOutlineThickness(1.0f);
    speedText.setPosition(position.x + padding, statsY + 5.0f);
    window.draw(speedText);

    // Power stat
    string powerStr = "Power: ";
    for (int i = 0; i < 5; i++) {
        powerStr += (i < characterPowerStars[characterIndex]) ? "★" : "☆";
    }
    Text powerText(powerStr, font, 20);
    powerText.setFillColor(Color(255, 255, 255, (Uint8)(255 * opacity)));
    powerText.setOutlineColor(Color::Black);
    powerText.setOutlineThickness(1.0f);
    powerText.setPosition(position.x + padding, statsY + 25.0f);
    window.draw(powerText);
}

// Handle Character Selection Input
void handleCharacterSelectInput(int& selectedIndex, int& currentState, 
                                bool& keyPressed, int& playerCharacter) {
    if (keyPressed) return;

    // Navigation
    if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A)) {
        selectedIndex = 0;
        keyPressed = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D)) {
        selectedIndex = 1;
        keyPressed = true;
    }

    // Selection
    if (Keyboard::isKeyPressed(Keyboard::Enter) || Keyboard::isKeyPressed(Keyboard::Space)) {
        playerCharacter = getSelectedCharacter(selectedIndex);
        currentState = PLAYING_LEVEL1;
        keyPressed = true;
    }

    // Return to menu
    if (Keyboard::isKeyPressed(Keyboard::Escape)) {
        currentState = MAIN_MENU;
        keyPressed = true;
    }
}

// Get Selected Character Type
int getSelectedCharacter(int index) {
    return (index == 0) ? YELLOW_TUMBLE : GREEN_TUMBLE;
}

// Display Game Over Screen
void displayGameOver(RenderWindow& window, Texture& bgTexture, Font& font, 
                     int selectedIndex, int finalScore) {
    // Draw background
    Sprite bg(bgTexture);
    bg.setScale((float)screen_x / bgTexture.getSize().x, (float)screen_y / bgTexture.getSize().y);
    window.draw(bg);

    // Draw bold "GAME OVER" title in a styled box
    float titleBoxWidth = 500.0f;
    float titleBoxHeight = 120.0f;
    float titleBoxX = screen_x / 2.0f - titleBoxWidth / 2.0f;
    float titleBoxY = 150.0f;
    
    RectangleShape titleBox(Vector2f(titleBoxWidth, titleBoxHeight));
    titleBox.setPosition(titleBoxX, titleBoxY);
    titleBox.setFillColor(Color(100, 0, 0, 220)); // Dark red background
    titleBox.setOutlineThickness(6.0f);
    titleBox.setOutlineColor(Color::Red); // Red border
    window.draw(titleBox);
    
    Text gameOverTitle("GAME OVER", font, 76);
    gameOverTitle.setFillColor(Color::Red);
    gameOverTitle.setStyle(Text::Bold);
    gameOverTitle.setOutlineColor(Color::Black);
    gameOverTitle.setOutlineThickness(4.0f);
    gameOverTitle.setPosition(screen_x / 2.0f - gameOverTitle.getLocalBounds().width / 2, 200.0f);
    window.draw(gameOverTitle);

    // Draw final score in a box
    float scoreBoxWidth = 400.0f;
    float scoreBoxHeight = 60.0f;
    float scoreBoxX = screen_x / 2.0f - scoreBoxWidth / 2.0f;
    float scoreBoxY = 300.0f;
    
    RectangleShape scoreBox(Vector2f(scoreBoxWidth, scoreBoxHeight));
    scoreBox.setPosition(scoreBoxX, scoreBoxY);
    scoreBox.setFillColor(Color(0, 0, 0, 200)); // Semi-transparent black
    scoreBox.setOutlineThickness(3.0f);
    scoreBox.setOutlineColor(Color::Yellow); // Yellow border
    window.draw(scoreBox);
    
    Text scoreText("Final Score: " + to_string(finalScore), font, 38);
    scoreText.setFillColor(Color::White);
    scoreText.setStyle(Text::Bold);
    scoreText.setOutlineColor(Color::Black);
    scoreText.setOutlineThickness(2.0f);
    scoreText.setPosition(screen_x / 2.0f - scoreText.getLocalBounds().width / 2, 315.0f);
    window.draw(scoreText);

    // Draw menu options in styled boxes
    float startY = 420.0f;
    float spacing = 100.0f;
    float centerX = screen_x / 2.0f;
    float boxWidth = 450.0f;
    float boxHeight = 70.0f;

    const char* gameOverOptions[2] = {"BACK TO MENU", "QUIT GAME"};
    
    for (int i = 0; i < 2; i++) {
        bool isSelected = (i == selectedIndex);
        float scale = isSelected ? 1.12f : 1.0f;
        float boxX = centerX - boxWidth / 2.0f;
        float boxY = startY + i * spacing - boxHeight / 2.0f + 10.0f;
        
        // Draw box background
        RectangleShape optionBox(Vector2f(boxWidth, boxHeight));
        optionBox.setPosition(boxX, boxY);
        
        if (isSelected) {
            optionBox.setFillColor(Color(255, 255, 200, 180)); // Light yellow background
            optionBox.setOutlineThickness(4.0f);
            optionBox.setOutlineColor(Color(255, 255, 0)); // Yellow border
        } else {
            optionBox.setFillColor(Color(50, 50, 50, 200)); // Dark gray background
            optionBox.setOutlineThickness(2.0f);
            optionBox.setOutlineColor(Color(150, 150, 150)); // Light gray border
        }
        window.draw(optionBox);
        
        // Draw text
        Text optionText(gameOverOptions[i], font, 44);
        optionText.setPosition(centerX, startY + i * spacing);
        
        if (isSelected) {
            optionText.setFillColor(Color(255, 255, 0)); // Yellow
            optionText.setStyle(Text::Bold);
            optionText.setOutlineColor(Color::Black);
            optionText.setOutlineThickness(2.0f);
            
            // Apply scale
            Vector2f center = Vector2f(centerX, startY + i * spacing);
            optionText.setOrigin(optionText.getLocalBounds().width / 2,
                                optionText.getLocalBounds().height / 2);
            optionText.setScale(scale, scale);
            optionText.setPosition(center);
        } else {
            optionText.setFillColor(Color(220, 220, 220)); // Light gray
            optionText.setStyle(Text::Regular);
            optionText.setOutlineColor(Color::Black);
            optionText.setOutlineThickness(1.5f);
            
            // Center text
            optionText.setOrigin(optionText.getLocalBounds().width / 2,
                                optionText.getLocalBounds().height / 2);
        }
        
        window.draw(optionText);
    }

    // Draw instruction text in a box
    float instBoxWidth = 550.0f;
    float instBoxHeight = 45.0f;
    float instBoxX = screen_x / 2.0f - instBoxWidth / 2.0f;
    float instBoxY = 640.0f;
    
    RectangleShape instBox(Vector2f(instBoxWidth, instBoxHeight));
    instBox.setPosition(instBoxX, instBoxY);
    instBox.setFillColor(Color(0, 0, 0, 180)); // Semi-transparent black
    instBox.setOutlineThickness(2.0f);
    instBox.setOutlineColor(Color(200, 200, 200)); // Light gray border
    window.draw(instBox);
    
    Text instruction("Use UP/DOWN to navigate | ENTER to select", font, 24);
    instruction.setFillColor(Color::White);
    instruction.setOutlineColor(Color::Black);
    instruction.setOutlineThickness(1.5f);
    instruction.setPosition(screen_x / 2.0f - instruction.getLocalBounds().width / 2, 652.0f);
    window.draw(instruction);
}

// Handle Game Over Input
void handleGameOverInput(int& selectedIndex, int& currentState, 
                         RenderWindow& window, bool& keyPressed) {
    if (keyPressed) return;

    // Navigation
    if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W)) {
        selectedIndex = (selectedIndex - 1 + 2) % 2;
        keyPressed = true;
    }
    if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) {
        selectedIndex = (selectedIndex + 1) % 2;
        keyPressed = true;
    }

    // Selection
    if (Keyboard::isKeyPressed(Keyboard::Enter) || Keyboard::isKeyPressed(Keyboard::Space)) {
        keyPressed = true;
        switch (selectedIndex) {
            case 0: // BACK TO MENU
                currentState = MAIN_MENU;
                break;
            case 1: // QUIT GAME
                window.close();
                break;
        }
    }
}
