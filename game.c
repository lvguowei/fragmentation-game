#include "raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include "screens/screens.h"

static Texture2D cursor;

void UpdateAndDraw();

int main(void) {
  srand(time(NULL));
  stage = 1;
  quit = false;
  prize = false;
  score = 0;
  highestScore = 0;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fragmentation Game");

  cursor = LoadTexture("resources/images/cursor.png");
  InitAudioDevice();
  currentScreen = TITLE;
  InitTitleScreen();
  ToggleFullscreen();
  HideCursor();
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateAndDraw, 0, 1);
#else
  SetTargetFPS(60);
  while (!quit && !WindowShouldClose()) {
    UpdateAndDraw();
  }
#endif

  CloseAudioDevice();
  // UnloadTexture(cursor);
  CloseWindow();
  return 0;
}

void UpdateAndDraw() {
  switch (currentScreen) {
  case TITLE: {
    UpdateTitleScreen();
    if (FinishTitleScreen()) {
      UnloadTitleScreen();
      currentScreen = GAMEPLAY;
      InitGameplayScreen();
    }
    break;
  case GAMEPLAY: {
    UpdateGameplayScreen();
    if (FinishGameplayScreen()) {
      UnloadGameplayScreen();
      currentScreen = ENDING;
      InitEndingScreen();
      if (score > highestScore) {
        highestScore = score;
        prize = true;
      }
    }
    break;
  }
  case ENDING: {
    UpdateEndingScreen();
    switch (FinishEndingScreen()) {
    case 0: {
      // no finishing
      break;
    }
    case 1: {
      UnloadEndingScreen();
      currentScreen = TITLE;
      InitTitleScreen();
      score = 0;
      stage = 1;
      prize = false;
      break;
    }
    case 2: {
      stage++;
      UnloadEndingScreen();
      currentScreen = GAMEPLAY;
      InitGameplayScreen();
      break;
    }
    default:
      break;
    }
    break;
  }
  }
  default:
    break;
  }

  BeginDrawing();
  ClearBackground(RAYWHITE);
  switch (currentScreen) {
  case TITLE: {
    DrawTitleScreen();
    break;
  }
  case GAMEPLAY: {
    DrawGameplayScreen();
    break;
  }
  case ENDING: {
    DrawEndingScreen();
    break;
  }
  default:
    break;
  }

  // Draw cursor
  Vector2 cursorPos = GetMousePosition();
  DrawTexture(cursor, cursorPos.x, cursorPos.y, WHITE);
  EndDrawing();
}
