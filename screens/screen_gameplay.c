#include "backgrounds.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <stdlib.h>

#define DURATION 30
#define LINES_OF_BRICKS 15
#define BRICKS_PER_LINE 15
#define FILE_NUM 5
#define ALPHA 40
#define MARGIN_LEFT 400
#define MARGIN_RIGHT 100
#define MARGIN_TOP 100
#define MARGIN_DOWN 100
#define TIMER_FONT_SIZE 150
#define SCORE_FONT_SIZE 100
#define STAGE_FONT_SIZE 50
#define LABEL_FONT_SIZE 30

typedef struct Brick {
  Vector2 position;
  bool active;
  int file;
} Brick;

static int framesCount;
static bool finishScreen;

static bool pause = false;
static int baseTime = 0;
static int pauseTime = 0;
static int elapsedTime = 0;

static const Color TIMER_COLOR = LIME;
static const Color TIMER_TEXT_COLOR = GREEN;
static const Color TIMER_COLOR_ALARM = RED;

static const Color FILE_COLORS[FILE_NUM] = {SKYBLUE, GREEN, PURPLE, PINK,
                                            ORANGE};

static Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE];
static Vector2 brickSize = {0};
static int targetX = 0;
static int targetY = 0;
static int fragmentationLevel = 10; // from 0 - 100

static Sound clickSound;
static Sound beepSound;
static Sound beepHighSound;
static Music stage1Music;
static Music stage2Music;
static Music stage3Music;

static void chooseRandomBrick(int *x, int *y);
static void chooseNextTarget(int *nextX, int *nextY);

void InitGameplayScreen() {
  finishScreen = false;
  framesCount = 0;
  baseTime = GetTime();

  clickSound = LoadSound("resources/sounds/click.mp3");
  beepSound = LoadSound("resources/sounds/beep.mp3");
  beepHighSound = LoadSound("resources/sounds/beep_high.mp3");

  if (stage == 1) {
    InitStage1Background();
    stage1Music = LoadMusicStream("resources/music/stage1_music.mp3");
    SetMusicVolume(stage1Music, 1.0f);
    PlayMusicStream(stage1Music);
  } else if (stage == 2) {
    InitStage2Background();
    stage2Music = LoadMusicStream("resources/music/stage2_music.mp3");
    SetMusicVolume(stage2Music, 1.0f);
    PlayMusicStream(stage2Music);
  } else if (stage == STAGE_NUM) {
    InitStage3Background();
    stage3Music = LoadMusicStream("resources/music/stage3_music.mp3");
    SetMusicVolume(stage3Music, 1.0f);
    PlayMusicStream(stage3Music);
  }

  // Set fragmentationLevel based on stage
  if (stage == 1) {
    fragmentationLevel = 5;
  }
  if (stage == 2) {
    fragmentationLevel = 15;
  }
  if (stage == 3) {
    fragmentationLevel = 100;
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
      if (rand() % 100 <= 100 - fragmentationLevel) {
        brick[i][j].file = prevFile;
      } else {
        int file = rand() % FILE_NUM;
        brick[i][j].file = file;
        prevFile = file;
      }
    }
  }
}

void UpdateGameplayScreen() {
  framesCount++;

  if (stage == 1) {
    UpdateStage1Background();
    UpdateMusicStream(stage1Music);
  } else if (stage == 2) {
    UpdateStage2Background();
    UpdateMusicStream(stage2Music);
  } else if (stage == STAGE_NUM) {
    UpdateStage3Background();
    UpdateMusicStream(stage3Music);
  }

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
      finishScreen = true;
    } else {
      if (elapsedTime >= DURATION - 5 && elapsedTime <= DURATION) {
        if (elapsedTime == prevElapseTime + 1) {
          if (elapsedTime == DURATION) {
            PlaySound(beepHighSound);
          } else {
            PlaySound(beepSound);
          }
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

          // Increase score
          score += 5;

          // Play sound
          PlaySound(clickSound);
        }
      }
    }
  }
}

void DrawGameplayScreen() {
  if (stage == 1) {
    DrawStage1Background();
  } else if (stage == 2) {
    DrawStage2Background();
  } else if (stage == STAGE_NUM) {
    DrawStage3Background();
  }

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
          color = BLANK;
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
    DrawLineEx((Vector2){MARGIN_LEFT, MARGIN_TOP + i * brickSize.y},
               (Vector2){MARGIN_LEFT + BRICKS_PER_LINE * brickSize.x,
                         MARGIN_TOP + i * brickSize.y},
               6, BLACK);
  }

  for (int i = 0; i <= BRICKS_PER_LINE; i++) {
    DrawLineEx((Vector2){MARGIN_LEFT + i * brickSize.x, MARGIN_TOP},
               (Vector2){MARGIN_LEFT + i * brickSize.x,
                         MARGIN_TOP + LINES_OF_BRICKS * brickSize.y},
               6, BLACK);
  }

  if (pause) {
    DrawText("GAME PAUSED",
             SCREEN_WIDTH / 2 - MeasureText("GAME PAUSED", 40) / 2,
             SCREEN_HEIGHT / 2 - 40, 40, GRAY);
  }

  // Draw stage
  const char *sstage = TextFormat("STAGE %d", stage);
  DrawRectangle(0, 0, MeasureText(sstage, STAGE_FONT_SIZE) + 40,
                STAGE_FONT_SIZE + 20, Fade(BLACK, 0.2));
  DrawText(sstage, 20, 10, STAGE_FONT_SIZE, ORANGE);

  // Draw countdown timer
  Color color;
  Color textColor;
  if (elapsedTime >= DURATION - 5) {
    color = TIMER_COLOR_ALARM;
    textColor = TIMER_COLOR_ALARM;
  } else {
    color = TIMER_COLOR;
    textColor = TIMER_TEXT_COLOR;
  }
  int timerY = LABEL_FONT_SIZE + 150;

  // 180 - 540
  Vector2 center = (Vector2){160, timerY + 100};
  float startAng = 180;
  float endAng = 540 - ((float)elapsedTime / DURATION) * 360;
  if (endAng < 180)
    endAng = 180;

  int t = DURATION - elapsedTime;
  if (t < 0) t = 0;
  const char *stime = FormatText("%d", t);

  DrawCircleSector(center, 150, 180, 540, 200, Fade(color, 0.2));
  DrawCircleSector(center, 150, startAng, endAng, 200, Fade(color, 0.6));
  DrawText(stime, center.x - MeasureText(stime, TIMER_FONT_SIZE) / 2,
           center.y - TIMER_FONT_SIZE / 2, TIMER_FONT_SIZE, textColor);

  // Draw score
  int scoreLableY = SCREEN_HEIGHT - LABEL_FONT_SIZE - SCORE_FONT_SIZE - 100;
  DrawText("SCORE", 10, scoreLableY, LABEL_FONT_SIZE, SKYBLUE);
  char sscore[32];
  sprintf(sscore, "%d", score);
  int scoreY = scoreLableY + LABEL_FONT_SIZE + 30;
  DrawText(sscore, 10, scoreY, SCORE_FONT_SIZE, SKYBLUE);
}

void UnloadGameplayScreen() {
  UnloadSound(clickSound);
  UnloadSound(beepSound);
  UnloadSound(beepHighSound);
  if (stage == 1) {
    UnloadStage1Background();
    StopMusicStream(stage1Music);
    UnloadMusicStream(stage1Music);
  } else if (stage == 2) {
    UnloadStage2Background();
    StopMusicStream(stage2Music);
    UnloadMusicStream(stage2Music);
  } else if (stage == STAGE_NUM) {
    UnloadStage3Background();
    StopMusicStream(stage3Music);
    UnloadMusicStream(stage3Music);
  }
}

bool FinishGameplayScreen() { return finishScreen; }

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
