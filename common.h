#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <time.h>

#define BOARD_SIZE 8

typedef enum {
    EMPTY,
    WHITE_PIECE,
    BLACK_PIECE,
    WHITE_KING,
    BLACK_KING
} PieceType;

typedef enum {
    PLAYER_WHITE = 1,
    PLAYER_BLACK = 2
} Player;

typedef enum {
    MODE_PVP,
    MODE_BOT
} GameMode;

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    PieceType board[BOARD_SIZE][BOARD_SIZE];
    Player current_turn;
    bool game_over;
    bool paused;
    int moves_count;
    int winner; // 0: none, 1: white, 2: black
    int white_pieces;
    int black_pieces;
    time_t start_time;
    time_t end_time;
    GameMode mode;
} GameState;

#endif // COMMON_H
