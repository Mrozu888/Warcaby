#include "ui.h"
#include <string.h>
#include <stdlib.h>

void init_ui() {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    start_color();

    // Definicja kolorów (Foreground, Background)

    // 1. Jasne pole (tło białe/szare)
    init_pair(CP_LIGHT_TILE, COLOR_BLACK, COLOR_WHITE);

    // 2. Ciemne pole (tło czarne)
    init_pair(CP_DARK_TILE, COLOR_WHITE, COLOR_BLACK);

    // 3. Pionki na ciemnym polu (standard)
    init_pair(CP_P1, COLOR_RED, COLOR_BLACK);
    init_pair(CP_P2, COLOR_CYAN, COLOR_BLACK);

    // 4. Kursor i Zaznaczenie (tła)
    init_pair(CP_CURSOR, COLOR_BLACK, COLOR_GREEN);
    init_pair(CP_SELECTED, COLOR_BLACK, COLOR_YELLOW);

    // 5. Tekst menu
    init_pair(CP_TEXT, COLOR_WHITE, COLOR_BLACK);

    // 6. Pionki na tle kursora (zielonym)
    init_pair(CP_P1_CURSOR, COLOR_RED, COLOR_GREEN);
    init_pair(CP_P2_CURSOR, COLOR_CYAN, COLOR_GREEN);

    // 7. Pionki na tle zaznaczenia (żółtym)
    init_pair(CP_P1_SELECTED, COLOR_RED, COLOR_YELLOW);
    init_pair(CP_P2_SELECTED, COLOR_CYAN, COLOR_YELLOW);

    // 8. Ramka
    init_pair(CP_FRAME, COLOR_BLUE, COLOR_BLACK);
}

void close_ui() {
    endwin();
}

// Funkcja pomocnicza do rysowania środka
void center_print(int y, const char *text) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    mvprintw(y, (cols - strlen(text)) / 2, "%s", text);
}

int show_start_menu() {
    int choice = 0;
    const char *options[] = {
        "Gra z kolega (PvP)",
        "Gra z Botem (PvE)",
        "Wyjscie"
    };
    int n_options = 3;

    while(1) {
        clear();
        attron(COLOR_PAIR(CP_TEXT) | A_BOLD);
        center_print(5, "=== PROJEKT WARCABY (POSIX) ===");
        attroff(COLOR_PAIR(CP_TEXT) | A_BOLD);

        for(int i = 0; i < n_options; i++) {
            if(i == choice) {
                attron(A_REVERSE); // Odwróć kolory dla zaznaczenia
                mvprintw(10 + i*2, 30, " > %s < ", options[i]);
                attroff(A_REVERSE);
            } else {
                mvprintw(10 + i*2, 30, "   %s   ", options[i]);
            }
        }

        center_print(20, "Uzyj strzalek gora/dol i zatwierdz ENTER");

        int ch = getch();
        switch(ch) {
            case KEY_UP:
                choice--;
                if (choice < 0) choice = n_options - 1;
                break;
            case KEY_DOWN:
                choice++;
                if (choice >= n_options) choice = 0;
                break;
            case 10: // Enter
            case 13: // Enter (czasem 13)
                if (choice == 2) return -1; // Wyjście
                return choice; // 0 lub 1
        }
    }
}

int show_pause_menu() {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    WINDOW *win = newwin(10, 40, (rows-10)/2, (cols-40)/2);
    keypad(win, TRUE);
    box(win, 0, 0);
    wbkgd(win, COLOR_PAIR(CP_DARK_TILE));

    int choice = 0;
    const char *options[] = { "Wznow gre", "Wyjdz do menu" };

    while(1) {
        mvwprintw(win, 2, 14, "PAUZA");

        for(int i=0; i<2; i++) {
            if (i == choice) wattron(win, A_REVERSE);
            mvwprintw(win, 5+i, 12, "%s", options[i]);
            if (i == choice) wattroff(win, A_REVERSE);
        }

        wrefresh(win);

        int ch = wgetch(win);
        switch(ch) {
            case KEY_UP: case KEY_DOWN:
                choice = !choice; // Przełączanie 0 <-> 1
                break;
            case 10: // Enter
            case 13:
                delwin(win);
                touchwin(stdscr);
                refresh();
                return choice;
            case 'q': // Tylko q zamyka pauzę (jako powrót do gry, jeśli ktoś się rozmyślił)
            case 27:
                delwin(win);
                touchwin(stdscr);
                refresh();
                return 0;
        }
    }
}

void show_end_screen(GameState *game) {
    clear();
    attron(A_BOLD);
    center_print(5, "KONIEC GRY");
    attroff(A_BOLD);

    char buf[64];
    if (game->winner == 1) sprintf(buf, "Wygral: BIALY (Gracz 1)");
    else if (game->winner == 2) sprintf(buf, "Wygral: CZARNY (Gracz 2 / Bot)");
    else sprintf(buf, "Gra przerwana / Remis");
    center_print(8, buf);

    sprintf(buf, "Liczba ruchow: %d", game->moves_count);
    center_print(10, buf);

    double duration = difftime(game->end_time, game->start_time);
    sprintf(buf, "Czas gry: %.0f sekund", duration);
    center_print(11, buf);

    center_print(15, "Nacisnij dowolny klawisz aby wyjsc...");
    refresh();
    timeout(-1);
    getch();
}

void draw_game_screen(GameState *game, Position cursor, Position selected, bool is_selected) {
    clear();

    int start_y = 3;
    int start_x = 5;
    int board_width = BOARD_SIZE * 4;
    int board_height = BOARD_SIZE * 2;

    // Rysowanie ramki wokół planszy
    attron(COLOR_PAIR(CP_FRAME));
    // Górna i dolna krawędź
    mvhline(start_y - 1, start_x, ACS_HLINE, board_width);
    mvhline(start_y + board_height, start_x, ACS_HLINE, board_width);
    // Lewa i prawa krawędź
    mvvline(start_y, start_x - 1, ACS_VLINE, board_height);
    mvvline(start_y, start_x + board_width, ACS_VLINE, board_height);
    // Rogi
    mvaddch(start_y - 1, start_x - 1, ACS_ULCORNER);
    mvaddch(start_y - 1, start_x + board_width, ACS_URCORNER);
    mvaddch(start_y + board_height, start_x - 1, ACS_LLCORNER);
    mvaddch(start_y + board_height, start_x + board_width, ACS_LRCORNER);
    attroff(COLOR_PAIR(CP_FRAME));

    // Rysowanie planszy
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {

            int draw_x = start_x + (x * 4);
            int draw_y = start_y + (y * 2);

            // Określenie stanu pola (zwykłe, kursor, zaznaczone)
            bool is_cursor = (cursor.x == x && cursor.y == y);
            bool is_sel_tile = (is_selected && selected.x == x && selected.y == y);

            // Domyślny kolor tła pola
            int bg_pair = ((x + y) % 2 == 0) ? CP_LIGHT_TILE : CP_DARK_TILE;

            // Jeśli kursor lub zaznaczenie, nadpisujemy tło
            if (is_sel_tile) bg_pair = CP_SELECTED;
            else if (is_cursor) bg_pair = CP_CURSOR;

            PieceType p = game->board[y][x];

            // Wybór pary kolorów (Tekst + Tło)
            int final_pair = bg_pair;

            if (p != EMPTY) {
                // Jeśli jest pionek, musimy dobrać parę: (KolorPionka, KolorTłaPola)
                if (p == WHITE_PIECE || p == WHITE_KING) {
                    if (is_sel_tile) final_pair = CP_P1_SELECTED;
                    else if (is_cursor) final_pair = CP_P1_CURSOR;
                    else final_pair = CP_P1;
                } else {
                    if (is_sel_tile) final_pair = CP_P2_SELECTED;
                    else if (is_cursor) final_pair = CP_P2_CURSOR;
                    else final_pair = CP_P2;
                }
            }

            attron(COLOR_PAIR(final_pair));

            // Rysowanie pola (2 linie wysokości)
            // Linia 1
            mvprintw(draw_y, draw_x, "    ");
            // Linia 2
            mvprintw(draw_y + 1, draw_x, "    ");

            // Rysowanie pionka na środku
            if (p != EMPTY) {
                if (p == WHITE_KING || p == BLACK_KING) attron(A_BOLD);

                const char *symbol = (p == WHITE_PIECE || p == BLACK_PIECE) ? "●" : "♔";
                // Wyśrodkowanie w polu 4x2: x+1, y+0 (lub y+1 dla idealnego środka w pionie, ale y+0 jest czytelniejsze)
                mvprintw(draw_y, draw_x + 1, " %s", symbol);

                if (p == WHITE_KING || p == BLACK_KING) attroff(A_BOLD);
            }

            attroff(COLOR_PAIR(final_pair));
        }
    }

    // Rysowanie współrzędnych
    attron(COLOR_PAIR(CP_TEXT));
    for(int i=0; i<8; i++) {
        mvprintw(start_y + i*2, start_x - 3, "%d", 8-i); // Liczby po lewej
        mvprintw(start_y + board_height + 1, start_x + 2 + i*4, "%c", 'A'+i); // Litery na dole
    }
    attroff(COLOR_PAIR(CP_TEXT));

    // Panel boczny
    int stats_x = 50;
    mvprintw(start_y, stats_x, "--- STATYSTYKI ---");

    mvprintw(start_y + 2, stats_x, "Tura: ");
    if (game->current_turn == PLAYER_WHITE) {
        attron(COLOR_PAIR(CP_P1)); printw("BIALY"); attroff(COLOR_PAIR(CP_P1));
    } else {
        attron(COLOR_PAIR(CP_P2)); printw("CZARNY"); attroff(COLOR_PAIR(CP_P2));
    }

    mvprintw(start_y + 4, stats_x, "Biale: %d ", game->white_pieces);
    mvprintw(start_y + 5, stats_x, "Czarne: %d ", game->black_pieces);

    time_t now = time(NULL);
    static time_t paused_start = 0;
    static double paused_accum = 0.0;
    if (game->paused) {
        if (paused_start == 0) paused_start = now;
    } else {
        if (paused_start != 0) {
            paused_accum += difftime(now, paused_start);
            paused_start = 0;
        }
    }
    double elapsed = difftime(now, game->start_time) - paused_accum;
    mvprintw(start_y + 7, stats_x, "Czas: %.0f s", elapsed);

    mvprintw(start_y + 10, stats_x, "STEROWANIE:");
    mvprintw(start_y + 11, stats_x, "[Q]      Pauza");
    mvprintw(start_y + 12, stats_x, "[SPACJA] Wybierz");

    refresh();
}
