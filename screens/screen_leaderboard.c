#include "raylib.h"
#include "screens.h"
#include "leaderboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool finishScreen;
static int framesCount;
static Entry *leaderboard_entries;
static int leaderboard_size;
void InitLeaderBoardScreen(void) {
  framesCount = 0;
  finishScreen = false;
  leaderboard_entries = read_leaderboard(&leaderboard_size);
  printf("Read %d entries\n", leaderboard_size);
  for (int i = 0; i < leaderboard_size; i++ ) {
    printf("%s\n", (leaderboard_entries + i)->name);
  }
}
void UpdateLeaderBoardScreen(void) { framesCount++; }
void DrawLeaderBoardScreen(void) { DrawCircle(100, 100, 100, RED); }
void UnloadLeaderBoardScreen(void) {}
bool FinishLeaderBoardScreen(void) { return false; }
