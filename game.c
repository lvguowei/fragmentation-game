#include "raylib.h"
#include "screens/backgrounds.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include "screens/screens.h"

static bool shouldDrawCursor;
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
  shouldDrawCursor = true;
  quit = false;
  prize = false;
  score = 0;
  highestScore = 0;

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Fragmentation Game");
  InitAudioDevice();
  cursor = LoadTexture("resources/images/cursor.png");

  // title screen
  titleMusic = LoadMusicStream("resources/music/title.mp3");
  startSound = LoadSound("resources/sounds/start.wav");
  leaderboard = LoadTexture("resources/images/leaderboard.png");

  // ending screen
  endingMusic = LoadMusicStream("resources/music/ending.mp3");


  // gameplay screen
  clickSound = LoadSound("resources/sounds/click.mp3");
  beepSound = LoadSound("resources/sounds/beep.mp3");
  beepHighSound = LoadSound("resources/sounds/beep_high.mp3");
  stage1Music = LoadMusicStream("resources/music/stage1_music.mp3");
  stage2Music = LoadMusicStream("resources/music/stage2_music.mp3");
  stage3Music = LoadMusicStream("resources/music/stage3_music.mp3");
  InitStage1Background();
  InitStage2Background();
  InitStage3Background();

  // transition screen
  transitionMusic = LoadMusicStream("resources/music/transition.mp3");

  currentScreen = TITLE;
  InitTitleScreen();
  //ToggleFullscreen();
  HideCursor();
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateAndDraw, 0, 1);
#else
  SetTargetFPS(60);
  while (!quit && !WindowShouldClose()) {
    UpdateAndDraw();
  }
#endif
  UnloadStage1Background();
  UnloadStage2Background();
  UnloadStage3Background();
  
  UnloadMusicStream(titleMusic);
  UnloadSound(startSound);
  UnloadMusicStream(endingMusic);
  UnloadSound(clickSound);
  UnloadSound(beepSound);
  UnloadSound(beepHighSound);
  UnloadMusicStream(stage1Music);
  UnloadMusicStream(stage2Music);
  UnloadMusicStream(stage3Music);
  UnloadMusicStream(transitionMusic);
  CloseAudioDevice();
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
      case TRANSITION:
        UnloadTransitionScreen();
        break;
      case LEADERBOARD: {
        UnloadLeaderBoardScreen();
        break;
      }
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
      case TRANSITION:
        InitTransitionScreen();
        break;
      case LEADERBOARD: {
        InitLeaderBoardScreen();
        break;
      }
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
  } else {
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
      int finish_value = FinishTitleScreen();
      if (finish_value == 1) {
        TransitionToScreen(TRANSITION);
      } else if (finish_value == 2) {
#if defined(PLATFORM_WEB)
#else
        TransitionToScreen(LEADERBOARD);
#endif
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
      if (FinishEndingScreen()) {
        if (stage == STAGE_NUM) {
          TransitionToScreen(TITLE);
        } else {
          TransitionToScreen(TRANSITION);
        }
      }
      break;
    }
    case TRANSITION: {
      UpdateTransitionScreen();
      if (FinishTransitionScreen()) {
        if (stage == STAGE_NUM) {
          TransitionToScreen(TITLE);
        } else {
          TransitionToScreen(GAMEPLAY);
        }
      }
      break;
    }
      case LEADERBOARD: {
        UpdateLeaderBoardScreen();
        if (FinishLeaderBoardScreen()) {
          TransitionToScreen(TITLE);
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

  if (IsKeyPressed(KEY_M)) {
    if (stage != STAGE_NUM) {
      shouldDrawCursor = !shouldDrawCursor;
    }
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
  case TRANSITION: {
    DrawTransitionScreen();
    break;
  }
  case LEADERBOARD: {
    DrawLeaderBoardScreen();
    break;
  }
  default:
    break;
  }

  // Draw full screen rectangle
  if (onTransition)
    DrawTransition();

#if defined(PLATFORM_WEB)
    // Do not draw cursor in web
#else
  // Draw cursor
  if (shouldDrawCursor) {
    Vector2 cursorPos = GetMousePosition();
    DrawTexture(cursor, cursorPos.x, cursorPos.y, WHITE);
  }
#endif

  EndDrawing();
}
