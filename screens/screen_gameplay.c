#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <stdlib.h>

#define DURATION 30
#define LINES_OF_BRICKS 15
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

typedef struct Brick {
  Vector2 position;
  bool active;
  int file;
} Brick;

static int framesCount;
static bool finishScreen;

bool pause = false;
int baseTime = 0;
int pauseTime = 0;
int elapsedTime = 0;

Vector2 cursorPos = {0};

static const Color timerColor = GREEN;
static const Color timerColorAlarm = RED;

const Color FILE_COLORS[FILE_NUM] = {SKYBLUE, GREEN, PURPLE, PINK, ORANGE};

Brick brick[LINES_OF_BRICKS][BRICKS_PER_LINE] = {0};
Vector2 brickSize = {0};
int targetX = 0;
int targetY = 0;
int fragmentationLevel = 3; // from 0 - 10

Sound clickSound;
Sound beepSound;
Music bgMusic;

void chooseRandomBrick(int *x, int *y);
void chooseNextTarget(int *nextX, int *nextY);

void InitGameplayScreen() {
  finishScreen = false;
  framesCount = 0;
  baseTime = GetTime();

  clickSound = LoadSound("resources/click.mp3");
  beepSound = LoadSound("resources/beep.mp3");
  
  bgMusic = LoadMusicStream("resources/bg.mp3");
  PlayMusicStream(bgMusic);

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

void UpdateGameplayScreen() {
  framesCount++;
  UpdateMusicStream(bgMusic);
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
}

void UnloadGameplayScreen() {
  StopMusicStream(bgMusic);
  UnloadMusicStream(bgMusic);
  UnloadSound(clickSound);
  UnloadSound(beepSound);
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
