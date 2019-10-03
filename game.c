#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define PLAYER_MAX_LIFE 5
#define LINES_OF_BRICKS 80
#define BRICKS_PER_LINE 40
#define FILE_NUM 5
#define ALPHA 40
#define MARGIN_H 100
#define MARGIN_V 100
#define TIMER_FONT_SIZE 800

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { TITLE, GAMEPLAY, ENDING } GameScreen;

typedef struct Brick {
  Vector2 position;
  bool active;
  int file;
} Brick;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------

static bool pause = false;
static bool finish = false;
static GameScreen currentScreen;
static int score = 0;
static int baseTime = 0;
static int pauseTime = 0;
static int elapsedTime = 0;

static Color timerColor = (Color){0, 158, 47, 40};
static Color timerColorRed = (Color){230, 41, 55, 40};

static const Color FILE_COLORS_DARK[FILE_NUM] = {
    (Color){255, 161, 0, 255},   // orange
    (Color){0, 228, 48, 255},    // green
    (Color){200, 122, 255, 255}, // purple
    (Color){127, 106, 79, 255},  // brown
    (Color){255, 203, 0, 255}    // gold
};

static const Color FILE_COLORS_LIGHT[FILE_NUM] = {
    (Color){255, 161, 0, ALPHA},   // orange
    (Color){0, 228, 48, ALPHA},    // green
    (Color){200, 122, 255, ALPHA}, // purple
    (Color){127, 106, 79, ALPHA},  // brown
    (Color){255, 203, 0, ALPHA}    // gold
};

static Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE] = {0};
static Vector2 brickSize = {0};
static int targetX = 0;
static int targetY = 0;
static int framesCount = 0;

static Rectangle closeButtonRec;

static Sound clickSound;
static Sound beepSound;
static Music titleMusic;
static Music bgMusic;
static Music endingMusic;


//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);        // Initialize game
static void UpdateGame(void);      // Update game (one frame)
static void DrawGame(void);        // Draw game (one frame)
static void UnloadGame(void);      // Unload game
static void UpdateDrawFrame(void); // Update and Draw (one frame)

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void) {
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();
  closeButtonRec = (Rectangle){GetScreenWidth() - 60, 0, 60, 60};

  InitWindow(screenWidth, screenHeight, "Fragmentation Game");
  InitAudioDevice();

  clickSound = LoadSound("resources/click.mp3");
  beepSound = LoadSound("resources/beep.mp3");
  titleMusic = LoadMusicStream("resources/title.mp3");
  bgMusic = LoadMusicStream("resources/bg.mp3");
  endingMusic = LoadMusicStream("resources/ending.mp3");

  InitGame();
  ToggleFullscreen();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    UpdateDrawFrame();
  }
#endif

  UnloadGame();  // Unload loaded data (textures, sounds, models...)
  CloseWindow(); // Close window and OpenGL context
  return 0;
}

//------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//------------------------------------------------------------------------------------

// Initialize game variables
void InitGame(void) {

  PlayMusicStream(titleMusic);
  StopMusicStream(bgMusic);
  StopMusicStream(endingMusic);

  // Set the current screen to title
  currentScreen = TITLE;

  // Seed random
  srand(time(NULL));

  finish = false;

  score = 0;

  closeButtonRec = (Rectangle){GetScreenWidth() - 80, 0, 80, 80};

  // Calculate Brick size
  brickSize = (Vector2){(GetScreenWidth() - MARGIN_H * 2) / BRICKS_PER_LINE,
                        (GetScreenHeight() - MARGIN_V * 2) / LINES_OF_BRICKS};

  // Select a random target brick
  targetX = rand() % BRICKS_PER_LINE;
  targetY = rand() % LINES_OF_BRICKS;

  // Generate a bricks map
  int prevFile = 0;
  for (int i = 0; i < LINES_OF_BRICKS; i++) {
    for (int j = 0; j < BRICKS_PER_LINE; j++) {
      brick[i][j].position =
          (Vector2){j * brickSize.x + brickSize.x / 2 + MARGIN_H,
                    i * brickSize.y + brickSize.y / 2 + MARGIN_V};

      brick[i][j].active = false;
      if (rand() % 10 <= 6) {
        brick[i][j].file = prevFile;
      } else {
        int file = rand() % FILE_NUM;
        brick[i][j].file = file;
        prevFile = file;
      }
    }
  }
}

// Update game (one frame)
void UpdateGame(void) {
  switch (currentScreen) {
  case TITLE: {
    UpdateMusicStream(titleMusic);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mousePos = GetMousePosition();
      if (CheckCollisionPointRec(mousePos, closeButtonRec)) {
        CloseWindow();
      } else {
        currentScreen = GAMEPLAY;
        baseTime = GetTime();

        StopMusicStream(titleMusic);
        PlayMusicStream(bgMusic);
        StopMusicStream(endingMusic);
      }
    }
    break;
  }
  case GAMEPLAY: {
    UpdateMusicStream(bgMusic);
    if (finish) {
      currentScreen = ENDING;
    } else {
      if (IsKeyPressed(KEY_P)) {
        // Reset baseTime when unpause game
        if (pause) {
          baseTime = GetTime() - (pauseTime - baseTime);
        }
        pause = !pause;
        framesCount = 0;
        pauseTime = GetTime();
      }

      if (!pause) {
        // Update framesCount
        framesCount++;

        int prevElapseTime = elapsedTime;
        // Calculate elapsed time
        elapsedTime = GetTime() - baseTime;

        if (elapsedTime > 60) {
          // Times up!
          finish = true;
          framesCount = 0;

          StopMusicStream(titleMusic);
          StopMusicStream(bgMusic);
          PlayMusicStream(endingMusic);
        } else {
          if (elapsedTime >= 50 && elapsedTime <= 60) {
            if (elapsedTime == prevElapseTime + 1) {
              PlaySound(beepSound);
            }
          }

          if (IsMouseButtonPressed(0)) {
            // Handle brick clicked
            Vector2 mousePos = GetMousePosition();
            int i = (mousePos.y - MARGIN_V) / brickSize.y;
            int j = (mousePos.x - MARGIN_H) / brickSize.x;

            if (i == targetY && j == targetX) {
              // Choose next target brick
              if (brick[i][j].file ==
                  brick[i][(j + 1) % BRICKS_PER_LINE].file) {
                if (rand() % 10 <= 8) {
                  targetX = (targetX + 1) % BRICKS_PER_LINE;
                } else {
                  targetX = rand() % BRICKS_PER_LINE;
                  targetY = rand() % LINES_OF_BRICKS;
                }
              } else {
                targetX = rand() % BRICKS_PER_LINE;
                targetY = rand() % LINES_OF_BRICKS;
              }
              brick[i][j].active = true;

              // Reset framesCount
              framesCount = 0;

              // Increase score
              score += 10;

              // Play sound
              PlaySound(clickSound);
            }
          }
        }
      }
    }

    break;
  }
  case ENDING: {
    UpdateMusicStream(endingMusic);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      InitGame();

    }
    break;
  }
  default:
    break;
  }
}

// Draw game (one frame)
void DrawGame(void) {
  BeginDrawing();

  ClearBackground(RAYWHITE);

  switch (currentScreen) {
  case TITLE: {
    DrawText("FRAGMENTATION GAME",
             GetScreenWidth() / 2 - MeasureText("FRAGMENTATION GAME", 160) / 2,
             GetScreenHeight() / 2 - 160, 160, RED);

    DrawText("Touch Screen to Start",
             GetScreenWidth() / 2 -
                 MeasureText("Touch Screen to Start", 80) / 2,
             GetScreenHeight() / 2 - 80 + 300, 80, GRAY);
    DrawRectangleLinesEx(closeButtonRec, 5, LIGHTGRAY);
    DrawLine(closeButtonRec.x, closeButtonRec.y,
             closeButtonRec.x + closeButtonRec.width,
             closeButtonRec.y + closeButtonRec.height, LIGHTGRAY);
    DrawLine(closeButtonRec.x, closeButtonRec.y + closeButtonRec.height,
             closeButtonRec.x + closeButtonRec.width, closeButtonRec.y,
             LIGHTGRAY);
    break;
  }
  case GAMEPLAY: {
    // Draw bricks
    for (int i = 0; i < LINES_OF_BRICKS; i++) {
      for (int j = 0; j < BRICKS_PER_LINE; j++) {
        Color dark = FILE_COLORS_DARK[brick[i][j].file];
        Color light = FILE_COLORS_LIGHT[brick[i][j].file];
        Color targetColor;
        if ((framesCount / 20) % 2 == 0) {
          targetColor = dark;
        } else {
          targetColor = light;
        }

        if (targetX == j && targetY == i) {
          DrawRectangle(brick[i][j].position.x - brickSize.x / 2,
                        brick[i][j].position.y - brickSize.y / 2, brickSize.x,
                        brickSize.y, targetColor);
        } else {
          if (brick[i][j].active) {
            DrawRectangle(brick[i][j].position.x - brickSize.x / 2,
                          brick[i][j].position.y - brickSize.y / 2, brickSize.x,
                          brickSize.y, dark);
          } else {
            DrawRectangle(brick[i][j].position.x - brickSize.x / 2,
                          brick[i][j].position.y - brickSize.y / 2, brickSize.x,
                          brickSize.y, light);
          }
        }
        DrawRectangleLines(brick[i][j].position.x - brickSize.x / 2,
                           brick[i][j].position.y - brickSize.y / 2,
                           brickSize.x, brickSize.y, LIGHTGRAY);
      }
    }

    if (pause) {
      DrawText("GAME PAUSED",
               GetScreenWidth() / 2 - MeasureText("GAME PAUSED", 40) / 2,
               GetScreenHeight() / 2 - 40, 40, GRAY);
    }

    // Draw countdown timer
    if (!pause) {
      char stime[4];
      sprintf(stime, "%d", 60 - elapsedTime);
      Color textColor;
      if (elapsedTime >= 50) {
        textColor = timerColorRed;
      } else {
        textColor = timerColor;
      }
      DrawText(
          stime, GetScreenWidth() / 2 - MeasureText(stime, TIMER_FONT_SIZE) / 2,
          GetScreenHeight() / 2 - TIMER_FONT_SIZE, TIMER_FONT_SIZE, textColor);
    }

    break;
  }
  case ENDING: {
    ClearBackground(RAYWHITE);
    DrawText("GAME OVER",
             GetScreenWidth() / 2 - MeasureText("GAME OVER", 160) / 2,
             GetScreenHeight() / 2 - 160, 160, GRAY);

    char scoreLine[32] = "SCORE: ";
    char sscore[8];
    sprintf(sscore, "%d", score);
    strcat(scoreLine, sscore);

    DrawText(scoreLine, GetScreenWidth() / 2 - MeasureText(scoreLine, 100) / 2,
             GetScreenHeight() / 2 - 100 + 200, 100, GRAY);
    break;
  }
  default:
    break;
  }

  EndDrawing();
}

// Unload game variables
void UnloadGame(void) {
  UnloadMusicStream(titleMusic);
  UnloadMusicStream(bgMusic);
  UnloadMusicStream(endingMusic);
  UnloadSound(clickSound);
  UnloadSound(beepSound);
  CloseAudioDevice();
}

// Update and Draw (one frame)
void UpdateDrawFrame(void) {
  UpdateGame();
  DrawGame();
}
