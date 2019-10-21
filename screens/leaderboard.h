#ifndef LEADERBOARD_H
#define LEADERBOARD_H
typedef struct {
  char name[32];
  char email[32];
  char description[128];
  int score;
} Entry;

Entry* read_leaderboard(int* size);

#endif
