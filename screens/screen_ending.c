#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <string.h>

#define TEXTBOX_WIDTH 800
#define TEXTBOX_HEIGHT 80
#define TEXTBOX_FONT_SIZE 60
#define MAX_NAME_CHARS 20
#define MAX_EMAIL_CHARS 50
#define MAX_DESCRIPTION_CHARS 100

static bool finishScreen;
static int framesCount;
static Music endingMusic;
static Sound startSound;

static Rectangle playAgainRec;
static Rectangle nameBoxRec;
static Rectangle emailBoxRec;
static Rectangle descriptionBoxRec;

static bool mouseOnNameBox;
static bool mouseOnEmailBox;
static bool mouseOnDescriptionBox;

static int nameLetterCount;
static int emailLetterCount;
static int descriptionLetterCount;

void InitEndingScreen() {
  framesCount = 0;
  finishScreen = false;
  playAgainRec =
      (Rectangle){SCREEN_WIDTH / 2 - 300,
                  SCREEN_HEIGHT / 2 - 200 - 160 + 200 + 200 + 300, 600, 150};
  nameBoxRec = (Rectangle) {(SCREEN_WIDTH - TEXTBOX_WIDTH) / 2, 500, TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  emailBoxRec = (Rectangle) {(SCREEN_WIDTH - TEXTBOX_WIDTH) / 2, 500 + 100, TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  descriptionBoxRec = (Rectangle) {(SCREEN_WIDTH - TEXTBOX_WIDTH) / 2, 500 + 100 + 100, TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  endingMusic = LoadMusicStream("resources/music/ending.mp3");
  startSound = LoadSound("resources/sounds/start.wav");
  PlayMusicStream(endingMusic);
}

void UpdateEndingScreen() {
  framesCount++;
  UpdateMusicStream(endingMusic);

  mouseOnNameBox = false;
  mouseOnEmailBox = false;
  mouseOnDescriptionBox = false;

  if(CheckCollisionPointRec(GetMousePosition(), nameBoxRec)) {
    mouseOnNameBox = true;
  }

  if(CheckCollisionPointRec(GetMousePosition(), emailBoxRec)) {
    mouseOnEmailBox = true;
  }

  if(CheckCollisionPointRec(GetMousePosition(), descriptionBoxRec)) {
    mouseOnDescriptionBox = true;
  }

  if (mouseOnNameBox) {
    int key = GetKeyPressed();
    if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS))
      {
        name[letterCount] = (char)key;
        letterCount++;
      }

    if (IsKeyPressed(KEY_BACKSPACE))
      {
        letterCount--;
        name[letterCount] = '\0';

        if (letterCount < 0) letterCount = 0;
      }
  }



  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, playAgainRec)) {
      finishScreen = true;
      PlaySound(startSound);
    }
  }
}

void DrawEndingScreen() {
  // draw title
  if (stage < STAGE_NUM) {
    DrawText("WELL DONE!", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 120) / 2,
             SCREEN_HEIGHT / 2 - 300 - 120, 120, GRAY);
  } else {
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 120) / 2,
             SCREEN_HEIGHT / 2 - 300 - 120, 120, GRAY);
  }

  // draw score
  char scoreLine[32] = "SCORE: ";
  char sscore[8];
  sprintf(sscore, "%d", score);
  strcat(scoreLine, sscore);

  DrawText(scoreLine, SCREEN_WIDTH / 2 - MeasureText(scoreLine, 100) / 2,
           SCREEN_HEIGHT / 2 - 300 - 120 + 200, 100, GRAY);

  if (true) {
    // draw name box
    DrawRectangleRec(nameBoxRec, LIGHTGRAY);
    if (true) {
      DrawRectangleLines(nameBoxRec.x, nameBoxRec.y, nameBoxRec.width, nameBoxRec.height, RED);
    }
    DrawText("lvguowei", nameBoxRec.x + 5, nameBoxRec.y + 5, TEXTBOX_FONT_SIZE, RED);
  }
  /* if (prize && stage == STAGE_NUM) { */
  /*   if ((framesCount / 40) % 2 == 0) { */
  /*     DrawText("NEW RECORD", */
  /*              SCREEN_WIDTH / 2 - MeasureText("NEW RECORD", 100) / 2, */
  /*              SCREEN_HEIGHT / 2 - 300 - 120 + 200 + 200, 100, RED); */
  /*   } */
  /* } */
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
  StopMusicStream(endingMusic);
  UnloadMusicStream(endingMusic);
  UnloadSound(startSound);
}

bool FinishEndingScreen() { return finishScreen; }
