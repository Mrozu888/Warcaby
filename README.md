# Projekt Warcaby (POSIX)

## Opis
Projekt jest implementacją klasycznej gry w Warcaby w języku C, przeznaczoną do uruchamiania w terminalu systemów zgodnych z POSIX (UNIX). Interfejs graficzny został zrealizowany przy użyciu biblioteki `ncurses`, co zapewnia czytelną i responsywną rozgrywkę w terminalu.

Gra oferuje możliwość rywalizacji z drugim graczem lub z komputerem (Bot).

## Funkcjonalności
- **Tryby gry**:
  - Gracz vs Gracz (PvP).
  - Gracz vs Bot.
- **Zaawansowana logika gry w warcaby**:
  - Obsługa Królówek – ruch o dowolną liczbę pól.
  - Wymuszenie bicia.
  - Bicie wielokrotne.
  - Bicie do tyłu dla zwykłych pionków.
- **Statystyki**:
  - Automatyczny zapis wyników gier do pliku `stats.txt`.
  - Przeglądarka historii gier w menu głównym (Data, Zwycięzca, Liczba ruchów, Czas).
- **Interfejs**:
  - Kolorowa plansza (zależna od ustawień terminala).
  - Menu pauzy.
  - Przegląd statystyk.

## Wymagania
Aby skompilować i uruchomić projekt, potrzebujesz:
- Kompilatora C (GCC lub Clang) wspierającego standard C11.
- Systemu budowania **CMake** (wersja 3.10 lub nowsza).
- Biblioteki **ncurses** (zazwyczaj dostępna standardowo na Linux/macOS).

## Kompilacja i Uruchomienie

### Z poziomu terminala:
1. Utwórz katalog budowania:
   ```bash
   mkdir build
   cd build
   ```
2. Skonfiguruj projekt przy użyciu CMake:
   ```bash
   cmake ..
   ```
3. Zbuduj projekt:
   ```bash
   make
   ```
4. Uruchom grę:
   ```bash
   ./Warcaby
   ```

### Z poziomu CLion:
1. Otwórz projekt w CLion.
2. Poczekaj na załadowanie projektu CMake.
3. Kliknij zielony przycisk "Run" (lub Shift+F10).

## Sterowanie

| Klawisz | Akcja |
|---------|-------|
| **Strzałki** | Poruszanie kursorem po planszy / Nawigacja w menu |
| **SPACJA** / **ENTER** | Wybór pionka / Zatwierdzenie ruchu / Wybór w menu |
| **Q** | Pauza w trakcie gry |
| **ESC** | Wyjście z menu statystyk / Powrót |

## Zasady Gry
Zaimplementowano zasady zbliżone do warcabów międzynarodowych/polskich:
1. **Pionki**: Poruszają się o jedno pole do przodu po przekątnej. Mogą bić do przodu i do tyłu.
2. **Damki**: Pionek staje się damką po dotarciu do przeciwległej krawędzi planszy. Damka może poruszać się o dowolną liczbę pól po przekątnej i bić z dystansu.
3. **Bicie**: Jest obowiązkowe. Jeśli gracz ma możliwość bicia, gra nie pozwoli na wykonanie innego ruchu ("Wymuszenie bicia").
4. **Seria bić**: Jeśli po wykonaniu bicia ten sam pionek ma możliwość kolejnego bicia, gracz musi kontynuować sekwencję tym samym pionkiem.

## Struktura Projektu
- `main.c`: Główna pętla programu, inicjalizacja i obsługa sygnałów.
- `game_logic.c/h`: Logika gry, walidacja ruchów, zasady bicia i promocji.
- `bot.c/h`: Implementacja sztucznej inteligencji (AI).
- `ui.c/h`: Obsługa interfejsu użytkownika przy użyciu `ncurses` (rysowanie planszy, menu).
- `stats.c/h`: Obsługa zapisu i odczytu statystyk z pliku.
- `common.h`: Wspólne definicje struktur danych (GameState, Position, PieceType).

## Autorzy
Kamil Mróz, Maciej Maj