#include "raylib.h"
#include "screens.h"
#include <string.h>

#define FONT_SIZE 120
#define ANIM_SPEED 10

static int finishScreen;
static int framesCount;
static char STAGE_1_MESSAGE[] =
    "New phone.\nReading files\nis a piece of cake!\n";
static char STAGE_2_MESSAGE[] =
    "Used phone.\nFiles start to\nget fragmented...\n";
static char STAGE_3_MESSAGE[] = "Old phone.\nFiles are so\nfragmented!";

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

  if (framesCount % ANIM_SPEED == 0 &&
      framesCount / ANIM_SPEED <= (int)strlen(msg)) {
    PlaySound(textSound);
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    switch (stage) {
    case 0: {
      if (framesCount / ANIM_SPEED > (int)strlen(STAGE_1_MESSAGE)) {
        PlaySound(startSound);
        finishScreen = true;
      }
      break;
    }
    case 1: {
      if (framesCount / ANIM_SPEED > (int)strlen(STAGE_2_MESSAGE)) {
        PlaySound(startSound);
        finishScreen = true;
      }
      break;
    }
    case 2: {
      if (framesCount / ANIM_SPEED > (int)strlen(STAGE_3_MESSAGE)) {
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
  ClearBackground(RAYWHITE);

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

  DrawTexture(background, 0, 0, Fade(WHITE, 0.5));

  Rectangle rec = {200, 300, 1200, 1000};

  DrawRectangle(100, 200, 1200, 800, BLACK);
  DrawRectangleLinesEx((Rectangle){100, 200, 1200, 800}, 10, PINK);

  const char *t = TextSubtext(msg, 0, framesCount / ANIM_SPEED);
  if ((framesCount / 20) % 2 == 0) {
    t = TextFormat("%s%s", t, "_");
  }
  DrawTextRec(GetFontDefault(), t, rec, FONT_SIZE, 6, true, GREEN);

  DrawTexture(pic, 1400, 300, RAYWHITE);
}

void UnloadTransitionScreen() { StopMusicStream(transitionMusic); }

bool FinishTransitionScreen() { return finishScreen; }
