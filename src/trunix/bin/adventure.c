#!/bin/cinit

#include <ncurses.h>

#define HOOK_INIT_INT 0
#define HOOK_TICK_INT 1
void *HOOK_INIT = (void *) HOOK_INIT_INT;
void *HOOK_TICK = (void *) HOOK_TICK_INT;

int tile_x, tile_y;

int player_x, player_y;
void *PlayerController(void *action) {
  switch ((int) action) {
  case (HOOK_INIT_INT):
    player_x = tile_x;
    player_y = tile_y;
    return (void *) 'X';
    break;
  case (HOOK_TICK_INT):
    return (void *) 'x';
  default:
    return 0;
  }
}

struct {
  char ch;
  void *(*hook)(void *);
} TILES_SPECIAL[] = {
  { 'X', PlayerController }
};
#define TILES_SPECIAL_MAX sizeof(TILES_SPECIAL) / sizeof(TILES_SPECIAL[0])

struct {
  char *frames;
} TILES_ANIM[] = {
  "Aa",
  "Bb",
};
#define TILES_ANIM_MAX sizeof(TILES_ANIM) / sizeof(TILES_ANIM[0])

struct {
  unsigned int width;
  unsigned int height;
  char *tiles;
} LEVELS[] = {
  {
    10, 10,
    "+--------+" \
    "|       B|" \
    "| A      |" \
    "|        |" \
    "|        |" \
    "|    C   |" \
    "|        |" \
    "| X      |" \
    "|        |" \
    "+--------+",
  },
  {
    5, 10,
    "+---+" \
    "|   |" \
    "| A |" \
    "|   |" \
    "|   |" \
    "|   |" \
    "|   |" \
    "| X |" \
    "|   |" \
    "+---+",
  },
  {
    20, 10,
    "+------------------+" \
    "|       B          |" \
    "|           A      |" \
    "|                  |" \
    "|                  |" \
    "|              C   |" \
    "|                  |" \
    "|           X      |" \
    "|                  |" \
    "+------------------+",
  },
};
#define LEVEL_MAX sizeof(LEVELS) / sizeof(LEVELS[0])

void level_draw(unsigned int level, bool load) {
  move(0, 0);
  for (tile_y = 0; tile_y < LEVELS[level].height; ++tile_y) {
    for (tile_x = 0; tile_x < LEVELS[level].width; ++tile_x) {
      char ch = LEVELS[level].tiles[tile_x + (tile_y * LEVELS[level].width)];
      for (int i = 0; i < TILES_SPECIAL_MAX; ++i) {
        if (ch == TILES_SPECIAL[i].ch) {
          if (load) {
            ch = (int) TILES_SPECIAL[i].hook(HOOK_INIT);
          } else {
            ch = (int) TILES_SPECIAL[i].hook(HOOK_TICK);
          }
        }
      }
      addch(ch);
    }
    addch('\n');
  }
  refresh();
}

int main() {
  WINDOW *win = initscr();
  //int x = getmaxx(main);
  //int y = getmaxy(main);
  keypad(win, true);
  start_color();
  level_draw(0, true);
  for (int i = 0; i < 10; ++i) {
    // TODO: implement player control hook
    level_draw(0, false);
  }
  keypad(win, false);
  echo();
  nocbreak();
  endwin();
  return 0;
}
