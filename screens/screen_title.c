#include "raylib.h"
#include "screens.h"
#include <stdlib.h>

#define TITLE_GRID_ROWS 20
#define TITLE_GRID_COLS 30
#define TITLE_COLOR_NUM 6

const Color TITLE_COLORS[TITLE_COLOR_NUM] = {
    (Color){147, 58, 22, 180}, (Color){141, 2, 31, 180},
    (Color){94, 25, 20, 180},  (Color){184, 15, 10, 180},
    (Color){66, 13, 9, 180},   (Color){255, 40, 0, 180}};

Color titleColors[TITLE_GRID_ROWS][TITLE_GRID_COLS] = {0};
Rectangle closeButtonRec;
Music titleMusic;

static bool finishScreen;
static int framesCount;
Rectangle closeButtonRec;

void UpdateTitleGridColors();

void InitTitleScreen() {
  framesCount = 0;
  closeButtonRec = (Rectangle){SCREEN_WIDTH - 80, 0, 80, 80};
  finishScreen = false;
  titleMusic = LoadMusicStream("resources/title.mp3");
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
    } else {
      finishScreen = true;
    }
  }
}

void DrawTitleScreen() {
  // Draw grid anim
  int w = SCREEN_WIDTH / TITLE_GRID_COLS;
  int h = SCREEN_HEIGHT / TITLE_GRID_ROWS;
  for (int i = 0; i < TITLE_GRID_ROWS; i++) {
    for (int j = 0; j < TITLE_GRID_COLS; j++) {
      DrawRectangle(j * w, i * h, w, h, titleColors[i][j]);
    }
  }

  int textX = (SCREEN_WIDTH - MeasureText("FRAGMENTATION GAME", 120)) / 2;
  int textY = (SCREEN_HEIGHT - 120) / 2;

  DrawRectangle(0, textY - 50, SCREEN_WIDTH, 120 + 50 * 2, RED);

  // Game Title
  DrawText("FRAGMENTATION GAME", textX, textY, 120, RAYWHITE);

  // Touch to start
  if ((framesCount / 40) % 2 == 0) {
    DrawText("Touch Screen to Start",
             SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 80) / 2,
             SCREEN_HEIGHT / 2 - 80 + 300, 80, BLANK);
  } else {
    DrawText("Touch Screen to Start",
             SCREEN_WIDTH / 2 - MeasureText("Touch Screen to Start", 80) / 2,
             SCREEN_HEIGHT / 2 - 80 + 300, 80, RAYWHITE);
  }

  // Close button
  DrawRectangleLinesEx(closeButtonRec, 5, RAYWHITE);
  DrawLine(closeButtonRec.x, closeButtonRec.y,
           closeButtonRec.x + closeButtonRec.width,
           closeButtonRec.y + closeButtonRec.height, RAYWHITE);
  DrawLine(closeButtonRec.x, closeButtonRec.y + closeButtonRec.height,
           closeButtonRec.x + closeButtonRec.width, closeButtonRec.y, RAYWHITE);
}

void UnloadTitleScreen() {
  UnloadMusicStream(titleMusic);
}

bool FinishTitleScreen() { return finishScreen; }

void UpdateTitleGridColors() {
  for (int i = 0; i < TITLE_GRID_ROWS; i++) {
    for (int j = 0; j < TITLE_GRID_COLS; j++) {
      titleColors[i][j] = TITLE_COLORS[rand() % TITLE_COLOR_NUM];
    }
  }
}