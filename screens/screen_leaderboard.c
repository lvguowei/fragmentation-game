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
void InitLeaderBoardScreen(void) {
  framesCount = 0;
  finishScreen = false;
  leaderboard_size = 0;
  leaderboard_entries = read_leaderboard(&leaderboard_size);
  printf("Read %d entries\n", leaderboard_size);
  for (int i = 0; i < leaderboard_size; i++) {
    printf("%s\n", (leaderboard_entries + i)->name);
  }
}
void UpdateLeaderBoardScreen(void) { framesCount++; }
void DrawLeaderBoardScreen(void) {
  int n = leaderboard_size > LIMIT ? LIMIT : leaderboard_size;
  int y = 100;
  for (int i = 0; i < n; i++) {
    y += 50;
    DrawText(FormatText("#%2d", i), (SCREEN_WIDTH - 1000) / 2, y, 50, GRAY);
    DrawText(FormatText("%s", (leaderboard_entries + i) -> name), (SCREEN_WIDTH - 1000) / 2 + 250, y, 50, GRAY);
    DrawText(FormatText("%4d", (leaderboard_entries + i) -> score), (SCREEN_WIDTH - 1000) / 2 + 750, y, 50, GRAY);
  }
}
void UnloadLeaderBoardScreen(void) {}
bool FinishLeaderBoardScreen(void) { return false; }
