#include "leaderboard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENTRIES_SIZE 1000

Entry entries[MAX_ENTRIES_SIZE];

int cmpFn(const void *e1, const void *e2) {
  return ((Entry *)e2)->score - ((Entry *)e1)->score;
}

Entry *read_leaderboard(int *size) {
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;
  int entries_len = 0;
  fp = fopen("data.txt", "r");
  if (fp != NULL) {
    while ((read = getline(&line, &len, fp)) != -1) {
      strcpy(entries[entries_len].name, strtok(line, ","));
      strcpy(entries[entries_len].email, strtok(NULL, ","));
      strcpy(entries[entries_len].description, strtok(NULL, ","));
      entries[entries_len].score = atoi(strtok(NULL, ","));
      entries_len++;
    }
    qsort(entries, entries_len, sizeof(Entry), cmpFn);
  } else {
    printf("Error\n");
  }
  fclose(fp);
  if (line)
    free(line);

  *size = entries_len;
  return entries;
}
