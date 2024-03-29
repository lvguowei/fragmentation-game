#ifndef SCREENS_H
#define SCREENS_H

#include <stdbool.h>
#include "raylib.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define STAGE_NUM 3

typedef enum GameScreen {
  TITLE,
  GAMEPLAY,
  ENDING,
  TRANSITION,
  LEADERBOARD
} GameScreen;

// Global variables
GameScreen currentScreen;
int stage;
bool quit;
bool prize;
int score;
int highestScore;

// Resources

Sound startSound;

// Title screen
Music titleMusic;
Texture2D leaderboard;
// http://arcade.photonstorm.com/ king of fighers 2003
Texture2D gameTitle;

// Ending screen
Music endingMusic;

// Gameplay screen
Sound clickSound;
Sound beepSound;
Sound beepHighSound;
Music stage1Music;
Music stage2Music;
Music stage3Music;

// Transition screen
Music transitionMusic;
Sound textSound;
Texture2D newPhone;
Texture2D usedPhone;
Texture2D oldPhone;
Texture2D background;


#ifdef __cplusplus
extern "C" { // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Title Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTitleScreen(void);
void UpdateTitleScreen(void);
void DrawTitleScreen(void);
void UnloadTitleScreen(void);
int FinishTitleScreen(void);

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
bool FinishGameplayScreen(void);

//----------------------------------------------------------------------------------
// Ending Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitEndingScreen(void);
void UpdateEndingScreen(void);
void DrawEndingScreen(void);
void UnloadEndingScreen(void);
bool FinishEndingScreen(void);

//----------------------------------------------------------------------------------
// Transition Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitTransitionScreen(void);
void UpdateTransitionScreen(void);
void DrawTransitionScreen(void);
void UnloadTransitionScreen(void);
bool FinishTransitionScreen(void);

//----------------------------------------------------------------------------------
// Leaderboard Screen Functions Declaration
//----------------------------------------------------------------------------------
void InitLeaderBoardScreen(void);
void UpdateLeaderBoardScreen(void);
void DrawLeaderBoardScreen(void);
void UnloadLeaderBoardScreen(void);
bool FinishLeaderBoardScreen(void);

#ifdef __cplusplus
}
#endif

#endif // SCREENS_H
