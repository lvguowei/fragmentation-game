#include "raylib.h"
#include "screens.h"
#include <string.h>

#define FONT_SIZE 160

static int finishScreen;
static int framesCount;
static char STAGE_1_MESSAGE[] = "Brand new phone!";
static char STAGE_2_MESSAGE[] = "After 6 month ...";
static char STAGE_3_MESSAGE[] = "After 1 year ...";
static int TEXT_ANIM_SPEED = 10;

static Music transitionMusic;

void InitTransitionScreen() {
  finishScreen = false;
  framesCount = 0;
  transitionMusic = LoadMusicStream("resources/music/transition.mp3");
  PlayMusicStream(transitionMusic);
}

void UpdateTransitionScreen() {
  framesCount++;
  UpdateMusicStream(transitionMusic);
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    switch (stage) {
    case 0: {
      if (framesCount / TEXT_ANIM_SPEED > (int)strlen(STAGE_1_MESSAGE)) {
        finishScreen = true;
      }
      break;
    }
    case 1: {
      if (framesCount / TEXT_ANIM_SPEED > (int)strlen(STAGE_2_MESSAGE)) {
        finishScreen = true;
      }
      break;
    }
    case 2: {
      if (framesCount / TEXT_ANIM_SPEED > (int)strlen(STAGE_3_MESSAGE)) {
        finishScreen = true;
      }
      break;
    }
    default:
      break;
    }
  }
}

void DrawTransitionScreen() {
  switch (stage) {
  case 0: {
    DrawText(TextSubtext(STAGE_1_MESSAGE, 0, framesCount / TEXT_ANIM_SPEED),
             SCREEN_WIDTH / 2 - MeasureText(STAGE_1_MESSAGE, FONT_SIZE) / 2,
             SCREEN_HEIGHT / 2 - FONT_SIZE, FONT_SIZE, GRAY);
    break;
  }
  case 1: {
    DrawText(TextSubtext(STAGE_2_MESSAGE, 0, framesCount / TEXT_ANIM_SPEED),
             SCREEN_WIDTH / 2 - MeasureText(STAGE_2_MESSAGE, FONT_SIZE) / 2,
             SCREEN_HEIGHT / 2 - FONT_SIZE, FONT_SIZE, GRAY);
    break;
  }
  case 2: {
    DrawText(TextSubtext(STAGE_3_MESSAGE, 0, framesCount / TEXT_ANIM_SPEED),
             SCREEN_WIDTH / 2 - MeasureText(STAGE_3_MESSAGE, FONT_SIZE) / 2,
             SCREEN_HEIGHT / 2 - FONT_SIZE, FONT_SIZE, GRAY);
    break;
  }
  default:
    break;
  }
}

void UnloadTransitionScreen() {
  StopMusicStream(transitionMusic);
  UnloadMusicStream(transitionMusic);
}

bool FinishTransitionScreen() { return finishScreen; }
