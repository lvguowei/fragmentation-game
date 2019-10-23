#include "leaderboard.h"
#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIMIT 10

static bool finishScreen;
static int framesCount;
static Entry *leaderboard_entries;
static int leaderboard_size;
static Rectangle closeButtonRec;

void InitLeaderBoardScreen(void) {
  framesCount = 0;
  finishScreen = false;
  leaderboard_size = 0;
  leaderboard_entries = read_leaderboard(&leaderboard_size);
  closeButtonRec = (Rectangle){SCREEN_WIDTH - 80, 0, 80, 80};
}
void UpdateLeaderBoardScreen(void) {
  framesCount++;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, closeButtonRec)) {
      finishScreen = true;
    }
  }
}
void DrawLeaderBoardScreen(void) {
  ClearBackground(BLACK);
  int y = 100;
  DrawRectangle((SCREEN_WIDTH - 1000) / 2 - 40, y, 1000, (50 + 10) * 11 + 40,
                BLACK);
  int n = leaderboard_size > LIMIT ? LIMIT : leaderboard_size;
  DrawText("RANK", (SCREEN_WIDTH - 1000) / 2, y, 50, SKYBLUE);
  DrawText("NAME", (SCREEN_WIDTH - 1000) / 2 + 250, y, 50, SKYBLUE);
  DrawText("SCORE", (SCREEN_WIDTH - 1000) / 2 + 750, y, 50, SKYBLUE);
  y += 40;
  Color color;
  for (int i = 0; i < n; i++) {
    if (i == 0) {
      color = RED;
    } else if (i == 1) {
      color = ORANGE;
    } else if (i == 2) {
      color = YELLOW;
    } else {
      color = WHITE;
    }

    y += 50 + 10;
    DrawText(FormatText("#%2d", i + 1), (SCREEN_WIDTH - 1000) / 2, y, 50,
             color);
    DrawText(FormatText("%s", (leaderboard_entries + i)->name),
             (SCREEN_WIDTH - 1000) / 2 + 250, y, 50, color);
    DrawText(FormatText("%4d", (leaderboard_entries + i)->score),
             (SCREEN_WIDTH - 1000) / 2 + 750, y, 50, color);
  }

  // Close button
  DrawRectangleLinesEx(closeButtonRec, 5, RAYWHITE);
  DrawLine(closeButtonRec.x, closeButtonRec.y,
           closeButtonRec.x + closeButtonRec.width,
           closeButtonRec.y + closeButtonRec.height, RAYWHITE);
  DrawLine(closeButtonRec.x, closeButtonRec.y + closeButtonRec.height,
           closeButtonRec.x + closeButtonRec.width, closeButtonRec.y, RAYWHITE);
}
void UnloadLeaderBoardScreen(void) {}
bool FinishLeaderBoardScreen(void) { return finishScreen; }
