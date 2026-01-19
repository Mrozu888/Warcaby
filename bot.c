#include "bot.h"
#include "game_logic.h"
#include <stdlib.h>

// Funkcja pomocnicza do sprawdzania granic w bocie
bool is_in_bounds(int x, int y) {
    return x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
}

void make_bot_move(GameState *game) {
    // szukanie bicia wielokrotnego (zrobienie kolejnego bicia)
    if (game->is_chain_capture) {
        Position from = game->chain_piece_pos;
        // Sprawdź wszystkie możliwe bicia dla tego pionka
        for (int dy = -2; dy <= 2; dy++) {
            for (int dx = -2; dx <= 2; dx++) {
                if (dx == 0 || dy == 0) continue;

                Position to = {from.x + dx, from.y + dy};
                if (is_in_bounds(to.x, to.y)) {
                    if (attempt_move(game, from, to)) return;
                }
            }
        }
        // dla damki
        if (game->board[from.y][from.x] == BLACK_KING) {
             for (int dist = 2; dist < BOARD_SIZE; dist++) {
                 int dirs[4][2] = {{-1,-1}, {1,-1}, {-1,1}, {1,1}};
                 for(int i=0; i<4; i++) {
                     Position to = {from.x + dirs[i][0]*dist, from.y + dirs[i][1]*dist};
                     if (is_in_bounds(to.x, to.y)) {
                         if (attempt_move(game, from, to)) return;
                     }
                 }
             }
        }
        game->is_chain_capture = false;
        game->current_turn = PLAYER_WHITE;
        return;
    }

    // szukanie ruchu
    Position my_pieces[BOARD_SIZE * BOARD_SIZE / 2];
    int count = 0;

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            PieceType p = game->board[y][x];
            if (p == BLACK_PIECE || p == BLACK_KING) {
                my_pieces[count++] = (Position){x, y};
            }
        }
    }

    // losowanie ruchu bota
    for (int i = 0; i < count; i++) {
        int r = rand() % count;
        Position temp = my_pieces[i];
        my_pieces[i] = my_pieces[r];
        my_pieces[r] = temp;
    }

    // wykonanie ruchu przez boota
    for (int i = 0; i < count; i++) {
        Position from = my_pieces[i];
        PieceType p = game->board[from.y][from.x];

        int max_dist = (p == BLACK_KING) ? BOARD_SIZE : 3;
        int dirs[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};

        // szukanie bic
        for (int dist = 2; dist < max_dist; dist++) {
            for (int d = 0; d < 4; d++) {
                Position to = {from.x + dirs[d][0] * dist, from.y + dirs[d][1] * dist};
                if (is_in_bounds(to.x, to.y)) {
                    if (attempt_move(game, from, to)) return;
                }
            }
        }

        // brak bic
        for (int dist = 1; dist < max_dist; dist++) {
             for (int d = 0; d < 4; d++) {
                Position to = {from.x + dirs[d][0] * dist, from.y + dirs[d][1] * dist};
                if (is_in_bounds(to.x, to.y)) {
                    if (attempt_move(game, from, to)) return;
                }
            }
        }
    }

    // Brak ruchu = przegrana
    game->winner = 1;
    game->game_over = true;
    game->end_time = time(NULL);
}
