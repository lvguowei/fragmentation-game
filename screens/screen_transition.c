#include "raylib.h"
#include "screens.h"
#include <string.h>

#define FONT_SIZE 120

static int finishScreen;
static int framesCount;
static char STAGE_1_MESSAGE[] = "New phone.\nVery fast!";
static char STAGE_2_MESSAGE[] = "Used phone.\nKinda OK...";
static char STAGE_3_MESSAGE[] = "Old phone.\nVery sluggish... ";
static int TEXT_ANIM_SPEED = 10;

void InitTransitionScreen() {
  finishScreen = false;
  framesCount = 0;
  PlayMusicStream(transitionMusic);
}

void UpdateTransitionScreen() {
  framesCount++;
  UpdateMusicStream(transitionMusic);

  char *msg = "";
  switch (stage) {
  case 0: {
    msg = STAGE_1_MESSAGE;
    break;
  }
  case 1: {
    msg = STAGE_2_MESSAGE;
    break;
  }
  case 2: {
    msg = STAGE_3_MESSAGE;
    break;
  }
  default:
    break;
  }

  if (framesCount % TEXT_ANIM_SPEED == 0 &&
      framesCount / TEXT_ANIM_SPEED <= (int)strlen(msg)) {
    PlaySound(textSound);
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    switch (stage) {
    case 0: {
      if (framesCount / TEXT_ANIM_SPEED > (int)strlen(STAGE_1_MESSAGE)) {
        PlaySound(startSound);
        finishScreen = true;
      }
      break;
    }
    case 1: {
      if (framesCount / TEXT_ANIM_SPEED > (int)strlen(STAGE_2_MESSAGE)) {
        PlaySound(startSound);
        finishScreen = true;
      }
      break;
    }
    case 2: {
      if (framesCount / TEXT_ANIM_SPEED > (int)strlen(STAGE_3_MESSAGE)) {
        PlaySound(startSound);
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
  char *msg;
  Texture2D pic;
  switch (stage) {
  case 0: {
    msg = STAGE_1_MESSAGE;
    pic = newPhone;
    break;
  }
  case 1: {
    msg = STAGE_2_MESSAGE;
    pic = usedPhone;
    break;
  }
  case 2: {
    msg = STAGE_3_MESSAGE;
    pic = oldPhone;
    break;
  }
  default:
    msg = "";
    pic = newPhone;
    break;
  }
  DrawText(TextSubtext(msg, 0, framesCount / TEXT_ANIM_SPEED),
           SCREEN_WIDTH / 2 - MeasureText(msg, FONT_SIZE) / 2 - 400,
           SCREEN_HEIGHT / 2 - FONT_SIZE, FONT_SIZE, GRAY);

  DrawTexture(pic,
              SCREEN_WIDTH / 2 + MeasureText(msg, FONT_SIZE) / 2 - 400 + 300,
              SCREEN_HEIGHT / 2 - 300, WHITE);
}

void UnloadTransitionScreen() { StopMusicStream(transitionMusic); }

bool FinishTransitionScreen() { return finishScreen; }
