#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <stdlib.h>

#define DURATION 30
#define LINES_OF_BRICKS 15
#define BRICKS_PER_LINE 20
#define FILE_NUM 5
#define ALPHA 40
#define MARGIN_LEFT 400
#define MARGIN_RIGHT 100
#define MARGIN_TOP 100
#define MARGIN_DOWN 100
#define TIMER_FONT_SIZE 150
#define SCORE_FONT_SIZE 150
#define STAGE_FONT_SIZE 150
#define LABEL_FONT_SIZE 50

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

static const Color TIMER_COLOR = GREEN;
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
static Music bgMusic;
static Texture2D bgTextureLayer1;
static Texture2D bgTextureLayer2;
static Texture2D bgTextureLayer3;
static Texture2D bgTextureLayer4;
static Texture2D bgTextureLayer5;
static Texture2D bgTextureLayer6;
static Texture2D bgTextureLayer7;
static Texture2D bgTextureLayer8;

float scrollingLayer1 = 0.0f;
float scrollingLayer2 = 0.0f;
float scrollingLayer3 = 0.0f;
float scrollingLayer4 = 0.0f;
float scrollingLayer5 = 0.0f;
float scrollingLayer6 = 0.0f;
float scrollingLayer7 = 0.0f;
float scrollingLayer8 = 0.0f;

static void chooseRandomBrick(int *x, int *y);
static void chooseNextTarget(int *nextX, int *nextY);

static void InitBackground();
static void UpdateBackground();
static void DrawBackground();
static void UnloadBackground();

void InitGameplayScreen() {
  finishScreen = false;
  framesCount = 0;
  baseTime = GetTime();

  clickSound = LoadSound("resources/click.mp3");
  beepSound = LoadSound("resources/beep.mp3");
  beepHighSound = LoadSound("resources/beep_high.mp3");
  bgMusic = LoadMusicStream("resources/bg.mp3");
  InitBackground();
  PlayMusicStream(bgMusic);

  // Set fragmentationLevel based on stage
  if (stage == 1) {
    fragmentationLevel = 5;
  }
  if (stage == 2) {
    fragmentationLevel = 50;
  }
  if (stage == 3) {
    fragmentationLevel = 95;
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
  UpdateMusicStream(bgMusic);

  UpdateBackground();

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
  // Draw background image
  DrawBackground();

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
    DrawLine(MARGIN_LEFT, MARGIN_TOP + i * brickSize.y,
             MARGIN_LEFT + BRICKS_PER_LINE * brickSize.x,
             MARGIN_TOP + i * brickSize.y, RAYWHITE);
  }

  for (int i = 0; i <= BRICKS_PER_LINE; i++) {
    DrawLine(MARGIN_LEFT + i * brickSize.x, MARGIN_TOP,
             MARGIN_LEFT + i * brickSize.x,
             MARGIN_TOP + LINES_OF_BRICKS * brickSize.y, RAYWHITE);
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
  DrawText("TIMER", 10, timerLabelY, LABEL_FONT_SIZE, TIMER_COLOR);

  char stime[4];
  int t = DURATION - elapsedTime;
  if (t < 0)
    t = 0;
  sprintf(stime, "%d", t);
  Color textColor;
  if (elapsedTime >= DURATION - 5) {
    textColor = TIMER_COLOR_ALARM;
  } else {
    textColor = TIMER_COLOR;
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
}

void UnloadGameplayScreen() {
  StopMusicStream(bgMusic);
  UnloadMusicStream(bgMusic);
  UnloadSound(clickSound);
  UnloadSound(beepSound);
  UnloadSound(beepHighSound);
  UnloadBackground();
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

void InitBackground() {
  bgTextureLayer1 = LoadTexture("resources/layer1.png");
  bgTextureLayer2 = LoadTexture("resources/layer2.png");
  bgTextureLayer3 = LoadTexture("resources/layer3.png");
  bgTextureLayer4 = LoadTexture("resources/layer4.png");
  bgTextureLayer5 = LoadTexture("resources/layer5.png");
  bgTextureLayer6 = LoadTexture("resources/layer6.png");
  bgTextureLayer7 = LoadTexture("resources/layer7.png");
  bgTextureLayer8 = LoadTexture("resources/layer8.png");
}

void UpdateBackground() {
  scrollingLayer1 -= 0.5f;
  scrollingLayer2 -= 1.0f;
  scrollingLayer3 -= 1.5f;
  scrollingLayer4 -= 2.0f;
  scrollingLayer5 -= 2.5f;
  scrollingLayer6 -= 3.0f;
  scrollingLayer7 -= 3.5f;
  scrollingLayer8 -= 4.0f;

  if (scrollingLayer1 <= -bgTextureLayer1.width)
    scrollingLayer1 = 0;
  if (scrollingLayer2 <= -bgTextureLayer2.width)
    scrollingLayer2 = 0;
  if (scrollingLayer3 <= -bgTextureLayer3.width)
    scrollingLayer3 = 0;
  if (scrollingLayer4 <= -bgTextureLayer4.width)
    scrollingLayer4 = 0;
  if (scrollingLayer5 <= -bgTextureLayer5.width)
    scrollingLayer5 = 0;
  if (scrollingLayer6 <= -bgTextureLayer6.width)
    scrollingLayer6 = 0;
  if (scrollingLayer7 <= -bgTextureLayer7.width)
    scrollingLayer7 = 0;
  if (scrollingLayer8 <= -bgTextureLayer8.width)
    scrollingLayer8 = 0;
}

void DrawBackground() {
  DrawTextureEx(bgTextureLayer1, (Vector2){scrollingLayer1, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer1,
                (Vector2){bgTextureLayer1.width + scrollingLayer1, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer2, (Vector2){scrollingLayer2, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer2,
                (Vector2){bgTextureLayer2.width + scrollingLayer2, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer3, (Vector2){scrollingLayer3, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer3,
                (Vector2){bgTextureLayer3.width + scrollingLayer3, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer4, (Vector2){scrollingLayer4, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer4,
                (Vector2){bgTextureLayer4.width + scrollingLayer4, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer5, (Vector2){scrollingLayer5, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer5,
                (Vector2){bgTextureLayer5.width + scrollingLayer5, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer6, (Vector2){scrollingLayer6, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer6,
                (Vector2){bgTextureLayer6.width + scrollingLayer6, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer7, (Vector2){scrollingLayer7, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer7,
                (Vector2){bgTextureLayer7.width + scrollingLayer7, 0}, 0.0f,
                1.0f, WHITE);

  DrawTextureEx(bgTextureLayer8, (Vector2){scrollingLayer8, 0}, 0.0f, 1.0f,
                WHITE);
  DrawTextureEx(bgTextureLayer8,
                (Vector2){bgTextureLayer8.width + scrollingLayer8, 0}, 0.0f,
                1.0f, WHITE);
}

void UnloadBackground() {
  UnloadTexture(bgTextureLayer1);
  UnloadTexture(bgTextureLayer2);
  UnloadTexture(bgTextureLayer3);
  UnloadTexture(bgTextureLayer4);
  UnloadTexture(bgTextureLayer5);
  UnloadTexture(bgTextureLayer6);
  UnloadTexture(bgTextureLayer7);
  UnloadTexture(bgTextureLayer8);
}
