#include "backgrounds.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 100
#define MAX_COLS 100
#define DURATION 30
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

typedef enum { NORMAL, PENDING, HIDDEN } BrickState;

typedef struct Brick {
  Vector2 position;
  BrickState state;
  int file;
  double alpha;
} Brick;

static int framesCount;
static bool finishScreen;
static int fileChangeRate;

static int num_rows;
static int num_cols;

static bool showTutorial;
static double tutorialBaseTime;
static double elapsedTime;

static bool showCountDown;
static double countDownBaseTime;
static int countDown;

static const Color TIMER_COLOR = LIGHTGRAY;
static const Color TIMER_TEXT_COLOR = BLACK;
static const Color TIMER_COLOR_ALARM = RED;

static const Color FILE_COLORS[FILE_NUM] = {BLUE, LIME, VIOLET, RED, GOLD};
static const Color FILE_COLORS_DARK[FILE_NUM] = {DARKBLUE, DARKGREEN,
                                                 DARKPURPLE, MAROON, ORANGE};
static const Color FILE_COLORS_LIGHT[FILE_NUM] = {SKYBLUE, GREEN, PURPLE, PINK,
                                                  YELLOW};

static Brick brick[MAX_ROWS][MAX_COLS];
static int filesCounts[FILE_NUM] = {0};
static Vector2 brickSize = {0};
static int currentFile = 0;
static int fragmentationLevel = 10; // from 0 - 100

static Sound clickSound;
static Sound beepSound;
static Sound beepHighSound;
static Music stage1Music;
static Music stage2Music;
static Music stage3Music;

bool chooseNextFile();
bool allClear();

void InitGameplayScreen() {
  finishScreen = false;
  framesCount = 0;
  tutorialBaseTime = GetTime();
  showTutorial = false;
  elapsedTime = 0;
  showCountDown = true;
  countDown = 3;
  countDownBaseTime = GetTime();

  clickSound = LoadSound("resources/sounds/click.mp3");
  beepSound = LoadSound("resources/sounds/beep.mp3");
  beepHighSound = LoadSound("resources/sounds/beep_high.mp3");

  if (stage == 1) {
    num_rows = 10;
    num_cols = 10;
    fileChangeRate = 10 * 60;
    InitStage1Background();
    stage1Music = LoadMusicStream("resources/music/stage1_music.mp3");
    SetMusicVolume(stage1Music, 1.0f);
    PlayMusicStream(stage1Music);
    showTutorial = true;
  } else if (stage == 2) {
    num_rows = 15;
    num_cols = 15;
    fileChangeRate = 8 * 60;
    InitStage2Background();
    stage2Music = LoadMusicStream("resources/music/stage2_music.mp3");
    SetMusicVolume(stage2Music, 1.0f);
    PlayMusicStream(stage2Music);
  } else if (stage == STAGE_NUM) {
    num_rows = 20;
    num_cols = 20;
    fileChangeRate = 6 * 60;
    InitStage3Background();
    stage3Music = LoadMusicStream("resources/music/stage3_music.mp3");
    SetMusicVolume(stage3Music, 1.0f);
    PlayMusicStream(stage3Music);
  }

  // Set fragmentationLevel based on stage
  if (stage == 1) {
    fragmentationLevel = 10;
  }
  if (stage == 2) {
    fragmentationLevel = 50;
  }
  if (stage == 3) {
    fragmentationLevel = 100;
  }

  // Calculate Brick size
  brickSize = (Vector2){(SCREEN_WIDTH - MARGIN_LEFT - MARGIN_RIGHT) / num_cols,
                        (SCREEN_HEIGHT - MARGIN_TOP - MARGIN_DOWN) / num_rows};

  // Init files counts
  for (int i = 0; i < FILE_NUM; i++) {
    filesCounts[i] = 0;
  }

  // Generate a bricks map
  int prevFile = 0;
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      brick[i][j].position =
          (Vector2){j * brickSize.x + brickSize.x / 2 + MARGIN_LEFT,
                    i * brickSize.y + brickSize.y / 2 + MARGIN_TOP};
      brick[i][j].state = NORMAL;
      brick[i][j].alpha = 1;
      if (rand() % 100 <= 100 - fragmentationLevel) {
        brick[i][j].file = prevFile;
      } else {
        int file = rand() % FILE_NUM;
        brick[i][j].file = file;
        prevFile = file;
      }
      filesCounts[brick[i][j].file]++;
    }
  }

  // select current file
  chooseNextFile();
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

  if (showTutorial) {
    showCountDown = false;
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      showTutorial = false;
      showCountDown = true;
      countDownBaseTime = GetTime();
    }
  } else {
    if (showCountDown) {
      if (GetTime() - countDownBaseTime < 1) {
        countDown = 3;
      } else if (GetTime() - countDownBaseTime < 2) {
        countDown = 2;
      } else if (GetTime() - countDownBaseTime < 3) {
        countDown = 1;
      } else {
        showCountDown = false;
        tutorialBaseTime = GetTime();
      }
    } else {
      int prevElapseTime = elapsedTime;
      // Calculate elapsed time
      elapsedTime = GetTime() - tutorialBaseTime;
      if (elapsedTime > DURATION) {
        // Times up!
        finishScreen = true;
      } else {
        if ((int)elapsedTime >= DURATION - 6 &&
            (int)elapsedTime <= DURATION - 1) {
          if ((int)elapsedTime == prevElapseTime + 1) {
            if ((int)elapsedTime == DURATION - 1) {
              PlaySound(beepHighSound);
            } else {
              PlaySound(beepSound);
            }
          }
        }

        for (int i = 0; i < num_rows; i++) {
          for (int j = 0; j < num_cols; j++) {
            if (brick[i][j].state == PENDING) {
              if (framesCount % 2 == 0) {
                brick[i][j].state = HIDDEN;
                filesCounts[brick[i][j].file]--;
                score += 5;
                if (framesCount % 6 == 0) {
                  PlaySound(clickSound);
                }
                if (filesCounts[currentFile] == 0) {
                  if (!chooseNextFile()) {
                    finishScreen = true;
                    goto end;
                  }
                }
                goto end;
              }
            }
          }
        }
      end:

        for (int i = 0; i < num_rows; i++) {
          for (int j = 0; j < num_cols; j++) {
            if (brick[i][j].state == HIDDEN) {
              if (framesCount % 2 == 0) {
                if (brick[i][j].position.y + brickSize.y / 2 > 0) {
                  brick[i][j].position.y -= 50;
                }
                if (brick[i][j].alpha > 0) {
                  brick[i][j].alpha -= 0.1;
                }
              }
            }
          }
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
          // Handle brick clicked
          Vector2 mousePos = GetMousePosition();
          int i = (mousePos.y - MARGIN_TOP) / brickSize.y;
          int j = (mousePos.x - MARGIN_LEFT) / brickSize.x;

          while (true) {
            if (brick[i][j].file == currentFile &&
                brick[i][j].state == NORMAL) {
              PlaySound(clickSound);
              brick[i][j].state = PENDING;
              // go to next grid
              if (j == num_cols - 1) {
                if (i == num_rows - 1) {
                  break;
                } else {
                  i++;
                  j = 0;
                }
              } else {
                j++;
              }
            } else {
              break;
            }
          }
        }

        // update current file
        if (framesCount % fileChangeRate == 0) {
          if (!chooseNextFile()) {
            finishScreen = true;
          }
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

  // draw hint color
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
                Fade(FILE_COLORS[currentFile], 0.3));

  // Draw bricks
  int shadow_thick = brickSize.x / 20;
  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      DrawRectangle(brick[i][j].position.x - brickSize.x / 2,
                    brick[i][j].position.y - brickSize.y / 2, brickSize.x,
                    brickSize.y,
                    Fade(FILE_COLORS[brick[i][j].file], brick[i][j].alpha));

      // draw shadow lines
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + shadow_thick / 2,
                    brick[i][j].position.y - brickSize.y / 2},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + shadow_thick / 2,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y},
          shadow_thick,
          Fade(FILE_COLORS_DARK[brick[i][j].file], brick[i][j].alpha));
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y -
                        shadow_thick / 2},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y -
                        shadow_thick / 2},
          shadow_thick,
          Fade(FILE_COLORS_DARK[brick[i][j].file], brick[i][j].alpha));
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2,
                    brick[i][j].position.y - brickSize.y / 2 +
                        shadow_thick / 2},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x,
                    brick[i][j].position.y - brickSize.y / 2 +
                        shadow_thick / 2},
          shadow_thick,
          Fade(FILE_COLORS_LIGHT[brick[i][j].file], brick[i][j].alpha));
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x -
                        shadow_thick / 2,
                    brick[i][j].position.y - brickSize.y / 2},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x -
                        shadow_thick / 2,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y},
          shadow_thick,
          Fade(FILE_COLORS_LIGHT[brick[i][j].file], brick[i][j].alpha));
    }
  }

  // Draw stage
  const char *sstage = TextFormat("STAGE %d", stage);
  DrawRectangle(0, 0, MeasureText(sstage, STAGE_FONT_SIZE) + 40,
                STAGE_FONT_SIZE + 20, Fade(BLACK, 0.2));
  DrawText(sstage, 20, 10, STAGE_FONT_SIZE, ORANGE);

  // Draw countdown timer
  Color color;
  Color textColor;
  if (elapsedTime >= DURATION - 6) {
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
  float endAng = 540 - (elapsedTime / DURATION) * 360;
  if (endAng < 180)
    endAng = 180;

  int t = DURATION - elapsedTime;
  if (t < 0)
    t = 0;
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

  if (showCountDown) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8));
    const char *cd = FormatText("%d", countDown);
    DrawText(cd, (SCREEN_WIDTH - MeasureText(cd, 300)) / 2,
             (SCREEN_HEIGHT - 300) / 2, 300, RAYWHITE);
  }

  // draw tutorial
  if (showTutorial) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8));
    Rectangle rec = {300, 550, SCREEN_WIDTH - 800, 400};
    DrawTextRec(GetFontDefault(),
                "<--- Click on the boxes that has the same color as hinted on "
                "the left.",
                rec, 60, 5, true, RAYWHITE);
    DrawTriangle((Vector2){10, 60}, (Vector2){120, 560}, (Vector2){120, 640},
                 RAYWHITE);

    if ((framesCount / 40) % 2 == 0) {
      DrawText("Touch Screen to Start",
               SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 30) / 2,
               SCREEN_HEIGHT / 2 - 30 + 300, 30, LIGHTGRAY);
    }
  }

  // Draw current file
  DrawText("Pick color", 10, SCREEN_HEIGHT / 2 - 50, LABEL_FONT_SIZE,
           FILE_COLORS[currentFile]);
  DrawRectangle(10, SCREEN_HEIGHT / 2, 200, 150, FILE_COLORS[currentFile]);
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

bool allClear() {
  for (int i = 0; i < FILE_NUM; i++) {
    if (filesCounts[i] > 0) {
      return false;
    }
  }
  return true;
}

bool chooseNextFile() {
  if (allClear()) {
    return false;
  }

  int result = rand() % FILE_NUM;
  int count;
  count = filesCounts[result];
  while (count == 0) {
    result = (result + 1) % FILE_NUM;
    count = filesCounts[result];
  }
  currentFile = result;
  return true;
}
