#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "raylib.h"
#include "screens/screens.h"

int main(void) {
  srand(time(NULL));
  stage = 1;
  quit = false;
  prize = false;
  score = 0;
  highestScore = 0;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fragmentation Game");
  InitAudioDevice();
  currentScreen = TITLE;
  InitTitleScreen();
  //ToggleFullscreen();
  HideCursor();
  SetTargetFPS(60);
  while (!quit && !WindowShouldClose()) {
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
    DrawLineEx(cursorPos, (Vector2){cursorPos.x + 40, cursorPos.y + 40}, 8,
               BLACK);
    DrawLineEx(cursorPos, (Vector2){cursorPos.x + 30, cursorPos.y}, 8, BLACK);
    DrawLineEx(cursorPos, (Vector2){cursorPos.x, cursorPos.y + 30}, 8, BLACK);
    EndDrawing();
  }

  CloseAudioDevice();
  CloseWindow();
  return 0;
}
