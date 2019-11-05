#include "raylib.h"
#include "screens.h"
#include <stdlib.h>
#include <stdio.h>

#define TITLE_GRID_ROWS 30
#define TITLE_GRID_COLS 60
#define TITLE_COLOR_NUM 6
#define TITLE_FONT_SIZE 200
#define SUBTITLE_FONT_SIZE 80

static const Color TITLE_COLORS[TITLE_COLOR_NUM] = {
    (Color){147, 58, 22, 180}, (Color){141, 2, 31, 180},
    (Color){94, 25, 20, 180},  (Color){184, 15, 10, 180},
    (Color){66, 13, 9, 180},   (Color){255, 40, 0, 180}};

static const char *subtitle = "Time to fight files fragmentation!";
static Color titleColors[TITLE_GRID_ROWS][TITLE_GRID_COLS] = {0};

static int finishScreen;
static int framesCount;
static Rectangle closeButtonRec;
static Rectangle leaderboardRec;

void UpdateTitleGridColors();

void InitTitleScreen() {
  framesCount = 0;
  closeButtonRec = (Rectangle){SCREEN_WIDTH - 80, 0, 80, 80};
  leaderboardRec = (Rectangle){10, 10, 64, 64};
  finishScreen = 0;
  PlayMusicStream(titleMusic);
  UpdateTitleGridColors();
}

void UpdateTitleScreen() {
  framesCount++;
  if (framesCount % 30 == 0) {
    UpdateTitleGridColors();
  }
  UpdateMusicStream(titleMusic);
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, closeButtonRec)) {
      quit = true;
    } else if (CheckCollisionPointRec(mousePos, leaderboardRec)) {
      finishScreen = 2;
      PlaySound(startSound);
    } else {
      finishScreen = 1;
      PlaySound(startSound);
    }
  }
}

void DrawTitleScreen() {
  // Draw grid anim
  double w = (double)SCREEN_WIDTH / TITLE_GRID_COLS;
  double h = (double)SCREEN_HEIGHT / TITLE_GRID_ROWS;
  for (int i = 0; i < TITLE_GRID_ROWS; i++) {
    for (int j = 0; j < TITLE_GRID_COLS; j++) {
      DrawRectangle(j * w, i * h, w, h, titleColors[i][j]);
    }
  }

  int textY = (SCREEN_HEIGHT - TITLE_FONT_SIZE) / 2 - 250;

  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7));

  DrawRectangleRounded((Rectangle) {150, textY - 100, SCREEN_WIDTH - 300, 350}, 0.2, 100, RED);

  // Game Title
  DrawTexture(gameTitle, (SCREEN_WIDTH - gameTitle.width) / 2, 200, WHITE);

  // sub title
  DrawText(subtitle, (SCREEN_WIDTH - MeasureText(subtitle, SUBTITLE_FONT_SIZE)) / 2, 520, SUBTITLE_FONT_SIZE, RAYWHITE);

  // Touch to start
  if ((framesCount / 40) % 2 == 0) {
    DrawText("Touch Screen to Start",
             SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 60) / 2,
             SCREEN_HEIGHT / 2 - 60 + 400, 60, BLANK);
  } else {
    DrawText("Touch Screen to Start",
             SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 60) / 2,
             SCREEN_HEIGHT / 2 - 60 + 400, 60, RAYWHITE);
  }

  // Close button
  DrawRectangleLinesEx(closeButtonRec, 5, RAYWHITE);
  DrawLine(closeButtonRec.x, closeButtonRec.y,
           closeButtonRec.x + closeButtonRec.width,
           closeButtonRec.y + closeButtonRec.height, RAYWHITE);
  DrawLine(closeButtonRec.x, closeButtonRec.y + closeButtonRec.height,
           closeButtonRec.x + closeButtonRec.width, closeButtonRec.y, RAYWHITE);

  // leaderboard button
  DrawTexture(leaderboard, leaderboardRec.x, leaderboardRec.y, YELLOW);
}

void UnloadTitleScreen() {
}

/*
  0 - not finish
  1 - go to play screen
  2 - go to leaderboard screen
 */
int FinishTitleScreen() { return finishScreen; }

void UpdateTitleGridColors() {
  for (int i = 0; i < TITLE_GRID_ROWS; i++) {
    for (int j = 0; j < TITLE_GRID_COLS; j++) {
      titleColors[i][j] = TITLE_COLORS[rand() % TITLE_COLOR_NUM];
    }
  }
}
