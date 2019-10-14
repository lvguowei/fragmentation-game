#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <string.h>

/* 0 - Not finish
   1 - Go to title screen
   2 - Go to play screen
 */
static int finishScreen;
static int framesCount;
static Sound endingSound;
static Sound stageClearSound;

static Rectangle playAgainRec;

void InitEndingScreen() {
  framesCount = 0;
  finishScreen = false;
  playAgainRec =
      (Rectangle){SCREEN_WIDTH / 2 - 300,
                  SCREEN_HEIGHT / 2 - 200 - 160 + 200 + 200 + 300, 600, 150};
  endingSound = LoadSound("resources/sounds/ending.mp3");
  stageClearSound = LoadSound("resources/sounds/clear.mp3");

  if (stage == STAGE_NUM) {
    PlaySound(endingSound);
  } else {
    PlaySound(stageClearSound);
  }
}

void UpdateEndingScreen() {
  framesCount++;
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, playAgainRec)) {
      finishScreen = true;
    }
  }
}

void DrawEndingScreen() {
  if (stage < STAGE_NUM) {
    DrawText("WELL DONE!", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 160) / 2,
             SCREEN_HEIGHT / 2 - 200 - 160, 160, GRAY);
  } else {
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 160) / 2,
             SCREEN_HEIGHT / 2 - 200 - 160, 160, GRAY);
  }

  char scoreLine[32] = "SCORE: ";
  char sscore[8];
  sprintf(sscore, "%d", score);
  strcat(scoreLine, sscore);

  DrawText(scoreLine, SCREEN_WIDTH / 2 - MeasureText(scoreLine, 100) / 2,
           SCREEN_HEIGHT / 2 - 200 - 160 + 200, 100, GRAY);

  if (prize && stage == STAGE_NUM) {
    if ((framesCount / 40) % 2 == 0) {
      DrawText("NEW RECORD",
               SCREEN_WIDTH / 2 - MeasureText("NEW RECORD", 100) / 2,
               SCREEN_HEIGHT / 2 - 200 - 160 + 200 + 200, 100, RED);
    }
  }
  DrawRectangleRec(playAgainRec, SKYBLUE);
  if (stage < STAGE_NUM) {
    DrawText("NEXT STAGE",
             playAgainRec.x +
                 (playAgainRec.width - MeasureText("NEXT STAGE", 60)) / 2,
             playAgainRec.y + (playAgainRec.height - 60) / 2, 60, BLUE);
  } else {
    DrawText("PLAY AGAIN",
             playAgainRec.x +
                 (playAgainRec.width - MeasureText("PLAY AGAIN", 60)) / 2,
             playAgainRec.y + (playAgainRec.height - 60) / 2, 60, BLUE);
  }
}

void UnloadEndingScreen() {
  StopSound(endingSound);
  StopSound(stageClearSound);
  UnloadSound(stageClearSound);
  UnloadSound(endingSound);
}

bool FinishEndingScreen() { return finishScreen; }
