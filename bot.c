#include "bot.h"
#include "game_logic.h"
#include <stdlib.h>

void make_bot_move(GameState *game) {
    // Bardzo prosta AI: próbuje losowe ruchy aż znajdzie poprawny
    // W pełnej wersji należy zaimplementować Minimax

    Position from, to;
    int attempts = 0;
    bool moved = false;

    // Najpierw szukamy bicia (priorytet) - uproszczone dla przykładu
    // Tutaj pętla "brute force" po wszystkich polach
    while (!moved && attempts < 1000) {
        from.x = rand() % BOARD_SIZE;
        from.y = rand() % BOARD_SIZE;

        // Jeśli to pionek bota (Czarne)
        PieceType p = game->board[from.y][from.x];
        if (p == BLACK_PIECE || p == BLACK_KING) {
            // Losuj kierunek
            int dirX = (rand() % 2 == 0) ? 1 : -1;
            int dirY = (rand() % 2 == 0) ? 1 : -1;

            // Sprawdź ruch o 1 (zwykły) lub o 2 (bicie)
            int dist = (rand() % 2) + 1;

            to.x = from.x + (dirX * dist);
            to.y = from.y + (dirY * dist);

            if (attempt_move(game, from, to)) {
                moved = true;
            }
        }
        attempts++;
    }
}
