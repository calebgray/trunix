//#!/bin/cinit -lncurses

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ncurses.h>

#define gSpeed 250
#define gNanoSpeed (gSpeed * 1000)

int input;

int tile_x, tile_y;

struct {
  int width, height;
  int specialCount, portalsCount;
  struct {
    int frame;
    char *frames;
    void (*collision)();
  } *tiles;
  struct {
    void (*tick)();
  } *specials;
  struct {
    int x, y;
    int level;
    int destination;
  } *portals;
} gLevel = {
  0, 0, 0, NULL, NULL
};

struct {
  unsigned int width;
  unsigned int height;
  char *tiles;
} LEVELS[] = {
  {
    20, 10,
    "+------------------+" \
    "|       B          |" \
    "|           A      |" \
    "|                  |" \
    "|                  |" \
    "|              C   |" \
    "|                  |" \
    "|                X ]" \
    "|                  |" \
    "+------------------+",
  },
  {
    10, 10,
    "+--------+" \
    "|       B|" \
    "| A      ]" \
    "|        |" \
    "|        |" \
    "|    C   |" \
    "|        |" \
    "[ X      |" \
    "|        |" \
    "+--------+",
  },
  {
    5, 10,
    "+---+" \
    "|   |" \
    "[ A |" \
    "|   |" \
    "|   |" \
    "|   |" \
    "|   |" \
    "| X |" \
    "|   |" \
    "+---+",
  },
};
#define LEVEL_MAX sizeof(LEVELS) / sizeof(LEVELS[0])

struct {
  int from;
  int to;
  int destination;
} PORTALS[] = {
  { 0, 1, 0 },
  { 1, 0, 0 },
  { 1, 2, 0 },
  { 2, 1, 1 },
};
#define PORTAL_MAX sizeof(PORTALS) / sizeof(PORTALS[0])

int player_x, player_y;
void *PlayerControllerInit() {
  player_x = tile_x;
  player_y = tile_y;
  return (void *) ' ';
}
void PlayerControllerTick() {
  int oldPlayerX = player_x, oldPlayerY = player_y;
  
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
  
  // TODO: Add a collision hook to special tiles.
  // NOTE: frame-1 is the last rendered frame... So it's what we want to check.
  int tile = player_x + player_y * gLevel.width;
  if (gLevel.tiles[tile].collision != NULL) {
    gLevel.tiles[tile].collision(player_x, player_y);
  } else if (gLevel.tiles[tile].frames[gLevel.tiles[tile].frame-1] != ' ') {
    player_x = oldPlayerX;
    player_y = oldPlayerY;
  }

  mvaddch(player_y, player_x, 'x');
}

int level;
void *PortalInit() {
  mvprintw(22, 0, "%u", sizeof(gLevel.portals[0]));
  gLevel.portals = realloc(gLevel.portals, sizeof(gLevel.portals[0]) * (gLevel.portalsCount+1));
  gLevel.portals[gLevel.portalsCount].x = tile_x;
  gLevel.portals[gLevel.portalsCount].y = tile_y;
  gLevel.portals[gLevel.portalsCount].level = PORTALS[level].portal[gLevel.portalsCount].level;
  gLevel.portals[gLevel.portalsCount].destination = PORTALS[level].portal[gLevel.portalsCount].destination;
  ++gLevel.portalsCount;
  return NULL;
}

void PortalCollision(int x, int y) {
  for (int portal = 0; portal < gLevel.portalsCount; ++portal) {
    if (gLevel.portals[portal].x == x && gLevel.portals[portal].y == y) {
      int destination = gLevel.portals[portal].destination;
      level_load(gLevel.portals[portal].level);
      player_x = gLevel.portals[destination].x;
      player_y = gLevel.portals[destination].y;
      return;
    }
  }
}

struct {
  char tile;
  void *(*init)();
  void (*tick)();
  void (*collision)();
} TILES_SPECIAL[] = {
  {
    'X',
    PlayerControllerInit,
    PlayerControllerTick,
    NULL,
  },
  {
    ']',
    PortalInit,
    NULL,
    PortalCollision,
  },
};
#define TILES_SPECIAL_MAX sizeof(TILES_SPECIAL) / sizeof(TILES_SPECIAL[0])

struct {
  char *frames;
} TILES_ANIM[] = {
  "Aa",
  "Bb",
};
#define TILES_ANIM_MAX sizeof(TILES_ANIM) / sizeof(TILES_ANIM[0])

void level_unload() {
  if (gLevel.tiles != NULL) {
    int tile = 0;
    for (tile_y = 0; tile_y < gLevel.height; ++tile_y) {
      for (tile_x = 0; tile_x < gLevel.width; ++tile_x) {
        free(gLevel.tiles[tile].frames);
        ++tile;
      }
    }
    free(gLevel.tiles);
  }
  if (gLevel.specials != NULL) free(gLevel.specials);
  if (gLevel.portals != NULL) free(gLevel.portals);
}

void level_load(unsigned int lvl) {
  level_unload();

  level = lvl;

  int tile = 0;
  gLevel.width = LEVELS[level].width;
  gLevel.height = LEVELS[level].height;
  gLevel.tiles = malloc(sizeof(gLevel.tiles[0]) * gLevel.width * gLevel.height);
  gLevel.specialCount = 0;
  gLevel.specials = NULL;
  gLevel.portalsCount = 0;
  gLevel.portals = NULL;
  
  for (tile_y = 0; tile_y < gLevel.height; ++tile_y) {
    for (tile_x = 0; tile_x < gLevel.width; ++tile_x) {
      gLevel.tiles[tile].frame = 0;
      gLevel.tiles[tile].frames = NULL;
      gLevel.tiles[tile].collision = NULL;
      
      char ch = LEVELS[level].tiles[tile];
      
      for (int i = 0; i < TILES_SPECIAL_MAX; ++i) {
        if (ch == TILES_SPECIAL[i].tile) {
          if (TILES_SPECIAL[i].init != NULL) {
            char init = (int) TILES_SPECIAL[i].init();
            if (init != NULL) ch = init;
          }
          if (TILES_SPECIAL[i].tick != NULL) {
            ++gLevel.specialCount;
            gLevel.specials = realloc(gLevel.specials, sizeof(gLevel.specials[0]) * gLevel.specialCount);
            gLevel.specials[gLevel.specialCount-1].tick = TILES_SPECIAL[i].tick;
          }
          if (TILES_SPECIAL[i].collision != NULL) {
            gLevel.tiles[tile].collision = TILES_SPECIAL[i].collision;
          }
          break;
        }
      }
      
      for (int i = 0; i < TILES_ANIM_MAX; ++i) {
        if (ch == TILES_ANIM[i].frames[0]) {
          gLevel.tiles[tile].frames = malloc(sizeof(char) * (strlen(TILES_ANIM[i].frames) + 1));
          strcpy(gLevel.tiles[tile].frames, TILES_ANIM[i].frames);
          break;
        }
      }
      
      if (gLevel.tiles[tile].frames == NULL) {
        gLevel.tiles[tile].frames = malloc(sizeof(char) * 2);
        gLevel.tiles[tile].frames[0] = ch;
        gLevel.tiles[tile].frames[1] = 0;
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
  struct timeval start;
  struct timeval end;

  WINDOW *win = initscr();
  keypad(win, true);
  curs_set(0);
  raw();
  noecho();
  timeout(gSpeed);
  
  start_color();
  //
  
  //int x = getmaxx(win);
  //int y = getmaxy(win);
  
  level_load(0);
  while (input != 'q') {
    gettimeofday(&start, NULL);
    input = getch();
    gettimeofday(&end, NULL);
    unsigned int diff = 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    level_draw();
    //if (diff < gNanoSpeed) usleep(gNanoSpeed - diff);
  }
  level_unload();
  
  keypad(win, false);
  curs_set(1);
  echo();
  noraw();
  endwin();
  return 0;
}
