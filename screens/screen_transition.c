#include "raylib.h"
#include "screens.h"
#include <string.h>

#define FONT_SIZE 120

static int finishScreen;
static int framesCount;
static char STAGE_1_MESSAGE[] = "New phone.\nReading files \nis a piece of cake!";
static char STAGE_2_MESSAGE[] = "Used phone.\nFiles start to \nget fragmented...\n";
static char STAGE_3_MESSAGE[] = "Old phone.\nFiles are so \nfragmented!";

static int animSpeed;

void InitTransitionScreen() {
  finishScreen = false;
  framesCount = 0;
  switch (stage) {
  case 0: {
    animSpeed = 5;
    break;
  }
  case 1: {
    animSpeed = 8;
    break;
  }
  case 2: {
    animSpeed = 10;
    break;
  }
default:
  animSpeed = 3;
    break;
  }
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

  if (framesCount % animSpeed == 0 &&
      framesCount / animSpeed <= (int)strlen(msg)) {
    PlaySound(textSound);
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    switch (stage) {
    case 0: {
      if (framesCount / animSpeed > (int)strlen(STAGE_1_MESSAGE)) {
        PlaySound(startSound);
        finishScreen = true;
      }
      break;
    }
    case 1: {
      if (framesCount / animSpeed > (int)strlen(STAGE_2_MESSAGE)) {
        PlaySound(startSound);
        finishScreen = true;
      }
      break;
    }
    case 2: {
      if (framesCount / animSpeed > (int)strlen(STAGE_3_MESSAGE)) {
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
  Rectangle rec = {200, 300, 1200, 1000};
  DrawTextRec(GetFontDefault(), TextSubtext(msg, 0, framesCount / animSpeed), rec, FONT_SIZE, 6, true, DARKGRAY);

  DrawTexture(pic,
              SCREEN_WIDTH / 2 + MeasureText(msg, FONT_SIZE) / 2 - 400 + 300,
              SCREEN_HEIGHT / 2 - 300, WHITE);
}

void UnloadTransitionScreen() { StopMusicStream(transitionMusic); }

bool FinishTransitionScreen() { return finishScreen; }
