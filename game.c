#include "raylib.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define DURATION 30
#define LINES_OF_BRICKS 30
#define BRICKS_PER_LINE 20
#define FILE_NUM 5
#define ALPHA 40
#define MARGIN_LEFT 300
#define MARGIN_RIGHT 50
#define MARGIN_TOP 50
#define MARGIN_DOWN 50
#define TIMER_FONT_SIZE 150
#define SCORE_FONT_SIZE 150
#define STAGE_FONT_SIZE 150
#define LABEL_FONT_SIZE 50
#define STAGE_NUM 3

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
//-------------------v-----------------------------------------------------------------
static GameScreen currentScreen;
static int stage = 1;
static bool pause = false;
static bool finish = false;
static bool quit = false;
static int score = 0;
static int highestScore = 0;
static bool prize = false;
static int baseTime = 0;
static int pauseTime = 0;
static int elapsedTime = 0;

static Vector2 cursorPos = {0};

static Color timerColor;
static Color timerColorAlarm;

static const Color FILE_COLORS[FILE_NUM] = {SKYBLUE, GREEN, PURPLE, PINK,
                                            ORANGE};

static Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE] = {0};
static Vector2 brickSize = {0};
static int targetX = 0;
static int targetY = 0;
static int framesCount = 0;
static int fragmentationLevel = 3; // from 0 - 10

static Rectangle closeButtonRec;
static Rectangle playAgainRec;

static Sound clickSound;
static Sound beepSound;
static Sound stageClearSound;
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
  closeButtonRec = (Rectangle){SCREEN_WIDTH - 60, 0, 60, 60};
  playAgainRec =
      (Rectangle){SCREEN_WIDTH / 2 - 300,
                  SCREEN_HEIGHT / 2 - 200 - 160 + 200 + 200 + 300, 600, 150};
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fragmentation Game");
  InitAudioDevice();

  timerColor = Fade(GREEN, 0.5);
  timerColorAlarm = Fade(RED, 0.5);

  clickSound = LoadSound("resources/click.mp3");
  beepSound = LoadSound("resources/beep.mp3");
  stageClearSound = LoadSound("resources/clear.mp3");
  titleMusic = LoadMusicStream("resources/title.mp3");
  bgMusic = LoadMusicStream("resources/bg.mp3");
  endingMusic = LoadMusicStream("resources/ending.mp3");
  InitGame();
  //ToggleFullscreen();
  HideCursor();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60);
  while (!quit && !WindowShouldClose()) {
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

  prize = false;

  closeButtonRec = (Rectangle){SCREEN_WIDTH - 80, 0, 80, 80};

  // Set fragmentationLevel based on stage
  if (stage == 1) {
    fragmentationLevel = 2;
  }
  if (stage == 2) {
    fragmentationLevel = 4;
  }
  if (stage == 3) {
    fragmentationLevel = 10;
  }

  // Calculate Brick size
  brickSize =
      (Vector2){(SCREEN_WIDTH - MARGIN_LEFT - MARGIN_RIGHT) / BRICKS_PER_LINE,
                (SCREEN_HEIGHT - MARGIN_TOP - MARGIN_DOWN) / LINES_OF_BRICKS};

  // Select a random target brick
  targetX = rand() % BRICKS_PER_LINE;
  targetY = rand() % LINES_OF_BRICKS;

  // Generate a bricks map
  int prevFile = 0;
  for (int i = 0; i < LINES_OF_BRICKS; i++) {
    for (int j = 0; j < BRICKS_PER_LINE; j++) {
      brick[i][j].position =
          (Vector2){j * brickSize.x + brickSize.x / 2 + MARGIN_LEFT,
                    i * brickSize.y + brickSize.y / 2 + MARGIN_TOP};

      brick[i][j].active = false;
      if (rand() % 10 <= 10 - fragmentationLevel) {
        brick[i][j].file = prevFile;
      } else {
        int file = rand() % FILE_NUM;
        brick[i][j].file = file;
        prevFile = file;
      }
    }
  }
}

void chooseRandomBrick(int *x, int *y) {
  do {
    *x = rand() % BRICKS_PER_LINE;
    *y = rand() % LINES_OF_BRICKS;
  } while (brick[*y][*x].active);
}

void chooseNextTarget(int *nextX, int *nextY) {
  if (targetX == BRICKS_PER_LINE - 1 && targetY == LINES_OF_BRICKS - 1) {
    // target is last brick
    chooseRandomBrick(nextX, nextY);
  } else {
    int nX = (targetX + 1) % BRICKS_PER_LINE;
    int nY;
    if (targetX == BRICKS_PER_LINE - 1) {
      nY = targetY + 1;
    } else {
      nY = targetY;
    }
    if (brick[nY][nX].file == brick[targetY][targetX].file &&
        !brick[nY][nX].active) {
      if (rand() % 10 <= 9) {
        *nextX = nX;
        *nextY = nY;
      } else {
        chooseRandomBrick(nextX, nextY);
      }
    } else {
      chooseRandomBrick(nextX, nextY);
    }
  }
}

void goToPlay() {
  currentScreen = GAMEPLAY;
  baseTime = GetTime();
  StopMusicStream(titleMusic);
  PlayMusicStream(bgMusic);
  StopMusicStream(endingMusic);
}

void goToEnding() {
  currentScreen = ENDING;
  framesCount = 0;
  StopMusicStream(titleMusic);
  StopMusicStream(bgMusic);
  if (stage == STAGE_NUM) {
    PlayMusicStream(endingMusic);
  } else {
    PlaySound(stageClearSound);
  }
  if (score > highestScore) {
    highestScore = score;
    prize = true;
  }
}

void goToTitle() {
  score = 0;
  stage = 1;
  InitGame();
}

// Update game (one frame)
void UpdateGame(void) {
  cursorPos = GetMousePosition();
  // Update framesCount
  framesCount++;

  switch (currentScreen) {
  case TITLE: {
    UpdateMusicStream(titleMusic);
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mousePos = GetMousePosition();
      if (CheckCollisionPointRec(mousePos, closeButtonRec)) {
        quit = true;
      } else {
        goToPlay();
      }
    }
    break;
  }
  case GAMEPLAY: {
    UpdateMusicStream(bgMusic);
    if (finish) {
      goToEnding();
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
        int prevElapseTime = elapsedTime;
        // Calculate elapsed time
        elapsedTime = GetTime() - baseTime;

        if (elapsedTime > DURATION) {
          // Times up!
          finish = true;

        } else {
          if (elapsedTime >= DURATION - 5 && elapsedTime <= DURATION) {
            if (elapsedTime == prevElapseTime + 1) {
              PlaySound(beepSound);
            }
          }

          if (IsMouseButtonPressed(0)) {
            // Handle brick clicked
            Vector2 mousePos = GetMousePosition();
            int i = (mousePos.y - MARGIN_TOP) / brickSize.y;
            int j = (mousePos.x - MARGIN_LEFT) / brickSize.x;

            if (i == targetY && j == targetX) {
              int nextX, nextY;
              chooseNextTarget(&nextX, &nextY);
              targetX = nextX;
              targetY = nextY;

              brick[i][j].active = true;

              // Reset framesCount
              framesCount = 0;

              // Increase score
              score += 5;

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
    if (stage == STAGE_NUM) {
      UpdateMusicStream(endingMusic);
    }
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      Vector2 mousePos = GetMousePosition();
      if (CheckCollisionPointRec(mousePos, playAgainRec)) {
        if (stage < STAGE_NUM) {
          stage++;
          InitGame();
          goToPlay();
        } else {
          goToTitle();
        }
      }
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

    // Game Title
    DrawText("FRAGMENTATION GAME",
             SCREEN_WIDTH / 2 - MeasureText("FRAGMENTATION GAME", 120) / 2,
             SCREEN_HEIGHT / 2 - 120, 120, RED);

    // Touch to start
    if ((framesCount / 40) % 2 == 0) {
      DrawText("Touch Screen to Start",
               SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 80) / 2,
               SCREEN_HEIGHT / 2 - 80 + 300, 80, GRAY);
    } else {
      DrawText("Touch Screen to Start",
               SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 80) / 2,
               SCREEN_HEIGHT / 2 - 80 + 300, 80, RAYWHITE);
    }

    // Close button
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
        Color dark = FILE_COLORS[brick[i][j].file];
        Color light = Fade(dark, 0.4);
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
          Color color;
          if (brick[i][j].active) {
            color = RAYWHITE;
          } else {
            color = dark;
          }
          DrawRectangle(brick[i][j].position.x - brickSize.x / 2,
                        brick[i][j].position.y - brickSize.y / 2, brickSize.x,
                        brickSize.y, color);
        }
      }
    }

    // Draw grid
    for (int i = 0; i <= LINES_OF_BRICKS; i++) {
      DrawLine(MARGIN_LEFT, MARGIN_TOP + i * brickSize.y,
               MARGIN_LEFT + BRICKS_PER_LINE * brickSize.x,
               MARGIN_TOP + i * brickSize.y, LIGHTGRAY);
    }

    for (int i = 0; i <= BRICKS_PER_LINE; i++) {
      DrawLine(MARGIN_LEFT + i * brickSize.x, MARGIN_TOP,
               MARGIN_LEFT + i * brickSize.x,
               MARGIN_TOP + LINES_OF_BRICKS * brickSize.y, LIGHTGRAY);
    }

    if (pause) {
      DrawText("GAME PAUSED",
               SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2,
               SCREEN_HEIGHT / 2 - 40, 40, GRAY);
    }

    // Draw stage
    DrawText("STAGE", 10, MARGIN_TOP, LABEL_FONT_SIZE, ORANGE);

    char sstage[8];
    sprintf(sstage, "%d", stage);
    int stageY = MARGIN_TOP + LABEL_FONT_SIZE + 30;
    DrawText(sstage, 10, stageY, STAGE_FONT_SIZE, ORANGE);

    // Draw countdown timer
    int timerLabelY = stageY + 250;
    DrawText("TIMER", 10, timerLabelY, LABEL_FONT_SIZE, timerColor);

    char stime[4];
    int t = DURATION - elapsedTime;
    if (t < 0)
      t = 0;
    sprintf(stime, "%d", t);
    Color textColor;
    if (elapsedTime >= DURATION - 5) {
      textColor = timerColorAlarm;
    } else {
      textColor = timerColor;
    }
    int timerY = timerLabelY + LABEL_FONT_SIZE + 30;
    DrawText(stime, 10, timerY, TIMER_FONT_SIZE, textColor);

    // Draw score
    int scoreLableY = timerY + 250;
    DrawText("SCORE", 10, scoreLableY, LABEL_FONT_SIZE, SKYBLUE);
    char sscore[32];
    sprintf(sscore, "%d", score);
    int scoreY = scoreLableY + LABEL_FONT_SIZE + 30;
    DrawText(sscore, 10, scoreY, SCORE_FONT_SIZE, SKYBLUE);
    break;
  }
  case ENDING: {
    ClearBackground(RAYWHITE);

    if (stage < STAGE_NUM) {
      DrawText("WELL DONE!",
               SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 160) / 2,
               SCREEN_HEIGHT / 2 - 200 - 160, 160, GRAY);
    } else {
      DrawText("GAME OVER",
               SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 160) / 2,
               SCREEN_HEIGHT / 2 - 200 - 160, 160, GRAY);
    }

    char scoreLine[32] = "SCORE: ";
    char sscore[8];
    sprintf(sscore, "%d", score);
    strcat(scoreLine, sscore);

    DrawText(scoreLine, SCREEN_WIDTH / 2 - MeasureText(scoreLine, 100) / 2,
             SCREEN_HEIGHT / 2 - 200 - 160 + 200, 100, GRAY);

    if (prize) {
      if ((framesCount / 40) % 2 == 0) {
        DrawText("NEW RECORD",
                 SCREEN_WIDTH / 2 - MeasureText("NEW RECORD", 100) / 2,
                 SCREEN_HEIGHT / 2 - 200 - 160 + 200 + 200, 100, RED);
      }
    }
    DrawRectangleRec(playAgainRec, SKYBLUE);
    if (stage < STAGE_NUM) {
      DrawText("NEXT STAGE",
               playAgainRec.x +
                   (playAgainRec.width - MeasureText("NEXT STAGE", 60)) / 2,
               playAgainRec.y + (playAgainRec.height - 60) / 2, 60, BLUE);
    } else {
      DrawText("PLAY AGAIN",
               playAgainRec.x +
                   (playAgainRec.width - MeasureText("PLAY AGAIN", 60)) / 2,
               playAgainRec.y + (playAgainRec.height - 60) / 2, 60, BLUE);
    }
    break;
  }
  default:
    break;
  }

  DrawLineEx(cursorPos, (Vector2){cursorPos.x + 40, cursorPos.y + 40}, 8, BLACK);
  DrawLineEx(cursorPos, (Vector2){cursorPos.x + 30, cursorPos.y}, 8, BLACK);
  DrawLineEx(cursorPos, (Vector2){cursorPos.x, cursorPos.y + 30}, 8, BLACK);
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
