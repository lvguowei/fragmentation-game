#include "raylib.h"
#include "screens.h"

static int finishScreen;
static int framesCount;

void InitTransitionScreen() {
  finishScreen = false;
  framesCount = 0;
}

void UpdateTransitionScreen() {
  framesCount++;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    finishScreen = true;
  }
}

void DrawTransitionScreen() {
  switch (stage) {
  case 0: {
    DrawText("Going to stage 1",
             SCREEN_WIDTH / 2 - MeasureText("Going to stage x", 160) / 2,
             SCREEN_HEIGHT / 2 - 160, 160, GRAY);
    break;
  }
  case 1: {
    DrawText("Going to stage 2",
             SCREEN_WIDTH / 2 - MeasureText("Going to stage x", 160) / 2,
             SCREEN_HEIGHT / 2 - 160, 160, GRAY);
    break;
  }
  case 2: {
    DrawText("Going to stage 3",
             SCREEN_WIDTH / 2 - MeasureText("Going to stage x", 160) / 2,
             SCREEN_HEIGHT / 2 - 160, 160, GRAY);
    break;
  }
  default:
    break;
  }
}

void UnloadTransitionScreen() {}

bool FinishTransitionScreen() { return finishScreen; }
