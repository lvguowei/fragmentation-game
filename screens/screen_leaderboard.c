#include "raylib.h"
#include "screens.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static bool finishScreen;
static int framesCount;

void InitLeaderBoardScreen(void) {
  framesCount = 0;
  finishScreen = false;

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen("data.txt", "r");
  if (fp != NULL) {
    while ((read = getline(&line, &len, fp)) != -1) {
      printf("Retrieved line of length %zu:\n", read);
      printf("%s", line);
    }
  } else {
    printf("Error\n");
    printf("%d", errno);
  }
  /* fclose(fp); */
  /* if (line) free(line); */
}
void UpdateLeaderBoardScreen(void) {
  framesCount++;
}
void DrawLeaderBoardScreen(void) {
  DrawCircle(100, 100, 100, RED);
}
void UnloadLeaderBoardScreen(void){}
bool FinishLeaderBoardScreen(void){
  return false;
}
