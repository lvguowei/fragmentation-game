#include "raylib.h"
#include "screens.h"
#include "leaderboard.h"
#include <stdio.h>
#include <string.h>

#define TEXTBOX_WIDTH 700
#define TEXTBOX_HEIGHT 80
#define TEXTBOX_FONT_SIZE 50
#define TEXTBOX_LABEL_SIZE 40
#define MAX_NAME_CHARS 50
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

static char name[MAX_NAME_CHARS + 1] = "\0";
static char email[MAX_EMAIL_CHARS + 1] = "\0";
static char description[MAX_DESCRIPTION_CHARS + 1] = "\0";

void InitEndingScreen() {
  framesCount = 0;
  finishScreen = false;

  nameLetterCount = 0;
  emailLetterCount = 0;
  descriptionLetterCount = 0;

  playAgainRec =
      (Rectangle){SCREEN_WIDTH / 2 - 300,
                  SCREEN_HEIGHT / 2 - 200 - 160 + 200 + 200 + 300, 600, 150};
  nameBoxRec = (Rectangle){(SCREEN_WIDTH - TEXTBOX_WIDTH) / 2, 500,
                           TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  emailBoxRec = (Rectangle){(SCREEN_WIDTH - TEXTBOX_WIDTH) / 2, 500 + 100,
                            TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  descriptionBoxRec =
      (Rectangle){(SCREEN_WIDTH - TEXTBOX_WIDTH) / 2, 500 + 100 + 100,
                  TEXTBOX_WIDTH, TEXTBOX_HEIGHT};
  endingMusic = LoadMusicStream("resources/music/ending.mp3");
  startSound = LoadSound("resources/sounds/start.wav");
  PlayMusicStream(endingMusic);
}

void UpdateEndingScreen() {
  framesCount++;
  UpdateMusicStream(endingMusic);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (CheckCollisionPointRec(GetMousePosition(), nameBoxRec)) {
      mouseOnNameBox = true;
      mouseOnEmailBox = false;
      mouseOnDescriptionBox = false;
    }
    else if (CheckCollisionPointRec(GetMousePosition(), emailBoxRec)) {
      mouseOnNameBox = false;
      mouseOnEmailBox = true;
      mouseOnDescriptionBox = false;
    }
    else if (CheckCollisionPointRec(GetMousePosition(), descriptionBoxRec)) {
      mouseOnNameBox = false;
      mouseOnEmailBox = false;
      mouseOnDescriptionBox = true;
    } else {
      mouseOnNameBox = false;
      mouseOnEmailBox = false;
      mouseOnDescriptionBox = false;
    }
  }

  if (mouseOnNameBox) {
    int key = GetKeyPressed();
    if ((key >= 32) && (key <= 125) && (nameLetterCount < MAX_NAME_CHARS)) {
      name[nameLetterCount] = (char)key;
      name[nameLetterCount + 1] = '\0';
      nameLetterCount++;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      nameLetterCount--;
      name[nameLetterCount] = '\0';

      if (nameLetterCount < 0)
        nameLetterCount = 0;
    }
  }

  if (mouseOnEmailBox) {
    int key = GetKeyPressed();
    if ((key >= 32) && (key <= 125) && (emailLetterCount < MAX_EMAIL_CHARS)) {
      email[emailLetterCount] = (char)key;
      email[emailLetterCount + 1] = '\0';
      emailLetterCount++;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      emailLetterCount--;
      email[emailLetterCount] = '\0';

      if (emailLetterCount < 0)
        emailLetterCount = 0;
    }
  }

  if (mouseOnDescriptionBox) {
    int key = GetKeyPressed();
    if ((key >= 32) && (key <= 125) &&
        (descriptionLetterCount < MAX_DESCRIPTION_CHARS)) {
      description[descriptionLetterCount] = (char)key;
      description[descriptionLetterCount + 1] = '\0';
      descriptionLetterCount++;
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
      descriptionLetterCount--;
      description[descriptionLetterCount] = '\0';

      if (descriptionLetterCount < 0)
        descriptionLetterCount = 0;
    }
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, playAgainRec)) {
      finishScreen = true;
      PlaySound(startSound);
#if defined(PLATFORM_WEB)
#else
      if (nameLetterCount != 0) {
        save_entry(nameLetterCount == 0 ? "Anonymous" : name,
                   emailLetterCount == 0 ? " " : email,
                   descriptionLetterCount == 0 ? " " : description,
                   score);
      }
#endif
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

#if defined(PLATFORM_WEB)
  // Do not draw cursor in web
#else
  if (stage == STAGE_NUM) {
    // draw name box
    DrawText("NAME",
             nameBoxRec.x - MeasureText("NAME", TEXTBOX_LABEL_SIZE) - 30,
             nameBoxRec.y + 20, TEXTBOX_LABEL_SIZE, GRAY);
    DrawRectangleRec(nameBoxRec, LIGHTGRAY);
    if (mouseOnNameBox) {
      DrawRectangleLines(nameBoxRec.x, nameBoxRec.y, nameBoxRec.width,
                         nameBoxRec.height, RED);
    }
    DrawText(name, nameBoxRec.x + 10, nameBoxRec.y + 20, TEXTBOX_FONT_SIZE,
             RED);

    if (mouseOnNameBox) {
      if (nameLetterCount < MAX_NAME_CHARS) {
        if ((framesCount / 20) % 2 == 0) {
          DrawText("_",
                   nameBoxRec.x + 20 + MeasureText(name, TEXTBOX_FONT_SIZE),
                   nameBoxRec.y + 20, TEXTBOX_FONT_SIZE, RED);
        }
      }
    }

    // draw email box
    DrawText("EMAIL",
             emailBoxRec.x - MeasureText("EMAIL", TEXTBOX_LABEL_SIZE) - 30,
             emailBoxRec.y + 20, TEXTBOX_LABEL_SIZE, GRAY);
    DrawRectangleRec(emailBoxRec, LIGHTGRAY);
    if (mouseOnEmailBox) {
      DrawRectangleLines(emailBoxRec.x, emailBoxRec.y, emailBoxRec.width,
                         emailBoxRec.height, RED);
    }
    DrawText(email, emailBoxRec.x + 10, emailBoxRec.y + 20, TEXTBOX_FONT_SIZE,
             RED);

    if (mouseOnEmailBox) {
      if (emailLetterCount < MAX_EMAIL_CHARS) {
        if ((framesCount / 20) % 2 == 0) {
          DrawText("_",
                   emailBoxRec.x + 20 + MeasureText(email, TEXTBOX_FONT_SIZE),
                   emailBoxRec.y + 20, TEXTBOX_FONT_SIZE, RED);
        }
      }
    }
  }
#endif
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
    DrawText("DONE",
             playAgainRec.x +
                 (playAgainRec.width - MeasureText("DONE", 60)) / 2,
             playAgainRec.y + (playAgainRec.height - 60) / 2, 60, BLUE);
  }
}

void UnloadEndingScreen() {
  name[0] = '\0';
  nameLetterCount = 0;
  email[0] = '\0';
  emailLetterCount = 0;
  description[0] = '\0';
  descriptionLetterCount = 0;
  StopMusicStream(endingMusic);
  UnloadMusicStream(endingMusic);
  UnloadSound(startSound);
}

bool FinishEndingScreen() { return finishScreen; }
