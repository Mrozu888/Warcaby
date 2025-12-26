#ifndef STATS_H
#define STATS_H

#include "common.h"

#define MAX_RECORDS 100

typedef struct {
    char date[20];
    char winner[20];
    int moves;
    int duration;
} GameRecord;

void save_game_stats(GameState *game);
int load_game_stats(GameRecord *records, int max_records);

#endif // STATS_H
