#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include "common.h"

void init_game(GameState *game);
bool is_valid_move(GameState *game, Position from, Position to, bool is_capture);
bool attempt_move(GameState *game, Position from, Position to);
void check_win_condition(GameState *game);

#endif // GAME_LOGIC_H
