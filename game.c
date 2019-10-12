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

// Required variables to manage screen transitions (fade-in, fade-out)
static float transAlpha = 0.0f;
static bool onTransition = false;
static int transFromScreen = -1;
static int transToScreen = -1;
static void TransitionToScreen(int screen);
static void UpdateTransition(void);
static void DrawTransition(void);

static void UpdateAndDraw();

int main(void) {
  srand(time(NULL));
  stage = 0;
  quit = false;
  prize = false;
  score = 0;
  highestScore = 0;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fragmentation Game");

  cursor = LoadTexture("resources/images/cursor.png");
  InitAudioDevice();
  currentScreen = TITLE;
  InitTitleScreen();
  // ToggleFullscreen();
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

static void TransitionToScreen(int screen) {
  onTransition = true;
  transFromScreen = currentScreen;
  transToScreen = screen;
  transAlpha = 0.0f;
}

static void UpdateTransition(void) {
  if (onTransition) {
    transAlpha += 0.02f;

    // NOTE: Due to float internal representation, condition jumps on 1.0f
    // instead of 1.05f For that reason we compare against 1.01f, to avoid last
    // frame loading stop
    if (transAlpha > 1.01f) {
      transAlpha = 1.0f;

      // Unload current screen
      switch (transFromScreen) {
      case TITLE:
        UnloadTitleScreen();
        break;
      case GAMEPLAY:
        UnloadGameplayScreen();
        break;
      case ENDING:
        UnloadEndingScreen();
        break;
      default:
        break;
      }

      // Load next screen
      switch (transToScreen) {
      case TITLE:
        score = 0;
        stage = 0;
        prize = false;
        InitTitleScreen();
        break;
      case GAMEPLAY:
        stage++;
        InitGameplayScreen();
        break;
      case ENDING:
        InitEndingScreen();
        break;
      default:
        break;
      }

      currentScreen = transToScreen;
      onTransition = false;
    } else {
      switch (transFromScreen) {
      case TITLE:
        UpdateTitleScreen();
        break;
      default:
        break;
      }
    }
  } else
  {
    transAlpha = 0.0f;
    transFromScreen = -1;
    transToScreen = -1;
  }
}

static void DrawTransition(void) {
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                Fade(BLACK, transAlpha));
}

void UpdateAndDraw() {
  if (!onTransition) {
    switch (currentScreen) {
    case TITLE: {
      UpdateTitleScreen();
      if (FinishTitleScreen()) {
        TransitionToScreen(GAMEPLAY);
      }
      break;
    case GAMEPLAY: {
      UpdateGameplayScreen();
      if (FinishGameplayScreen()) {
        if (score > highestScore) {
          highestScore = score;
          prize = true;
        }
        TransitionToScreen(ENDING);
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
        TransitionToScreen(TITLE);
        break;
      }
      case 2: {
        TransitionToScreen(GAMEPLAY);
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
  } else {
    UpdateTransition();
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

  // Draw full screen rectangle
  if (onTransition)
    DrawTransition();

  // Draw cursor
  Vector2 cursorPos = GetMousePosition();
  DrawTexture(cursor, cursorPos.x, cursorPos.y, WHITE);
  EndDrawing();
}
