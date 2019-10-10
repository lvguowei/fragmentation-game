#include "backgrounds.h"
#include "raylib.h"

static Texture2D bgTextureLayer1;
static Texture2D bgTextureLayer2;
static Texture2D bgTextureLayer3;
static Texture2D bgTextureLayer4;
static Texture2D bgTextureLayer5;
static Texture2D bgTextureLayer6;
static Texture2D bgTextureLayer7;

static float scrollingLayer1 = 0.0f;
static float scrollingLayer2 = 0.0f;
static float scrollingLayer3 = 0.0f;
static float scrollingLayer4 = 0.0f;
static float scrollingLayer5 = 0.0f;
static float scrollingLayer6 = 0.0f;
static float scrollingLayer7 = 0.0f;

void InitStage3Background() {
  bgTextureLayer1 = LoadTexture("resources/images/backgrounds/stage3/layer1.png");
  bgTextureLayer2 = LoadTexture("resources/images/backgrounds/stage3/layer2.png");
  bgTextureLayer3 = LoadTexture("resources/images/backgrounds/stage3/layer3.png");
  bgTextureLayer4 = LoadTexture("resources/images/backgrounds/stage3/layer4.png");
  bgTextureLayer5 = LoadTexture("resources/images/backgrounds/stage3/layer5.png");
  bgTextureLayer6 = LoadTexture("resources/images/backgrounds/stage3/layer6.png");
  bgTextureLayer7 = LoadTexture("resources/images/backgrounds/stage3/layer7.png");
}

void UpdateStage3Background() {
  scrollingLayer1 -= 0.5f;
  scrollingLayer2 -= 1.0f;
  scrollingLayer3 -= 1.5f;
  scrollingLayer4 -= 2.0f;
  scrollingLayer5 -= 2.5f;
  scrollingLayer6 -= 3.0f;
  scrollingLayer7 -= 3.5f;

  if (scrollingLayer1 <= -bgTextureLayer1.width)
    scrollingLayer1 = 0;
  if (scrollingLayer2 <= -bgTextureLayer2.width)
    scrollingLayer2 = 0;
  if (scrollingLayer3 <= -bgTextureLayer3.width)
    scrollingLayer3 = 0;
  if (scrollingLayer4 <= -bgTextureLayer4.width)
    scrollingLayer4 = 0;
  if (scrollingLayer5 <= -bgTextureLayer5.width)
    scrollingLayer5 = 0;
  if (scrollingLayer6 <= -bgTextureLayer6.width)
    scrollingLayer6 = 0;
  if (scrollingLayer7 <= -bgTextureLayer7.width)
    scrollingLayer7 = 0;
}

void DrawStage3Background() {
  DrawTextureEx(bgTextureLayer1, (Vector2){scrollingLayer1, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer1,
                (Vector2){bgTextureLayer1.width + scrollingLayer1, 0}, 0.0f,
                1.0f, GRAY);

  DrawTextureEx(bgTextureLayer2, (Vector2){scrollingLayer2, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer2,
                (Vector2){bgTextureLayer2.width + scrollingLayer2, 0}, 0.0f,
                1.0f, GRAY);

  DrawTextureEx(bgTextureLayer3, (Vector2){scrollingLayer3, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer3,
                (Vector2){bgTextureLayer3.width + scrollingLayer3, 0}, 0.0f,
                1.0f, GRAY);

  DrawTextureEx(bgTextureLayer4, (Vector2){scrollingLayer4, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer4,
                (Vector2){bgTextureLayer4.width + scrollingLayer4, 0}, 0.0f,
                1.0f, GRAY);

  DrawTextureEx(bgTextureLayer5, (Vector2){scrollingLayer5, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer5,
                (Vector2){bgTextureLayer5.width + scrollingLayer5, 0}, 0.0f,
                1.0f, GRAY);

  DrawTextureEx(bgTextureLayer6, (Vector2){scrollingLayer6, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer6,
                (Vector2){bgTextureLayer6.width + scrollingLayer6, 0}, 0.0f,
                1.0f, GRAY);

  DrawTextureEx(bgTextureLayer7, (Vector2){scrollingLayer7, 0}, 0.0f, 1.0f,
                GRAY);
  DrawTextureEx(bgTextureLayer7,
                (Vector2){bgTextureLayer7.width + scrollingLayer7, 0}, 0.0f,
                1.0f, GRAY);
}

void UnloadStage3Background() {
  UnloadTexture(bgTextureLayer1);
  UnloadTexture(bgTextureLayer2);
  UnloadTexture(bgTextureLayer3);
  UnloadTexture(bgTextureLayer4);
  UnloadTexture(bgTextureLayer5);
  UnloadTexture(bgTextureLayer6);
  UnloadTexture(bgTextureLayer7);
}
