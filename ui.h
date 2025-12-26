#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include "common.h"

// Definicje par kolorów
#define CP_LIGHT_TILE 1   // Jasne pole (niegrywalne)
#define CP_DARK_TILE  2   // Ciemne pole (grywalne)
#define CP_P1         3   // Pionek gracza 1 (Białe - Czerwony na czarnym)
#define CP_P2         4   // Pionek gracza 2 (Czarne - Cyjan na czarnym)
#define CP_CURSOR     5   // Kursor (puste pole)
#define CP_SELECTED   6   // Zaznaczenie (puste pole)
#define CP_TEXT       7   // Zwykły tekst

// Nowe pary dla spójności (Pionek na tle kursora/zaznaczenia)
#define CP_P1_CURSOR    8 // P1 na zielonym
#define CP_P2_CURSOR    9 // P2 na zielonym
#define CP_P1_SELECTED 10 // P1 na żółtym
#define CP_P2_SELECTED 11 // P2 na żółtym
#define CP_FRAME       12 // Ramka planszy

void init_ui();
void close_ui();
int show_start_menu();
int show_pause_menu();
void show_end_screen(GameState *game);
void show_stats_screen();
void draw_game_screen(GameState *game, Position cursor, Position selected, bool is_selected);

#endif // UI_H
