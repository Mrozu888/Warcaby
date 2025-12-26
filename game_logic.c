#include "game_logic.h"
#include <stdlib.h>
#include <math.h>

void init_game(GameState *game) {
    game->current_turn = PLAYER_WHITE;
    game->game_over = false;
    game->paused = false;
    game->moves_count = 0;
    game->winner = 0;
    game->white_pieces = 0;
    game->black_pieces = 0;
    game->start_time = time(NULL);
    game->mode = MODE_PVP; // Default mode

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            game->board[y][x] = EMPTY;
            if ((x + y) % 2 != 0) { // Ciemne pola
                if (y < 3) {
                    game->board[y][x] = BLACK_PIECE;
                    game->black_pieces++;
                } else if (y > 4) {
                    game->board[y][x] = WHITE_PIECE;
                    game->white_pieces++;
                }
            }
        }
    }
}

bool is_valid_move(GameState *game, Position from, Position to, bool is_capture) {
    if (to.x < 0 || to.x >= BOARD_SIZE || to.y < 0 || to.y >= BOARD_SIZE) return false;
    if (game->board[to.y][to.x] != EMPTY) return false;

    PieceType p = game->board[from.y][from.x];
    int dy = to.y - from.y;
    int dx = to.x - from.x;

    // Kierunek ruchu (Białe w górę (-), Czarne w dół (+)) - chyba że damka
    if (p == WHITE_PIECE && dy > 0 && !is_capture) return false;
    if (p == BLACK_PIECE && dy < 0 && !is_capture) return false;

    // Zwykły ruch
    if (!is_capture && abs(dx) == 1 && abs(dy) == 1) return true;

    // Bicie
    if (is_capture && abs(dx) == 2 && abs(dy) == 2) {
        int midX = from.x + dx / 2;
        int midY = from.y + dy / 2;
        PieceType midP = game->board[midY][midX];

        if (midP == EMPTY) return false;

        // Czy bijemy wroga?
        bool is_enemy = false;
        if (game->current_turn == PLAYER_WHITE && (midP == BLACK_PIECE || midP == BLACK_KING)) is_enemy = true;
        if (game->current_turn == PLAYER_BLACK && (midP == WHITE_PIECE || midP == WHITE_KING)) is_enemy = true;

        if (is_enemy) return true;
    }
    return false;
}

bool attempt_move(GameState *game, Position from, Position to) {
    PieceType p = game->board[from.y][from.x];
    if (p == EMPTY) return false;

    // Walidacja właściciela pionka
    if (game->current_turn == PLAYER_WHITE && (p == BLACK_PIECE || p == BLACK_KING)) return false;
    if (game->current_turn == PLAYER_BLACK && (p == WHITE_PIECE || p == WHITE_KING)) return false;

    bool is_capture = (abs(to.x - from.x) == 2);

    if (is_valid_move(game, from, to, is_capture)) {
        // Wykonaj ruch
        game->board[to.y][to.x] = game->board[from.y][from.x];
        game->board[from.y][from.x] = EMPTY;

        // Bicie
        if (is_capture) {
            int midX = from.x + (to.x - from.x) / 2;
            int midY = from.y + (to.y - from.y) / 2;
            game->board[midY][midX] = EMPTY;
            if (game->current_turn == PLAYER_WHITE) game->black_pieces--;
            else game->white_pieces--;
        }

        // Promocja
        if (game->current_turn == PLAYER_WHITE && to.y == 0) game->board[to.y][to.x] = WHITE_KING;
        if (game->current_turn == PLAYER_BLACK && to.y == 7) game->board[to.y][to.x] = BLACK_KING;

        game->moves_count++;
        game->current_turn = (game->current_turn == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;
        return true;
    }
    return false;
}

void check_win_condition(GameState *game) {
    if (game->white_pieces <= 0) {
        game->winner = 2; // Czarne
        game->game_over = true;
        game->end_time = time(NULL);
    } else if (game->black_pieces <= 0) {
        game->winner = 1; // Białe
        game->game_over = true;
        game->end_time = time(NULL);
    }
}
