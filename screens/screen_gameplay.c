#include "backgrounds.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_ROWS 100
#define MAX_COLS 100
#define DURATION 30
#define FILE_NUM 4
#define ALPHA 40
#define MARGIN_LEFT 500
#define MARGIN_RIGHT 100
#define MARGIN_TOP 100
#define MARGIN_DOWN 100
#define TIMER_FONT_SIZE 100
#define SCORE_FONT_SIZE 80
#define STAGE_FONT_SIZE 30
#define LABEL_FONT_SIZE 30
#define LEFT_PANEL_WIDTH 400

typedef enum { NORMAL, PENDING, HIDDEN } BrickState;

typedef struct Brick {
  Vector2 position;
  BrickState state;
  int file;
  double timestamp;
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

static const Color FILE_COLORS[FILE_NUM] = {
    (Color){2, 136, 209, 255}, (Color){22, 128, 57, 255},
    (Color){171, 71, 188, 255}, (Color){255, 82, 82, 255}};
static const Color FILE_COLORS_DARK[FILE_NUM] = {
    (Color){1, 51, 78, 255}, (Color){8, 48, 21, 255}, (Color){65, 26, 71, 255},
    (Color){126, 0, 0, 255}};
static const Color FILE_COLORS_LIGHT[FILE_NUM] = {
    (Color){73, 190, 253, 255}, (Color){66, 223, 118, 255},
    (Color){203, 142, 214, 255}, (Color){255, 149, 149, 255}};

static Brick brick[MAX_ROWS][MAX_COLS];
static int filesCounts[FILE_NUM] = {0};
static int filesTotalCounts[FILE_NUM] = {0};
static int files[FILE_NUM] = {0};
static Vector2 brickSize = {0};
static int currentFile = 0;
static int fragmentationLevel = 10; // from 0 - 100

static double lastPlaySound;
bool chooseNextFile(void);
bool allClear(void);
void finish(void);

void InitGameplayScreen() {
  finishScreen = false;
  framesCount = 0;
  tutorialBaseTime = GetTime();
  showTutorial = false;
  elapsedTime = 0;
  showCountDown = true;
  countDown = 3;
  countDownBaseTime = GetTime();
  lastPlaySound = 0;

  if (stage == 1) {
    num_rows = 10;
    num_cols = 15;
    fileChangeRate = 10 * 60;
    SetMusicVolume(stage1Music, 1.0f);
    PlayMusicStream(stage1Music);
    showTutorial = true;
  } else if (stage == 2) {
    num_rows = 10;
    num_cols = 15;
    fileChangeRate = 8 * 60;
    SetMusicVolume(stage2Music, 1.0f);
    PlayMusicStream(stage2Music);
  } else if (stage == STAGE_NUM) {
    num_rows = 10;
    num_cols = 15;
    fileChangeRate = 6 * 60;
    SetMusicVolume(stage3Music, 1.0f);
    PlayMusicStream(stage3Music);
  }

  // Set fragmentationLevel based on stage
  if (stage == 1) {
    fragmentationLevel = 5;
  }
  if (stage == 2) {
    fragmentationLevel = 20;
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
    files[i] = 0;
    filesTotalCounts[i] = 0;
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
      filesTotalCounts[brick[i][j].file]++;
      // mark file exists
      files[brick[i][j].file] = 1;
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
        finish();
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
              if (brick[i][j].timestamp <= GetTime()) {
                brick[i][j].state = HIDDEN;
                filesCounts[brick[i][j].file]--;
                score += 5;
                if (GetTime() - lastPlaySound >= 0.1) {
                  PlaySound(clickSound);
                  lastPlaySound = GetTime();
                }
                if (filesCounts[currentFile] == 0) {
                  if (!chooseNextFile()) {
                    finish();
                    goto end;
                  }
                }
                goto end;
              } else {
                brick[i][j].alpha = 0.6;
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

          double ts = GetTime();
          while (true) {
            if (brick[i][j].file == currentFile &&
                brick[i][j].state == NORMAL) {
              PlaySound(clickSound);
              brick[i][j].state = PENDING;
              brick[i][j].timestamp = ts;
              ts += 0.05;
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
            finish();
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
  /* DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, */
  /*               Fade(FILE_COLORS[currentFile], 0.3)); */

  // Draw bricks
  int shadow_thick = brickSize.x / 23;
  int inner_margin = brickSize.x / 10;
  Color empty_color = Fade(LIGHTGRAY, 0.3);
  int conn_thick_h = 5;
  int conn_thick_v = 3;

  // draw grid bg color
  int border_thick = 20;
  DrawRectangle(MARGIN_LEFT, MARGIN_TOP, brickSize.x * num_cols,
                brickSize.y * num_rows, Fade(BLACK, 0.7));
  if ((framesCount / 40) % 2 == 0) {
    DrawRectangleLinesEx((Rectangle){MARGIN_LEFT - border_thick,
                                     MARGIN_TOP - border_thick,
                                     brickSize.x * num_cols + 2 * border_thick,
                                     brickSize.y * num_rows + 2 * border_thick},
                         border_thick, FILE_COLORS[currentFile]);
  } else {
    DrawRectangleLinesEx((Rectangle){MARGIN_LEFT - border_thick,
                                     MARGIN_TOP - border_thick,
                                     brickSize.x * num_cols + 2 * border_thick,
                                     brickSize.y * num_rows + 2 * border_thick},
                         border_thick, Fade(FILE_COLORS[currentFile], 0.5));
  }

  for (int i = 0; i < num_rows; i++) {
    for (int j = 0; j < num_cols; j++) {
      Color conn_color_h;
      if (brick[i][j].state == PENDING) {
        conn_color_h = FILE_COLORS_LIGHT[brick[i][j].file];
      } else {
        conn_color_h = empty_color;
      }
      // draw grid bg
      DrawRectangle(MARGIN_LEFT + j * brickSize.x + inner_margin,
                    MARGIN_TOP + i * brickSize.y + inner_margin,
                    brickSize.x - 2 * inner_margin,
                    brickSize.y - 2 * inner_margin, empty_color);

      if (j != 0) {
        // right
        DrawLineEx((Vector2){MARGIN_LEFT + j * brickSize.x,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) / 3},
                   (Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) / 3},
                   conn_thick_h, conn_color_h);

        DrawLineEx((Vector2){MARGIN_LEFT + j * brickSize.x,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) * 2 / 3},
                   (Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) * 2 / 3},
                   conn_thick_h, conn_color_h);
      }

      if (j != num_cols - 1) {
        // left
        DrawLineEx((Vector2){MARGIN_LEFT + (j + 1) * brickSize.x - inner_margin,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) / 3},
                   (Vector2){MARGIN_LEFT + (j + 1) * brickSize.x,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) / 3},
                   conn_thick_h, conn_color_h);


        DrawLineEx((Vector2){MARGIN_LEFT + (j + 1) * brickSize.x - inner_margin,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) * 2 / 3},
                   (Vector2){MARGIN_LEFT + (j + 1) * brickSize.x,
                             MARGIN_TOP + i * brickSize.y + inner_margin +
                                 (brickSize.y - 2 * inner_margin) * 2 / 3},
                   conn_thick_h, conn_color_h);
      }

      if (i != 0) {
        // top
        DrawLineEx((Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) / 3,
                             MARGIN_TOP + i * brickSize.y},
                   (Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) / 3,
                             MARGIN_TOP + i * brickSize.y + inner_margin},
                   conn_thick_v, empty_color);
        DrawLineEx((Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) * 2 / 3,
                             MARGIN_TOP + i * brickSize.y},
                   (Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) * 2 / 3,
                             MARGIN_TOP + i * brickSize.y + inner_margin},
                   conn_thick_v, empty_color);
      }

      if (i != num_rows - 1) {
        // down
        DrawLineEx((Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) / 3,
                             MARGIN_TOP + (i + 1) * brickSize.y - inner_margin},
                   (Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) / 3,
                             MARGIN_TOP + (i + 1) * brickSize.y},
                   conn_thick_v, empty_color);
        DrawLineEx((Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) * 2 / 3,
                             MARGIN_TOP + (i + 1) * brickSize.y - inner_margin},
                   (Vector2){MARGIN_LEFT + j * brickSize.x + inner_margin +
                                 (brickSize.x - 2 * inner_margin) * 2 / 3,
                             MARGIN_TOP + (i + 1) * brickSize.y},
                   conn_thick_v, empty_color);
      }

      DrawRectangle(brick[i][j].position.x - brickSize.x / 2 + inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + inner_margin,
                    brickSize.x - inner_margin * 2,
                    brickSize.y - inner_margin * 2,
                    Fade(FILE_COLORS[brick[i][j].file], brick[i][j].alpha));

      // draw shadow lines
      // vertical left
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2 +
                        shadow_thick / 2 + inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + inner_margin},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 +
                        shadow_thick / 2 + inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y -
                        inner_margin},
          shadow_thick,
          Fade(FILE_COLORS_DARK[brick[i][j].file], brick[i][j].alpha));

      // horizontal down
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y -
                        shadow_thick / 2 - inner_margin},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x -
                        inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y -
                        shadow_thick / 2 - inner_margin},
          shadow_thick,
          Fade(FILE_COLORS_DARK[brick[i][j].file], brick[i][j].alpha));

      // horizontal top
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 +
                        shadow_thick / 2 + inner_margin},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x -
                        inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 +
                        shadow_thick / 2 + inner_margin},
          shadow_thick,
          Fade(FILE_COLORS_LIGHT[brick[i][j].file], brick[i][j].alpha));

      // vertical right
      DrawLineEx(
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x -
                        shadow_thick / 2 - inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + inner_margin},
          (Vector2){brick[i][j].position.x - brickSize.x / 2 + brickSize.x -
                        shadow_thick / 2 - inner_margin,
                    brick[i][j].position.y - brickSize.y / 2 + brickSize.y -
                        inner_margin},
          shadow_thick,
          Fade(FILE_COLORS_LIGHT[brick[i][j].file], brick[i][j].alpha));
    }
  }

  // Draw left panel background
  DrawRectangle(0, 0, LEFT_PANEL_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6));

  // Draw stage
  const char *sstage = TextFormat("Stage %d", stage);
  DrawText(sstage,
           (LEFT_PANEL_WIDTH - MeasureText(sstage, STAGE_FONT_SIZE)) / 2, 10,
           STAGE_FONT_SIZE, ORANGE);

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

  int timer_radius = 100;
  // 180 - 540
  Vector2 center = (Vector2){LEFT_PANEL_WIDTH / 2, timerY + 10};
  float startAng = 180;
  float endAng = 540 - (elapsedTime / DURATION) * 360;
  if (endAng < 180)
    endAng = 180;

  int t = DURATION - elapsedTime;
  if (t < 0)
    t = 0;
  const char *stime = FormatText("%d", t);

  DrawCircleSector(center, timer_radius, 180, 540, 200, Fade(color, 0.2));
  DrawCircleSector(center, timer_radius, startAng, endAng, 200,
                   Fade(color, 0.6));
  DrawText(stime, center.x - MeasureText(stime, TIMER_FONT_SIZE) / 2,
           center.y - TIMER_FONT_SIZE / 2, TIMER_FONT_SIZE, textColor);

  // Draw progress
  DrawText("Progress",
           (LEFT_PANEL_WIDTH - MeasureText("Progress", LABEL_FONT_SIZE)) / 2,
           650, LABEL_FONT_SIZE, WHITE);

  int margin = 20;
  int height = 20;
  int width = LEFT_PANEL_WIDTH - 2 * margin;
  int y = 650;
  for (int i = 0; i < FILE_NUM; i++) {
    if (files[i] == 1) {
      y += 50;
      DrawRectangleLines(margin, y, width, height, FILE_COLORS[i]);
      DrawRectangle(margin, y, width, height, Fade(FILE_COLORS[i], 0.3));
      DrawRectangle(margin, y,
                    width * (1 - (float)filesCounts[i] / filesTotalCounts[i]),
                    height, FILE_COLORS[i]);
    }
  }

  // Draw score
  int scoreLableY = SCREEN_HEIGHT - LABEL_FONT_SIZE - SCORE_FONT_SIZE - 30;
  DrawText("Score",
           (LEFT_PANEL_WIDTH - MeasureText("Score", LABEL_FONT_SIZE)) / 2,
           scoreLableY, LABEL_FONT_SIZE, WHITE);
  char sscore[32];
  sprintf(sscore, "%d", score);
  int scoreY = scoreLableY + LABEL_FONT_SIZE + 30;
  DrawText(sscore,
           (LEFT_PANEL_WIDTH - MeasureText(sscore, SCORE_FONT_SIZE)) / 2,
           scoreY, SCORE_FONT_SIZE, SKYBLUE);

  if (showCountDown) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8));
    const char *cd = FormatText("%d", countDown);
    DrawText(cd, (SCREEN_WIDTH - MeasureText(cd, 300)) / 2,
             (SCREEN_HEIGHT - 300) / 2, 300, RAYWHITE);
  }

  // draw tutorial
  if (showTutorial) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8));
    Rectangle rec = {400, 400, SCREEN_WIDTH - 800, 400};
    DrawTextRec(
        GetFontDefault(),
        "<--- Click on file blocks that have the same color as hinted on "
        "the left.",
        rec, 60, 5, true, RAYWHITE);
    DrawTriangle((Vector2){10, 60}, (Vector2){120, 560}, (Vector2){120, 640},
                 RAYWHITE);

    if ((framesCount / 40) % 2 == 0) {
      DrawText("Touch Screen to Start",
               SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 60) / 2,
               SCREEN_HEIGHT / 2 - 60 + 300, 60, LIGHTGRAY);
    }
  }

  // Draw current file
  DrawText("Read File",
           (LEFT_PANEL_WIDTH - MeasureText("Read File", LABEL_FONT_SIZE)) / 2,
           SCREEN_HEIGHT / 2 - 200, LABEL_FONT_SIZE, WHITE);
  int w = 280;
  int h = 150;
  DrawRectangle((LEFT_PANEL_WIDTH - w) / 2, (SCREEN_HEIGHT - h) / 2 - 60, w, h,
                FILE_COLORS[currentFile]);
}

void UnloadGameplayScreen() {
  if (stage == 1) {
    StopMusicStream(stage1Music);
  } else if (stage == 2) {
    StopMusicStream(stage2Music);
  } else if (stage == STAGE_NUM) {
    StopMusicStream(stage3Music);
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

void finish(void) {
  finishScreen = true;
  score += (DURATION - elapsedTime) * 5;
}
