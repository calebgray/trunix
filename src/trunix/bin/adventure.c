//#!/bin/cinit

#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

int input;

int tile_x, tile_y;

struct {
  unsigned int width;
  unsigned int height;
  char *tiles;
} LEVELS[] = {
  {
    20, 10,
    "+------------------+" \
    "|       B          |" \
    "[           A      |" \
    "|                  |" \
    "|                  |" \
    "|              C   |" \
    "|                  |" \
    "|           X      |" \
    "|                  |" \
    "+------------------+",
  },
  {
    10, 10,
    "+--------+" \
    "|       B|" \
    "| A      |" \
    "|        |" \
    "|        |" \
    "|    C   |" \
    "|        ]" \
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
    "[   ]" \
    "|   |" \
    "| X |" \
    "|   |" \
    "+---+",
  },
};
#define LEVEL_MAX sizeof(LEVELS) / sizeof(LEVELS[0])

int player_x, player_y;
void *PlayerControllerInit() {
  player_x = tile_x;
  player_y = tile_y;
  return (void *) ' ';
}
void PlayerControllerTick() {
  switch(input) {
  case KEY_LEFT:
    --player_x;
    break;
  case KEY_RIGHT:
    ++player_x;
    break;
  case KEY_UP:
    --player_y;
    break;
  case KEY_DOWN:
    ++player_y;
    break;
  }
  move(player_y, player_x);
  addch('x');
}

struct {
  char tile;
  void *(*init)();
  void (*tick)();
} TILES_SPECIAL[] = {
  {
    'X',
    PlayerControllerInit,
    PlayerControllerTick,
  }
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
  int width, height, specialCount;
  struct {
    int frame;
    char *frames;
  } *tiles;
  struct {
    void (*tick)();
  } *specials;
} gLevel = {
  0, 0, 0, NULL, NULL
};

void level_unload() {
  if (gLevel.tiles != NULL) free(gLevel.tiles);
  if (gLevel.specials != NULL) free(gLevel.specials);
}

void level_load(unsigned int level) {
  level_unload();

  int tile = 0;
  gLevel.width = LEVELS[level].width;
  gLevel.height = LEVELS[level].height;
  gLevel.tiles = malloc(sizeof(gLevel.tiles[0]) * gLevel.width * gLevel.height);
  gLevel.specials = malloc(sizeof(gLevel.specials[0]));
  gLevel.specialCount = 0;
  for (tile_y = 0; tile_y < LEVELS[level].height; ++tile_y) {
    for (tile_x = 0; tile_x < LEVELS[level].width; ++tile_x) {
      char ch = LEVELS[level].tiles[tile];
      
      for (int i = 0; i < TILES_SPECIAL_MAX; ++i) {
        if (ch == TILES_SPECIAL[i].tile) {
          ch = (int) TILES_SPECIAL[i].init();
          if (TILES_SPECIAL[i].tick != 0) {
            ++gLevel.specialCount;
            gLevel.specials = realloc(gLevel.specials, sizeof(gLevel.specials[0]) * gLevel.specialCount);
            gLevel.specials[gLevel.specialCount-1].tick = TILES_SPECIAL[i].tick;
          }
          break;
        }
      }
      
      gLevel.tiles[tile].frame = 0;
      gLevel.tiles[tile].frames = malloc(sizeof(char) * 2);
      gLevel.tiles[tile].frames[0] = ch;
      gLevel.tiles[tile].frames[1] = 0;
      
      for (int i = 0; i < TILES_ANIM_MAX; ++i) {
        if (ch == TILES_ANIM[i].frames[0]) {
          free(gLevel.tiles[tile].frames);
          gLevel.tiles[tile].frames = TILES_ANIM[i].frames;
        }
      }
      
      ++tile;
    }
  }
}

void level_draw() {
  int tile = 0;
  
  move(0, 0);
  for (tile_y = 0; tile_y < gLevel.height; ++tile_y) {
    for (tile_x = 0; tile_x < gLevel.width; ++tile_x) {
      if (gLevel.tiles[tile].frames[gLevel.tiles[tile].frame] == 0) {
        gLevel.tiles[tile].frame = 0;
      }
      char ch = gLevel.tiles[tile].frames[gLevel.tiles[tile].frame];
      ++gLevel.tiles[tile].frame;
      
      addch(ch);
      
      ++tile;
    }
    addch('\n');
  }
  
  for (tile = 0; tile < gLevel.specialCount; ++tile) {
    gLevel.specials[tile].tick();
  }
  
  refresh();
}

int main() {
  WINDOW *win = initscr();
  //int x = getmaxx(main);
  //int y = getmaxy(main);
  keypad(win, true);
  curs_set(0);
  noecho();
  cbreak();
  
  start_color();
  
  
  
  struct timeval timeout = { 0, 250000 };
  fd_set rfds_save;
  FD_ZERO(&rfds_save);
  FD_SET(fileno(stdin), &rfds_save);
  fd_set rfds = rfds_save;
  
  level_load(0);
  for (int i = 0; i < 20; ++i) {
    if (select(fileno(stdin)+1, &rfds, 0, 0, &timeout) > 0) {
      input = getch();
    }

    level_draw();
    usleep(250000);
  }
  level_unload();
  
  keypad(win, false);
  echo();
  nocbreak();
  endwin();
  return 0;
}
