#include "game_logic.h"
#include <stdlib.h>
#include <math.h>


bool can_pawn_capture(GameState *game, Position pos);
bool can_king_capture(GameState *game, Position pos);
bool can_piece_capture(GameState *game, Position pos);
bool player_has_capture(GameState *game);
int sign(int x);

void init_game(GameState *game) {
    game->current_turn = PLAYER_WHITE;
    game->game_over = false;
    game->paused = false;
    game->moves_count = 0;
    game->winner = 0;
    game->white_pieces = 0;
    game->black_pieces = 0;
    game->start_time = time(NULL);
    game->mode = MODE_PVP;
    game->is_chain_capture = false;
    game->chain_piece_pos.x = -1;
    game->chain_piece_pos.y = -1;

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

int sign(int x) {
    return (x > 0) - (x < 0);
}

bool is_valid_pos(Position p) {
    return p.x >= 0 && p.x < BOARD_SIZE && p.y >= 0 && p.y < BOARD_SIZE;
}

bool is_enemy(GameState *game, PieceType p) {
    if (p == EMPTY) return false;
    if (game->current_turn == PLAYER_WHITE) return (p == BLACK_PIECE || p == BLACK_KING);
    else return (p == WHITE_PIECE || p == WHITE_KING);
}

bool is_own(GameState *game, PieceType p) {
    if (p == EMPTY) return false;
    if (game->current_turn == PLAYER_WHITE) return (p == WHITE_PIECE || p == WHITE_KING);
    else return (p == BLACK_PIECE || p == BLACK_KING);
}

// sprawdzenie czy jest wrog po przekatnej
bool can_pawn_capture(GameState *game, Position pos) {
    int dirs[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    for (int i = 0; i < 4; i++) {
        int midX = pos.x + dirs[i][0];
        int midY = pos.y + dirs[i][1];
        int toX = pos.x + dirs[i][0] * 2;
        int toY = pos.y + dirs[i][1] * 2;

        if (toX >= 0 && toX < BOARD_SIZE && toY >= 0 && toY < BOARD_SIZE) {
            if (game->board[toY][toX] == EMPTY) {
                if (is_enemy(game, game->board[midY][midX])) {
                    return true;
                }
            }
        }
    }
    return false;
}

// sprawdzenie czy jest wrog po przekatnej (damka)
bool can_king_capture(GameState *game, Position pos) {
    int dirs[4][2] = {{-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    for (int i = 0; i < 4; i++) {
        int dx = dirs[i][0];
        int dy = dirs[i][1];
        int x = pos.x + dx;
        int y = pos.y + dy;

        bool found_enemy = false;

        while (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            PieceType p = game->board[y][x];
            if (p == EMPTY) {
                if (found_enemy) return true; // Puste pole za wrogiem = bicie możliwe
            } else if (is_own(game, p)) {
                break;
            } else if (is_enemy(game, p)) {
                if (found_enemy) break;
                found_enemy = true;
            }
            x += dx;
            y += dy;
        }
    }
    return false;
}

// sprawdzenie pionka
bool can_piece_capture(GameState *game, Position pos) {
    PieceType p = game->board[pos.y][pos.x];
    if (p == WHITE_PIECE || p == BLACK_PIECE) return can_pawn_capture(game, pos);
    if (p == WHITE_KING || p == BLACK_KING) return can_king_capture(game, pos);
    return false;
}

// sprawdzenie czy gracz ma mozliwosc wykonania bicia
bool player_has_capture(GameState *game) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (is_own(game, game->board[y][x])) {
                Position pos = {x, y};
                if (can_piece_capture(game, pos)) return true;
            }
        }
    }
    return false;
}

// glowna funckja sprawdzajaca moziwosc wykonania ruchu
bool attempt_move(GameState *game, Position from, Position to) {
    if (!is_valid_pos(from) || !is_valid_pos(to)) return false;

    PieceType p = game->board[from.y][from.x];
    if (!is_own(game, p)) return false;

    if (game->is_chain_capture) {
        if (from.x != game->chain_piece_pos.x || from.y != game->chain_piece_pos.y) return false;
    }

    // trzeba bic
    bool capture_available = player_has_capture(game);

    int dx = to.x - from.x;
    int dy = to.y - from.y;
    int adx = abs(dx);
    int ady = abs(dy);

    if (adx != ady) return false; // Ruch tylko po przekątnej
    if (adx == 0) return false;
    if (game->board[to.y][to.x] != EMPTY) return false;

    bool is_capture = false;
    Position captured_pos = {-1, -1};

    // Logika dla PIONKA
    if (p == WHITE_PIECE || p == BLACK_PIECE) {

        if (adx == 1) {
            if (capture_available) return false; // wymuszenie bicia
            // tylko do przodu
            if (p == WHITE_PIECE && dy > 0) return false;
            if (p == BLACK_PIECE && dy < 0) return false;

            // Wykonanie ruchu
            game->board[to.y][to.x] = p;
            game->board[from.y][from.x] = EMPTY;
        }
        // Bicie
        else if (adx == 2) {
            int midX = from.x + dx / 2;
            int midY = from.y + dy / 2;
            PieceType midP = game->board[midY][midX];

            if (!is_enemy(game, midP)) return false;

            is_capture = true;
            captured_pos.x = midX;
            captured_pos.y = midY;

            // ruch
            game->board[to.y][to.x] = p;
            game->board[from.y][from.x] = EMPTY;
            game->board[midY][midX] = EMPTY;
        } else {
            return false; // pionek nie moze dalej niz 2 pola
        }
    }
    // Logika dla damki
    else {
        int stepX = sign(dx);
        int stepY = sign(dy);
        int x = from.x + stepX;
        int y = from.y + stepY;
        int enemy_count = 0;

        while (x != to.x) {
            if (game->board[y][x] != EMPTY) {
                if (is_own(game, game->board[y][x])) return false; // blokada przez swojego pionka
                if (is_enemy(game, game->board[y][x])) {
                    if (enemy_count > 0) return false; // nie mozna przeskoczyc 2
                    enemy_count++;
                    captured_pos.x = x;
                    captured_pos.y = y;
                }
            }
            x += stepX;
            y += stepY;
        }

        if (enemy_count == 0){
            if (capture_available) return false; // wymuszenie bicia
            game->board[to.y][to.x] = p;
            game->board[from.y][from.x] = EMPTY;
        } else {
            is_capture = true;
            game->board[to.y][to.x] = p;
            game->board[from.y][from.x] = EMPTY;
            game->board[captured_pos.y][captured_pos.x] = EMPTY;
        }
    }


    if (is_capture) {
        if (game->current_turn == PLAYER_WHITE) game->black_pieces--;
        else game->white_pieces--;

        // promotion
        bool promoted = false;
        if (p == WHITE_PIECE && to.y == 0) {
            game->board[to.y][to.x] = WHITE_KING;
            promoted = true;
        }
        if (p == BLACK_PIECE && to.y == 7) {
            game->board[to.y][to.x] = BLACK_KING;
            promoted = true;
        }

        // check if bicie
        if (can_piece_capture(game, to)) {
            game->is_chain_capture = true;
            game->chain_piece_pos = to;
            return true;
        }
    } else {
        // check if krolowka
        if (p == WHITE_PIECE && to.y == 0) game->board[to.y][to.x] = WHITE_KING;
        if (p == BLACK_PIECE && to.y == 7) game->board[to.y][to.x] = BLACK_KING;
    }

    // koniec tury
    game->is_chain_capture = false;
    game->chain_piece_pos.x = -1;
    game->moves_count++;
    game->current_turn = (game->current_turn == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE;

    return true;
}

void check_win_condition(GameState *game) {
    if (game->white_pieces <= 0) {
        game->winner = 2; // Czarne
        game->game_over = true;
        game->end_time = time(NULL);
    } else if (game->black_pieces <= 0) {
        game->winner = 1; // whte
        game->game_over = true;
        game->end_time = time(NULL);
    }
}
