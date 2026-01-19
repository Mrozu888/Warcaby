#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "stats.h"

#define STATS_FILE "stats.txt"

// zapisanie statystyk skonczonej gry
void save_game_stats(GameState *game) {
    FILE *fp = fopen(STATS_FILE, "a");
    if (!fp) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_str[20];
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M", t);

    const char *winner_str = (game->winner == 1) ? "BIALY" : (game->winner == 2 ? "CZARNY" : "REMIS");
    int duration = (int)difftime(game->end_time, game->start_time);

    // zapisanie do pliku informacji o dacie, zwyciezcy, ilosci ruchow oraz czasie gry
    fprintf(fp, "%s;%s;%d;%d\n", date_str, winner_str, game->moves_count, duration);
    fclose(fp);
}


// wczytanie statystyk z pliku
int load_game_stats(GameRecord *records, int max_records) {
    FILE *fp = fopen(STATS_FILE, "r");
    if (!fp) return 0;

    int count = 0;
    char line[128];

    while (fgets(line, sizeof(line), fp) && count < max_records) {
        // Format: DATE;WINNER;MOVES;DURATION
        char *token = strtok(line, ";");
        if (!token) continue;
        strncpy(records[count].date, token, 19);
        records[count].date[19] = '\0';

        token = strtok(NULL, ";");
        if (!token) continue;
        strncpy(records[count].winner, token, 19);
        records[count].winner[19] = '\0';

        token = strtok(NULL, ";");
        if (!token) continue;
        records[count].moves = atoi(token); // zamiana ascii na int

        token = strtok(NULL, ";");
        if (!token) continue;
        records[count].duration = atoi(token);

        count++;
    }

    fclose(fp);
    return count;
}
