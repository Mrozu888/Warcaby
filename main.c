#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <locale.h>
#include "common.h"
#include "game_logic.h"
#include "ui.h"
#include "bot.h"
#include "stats.h"

// Deklaracja zmiennej globalnej ncurses
extern int ESCDELAY;

volatile sig_atomic_t stop_flag = 0;

// obsluga sygnalow
void handle_signal(int sig) {
    stop_flag = 1;
}

void print_final_stats(GameState *game) {
    save_game_stats(game);
    show_end_screen(game);
}

int main(int argc, char *argv[]) {
    setlocale(LC_ALL, "");
    setenv("ESCDELAY", "25", 1);

    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    init_ui();
    ESCDELAY = 25;

    // main loop
    while (!stop_flag) {
        int menu_choice = show_start_menu();
        if (menu_choice == -1) {
            stop_flag = 1;
            break; //wyjscie
        }

        if (menu_choice == 2) {
            show_stats_screen();
            continue;
        }

        GameState game;
        init_game(&game);

        if (menu_choice == 1) {
            game.mode = MODE_BOT;
        } else {
            game.mode = MODE_PVP;
        }

        Position cursor = {0, 0};
        Position selected = {-1, -1};
        bool is_selected = false;
        bool back_to_menu = false;

        timeout(100);

        // game loop
        while (!game.game_over && !stop_flag && !back_to_menu) {
            check_win_condition(&game);
            draw_game_screen(&game, cursor, selected, is_selected);

            if (game.mode == MODE_BOT && game.current_turn == PLAYER_BLACK) {
                usleep(500000);
                make_bot_move(&game);
                continue;
            }

            int ch = getch();

            // obsluga q
            if (ch == 'q') {
                game.paused = true;
                int pause_choice = show_pause_menu();
                game.paused = false;

                timeout(100);

                if (pause_choice == 1) {
                    back_to_menu = true;
                    break;
                }
                continue;
            }

            switch (ch) {
                case KEY_UP:    if (cursor.y > 0) cursor.y--; break;
                case KEY_DOWN:  if (cursor.y < BOARD_SIZE - 1) cursor.y++; break;
                case KEY_LEFT:  if (cursor.x > 0) cursor.x--; break;
                case KEY_RIGHT: if (cursor.x < BOARD_SIZE - 1) cursor.x++; break;
                case ' ': // Space
                case '\n': // Enter
                case KEY_ENTER:
                    if (!is_selected) {
                        PieceType p = game.board[cursor.y][cursor.x];
                        if (p != EMPTY) {
                            if ((game.current_turn == PLAYER_WHITE && (p == WHITE_PIECE || p == WHITE_KING)) ||
                                (game.current_turn == PLAYER_BLACK && (p == BLACK_PIECE || p == BLACK_KING))) {
                                selected = cursor;
                                is_selected = true;
                            }
                        }
                    } else {
                        if (cursor.x == selected.x && cursor.y == selected.y) {
                            is_selected = false;
                        } else {
                            if (attempt_move(&game, selected, cursor)) {
                                is_selected = false;
                            }
                        }
                    }
                    break;
            }
        }

        if (!stop_flag && !back_to_menu) {
            print_final_stats(&game);
        }
    }

    close_ui();

    return 0;
}
