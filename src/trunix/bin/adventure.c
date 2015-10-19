//#!/bin/cinit -lncurses

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ncurses.h>

void level_load(unsigned int lvl);
void level_draw();

#define gSpeed 200
#define gNanoSpeed (gSpeed * 1000)

struct {
  unsigned int width, height;
  unsigned int input;
  /*struct timeval lastTime;
  unsigned int elapsedTime;*/
} gGame = {
  100, 100,
  0,
};

struct {
  unsigned int width, height;
  struct {
    unsigned int frame;
    char *frames;
    void (*collision)();
  } *tiles;
  
  unsigned int portalsCount;
  struct {
    unsigned int x, y;
    unsigned int level;
    unsigned int destination;
  } *portals;
  
  unsigned int specialCount;
  struct {
    void (*tick)();
  } *specials;
  
  unsigned int enemyCount;
  struct {
    unsigned int frame;
    char *frames;
    unsigned int x, y;
  } *enemies;
} gLevel = {
  0, 0, NULL,
  0, NULL,
  0, NULL,
  0, NULL,
};

struct {
  unsigned int width, height;
  char *tiles;
} LEVELS[] = {
  {
    60, 10,
    "+----------------------------------------------------------+" \
    "|             |                                            |" \
    "|           A      X                                       |" \
    "|             |                                            |" \
    "|        -----|                                            |" \
    "|                                                          |" \
    "|                                                          |" \
    "|                                                          ]" \
    "|                                                          |" \
    "+----------------------------------------------------------+",
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
    "+-v-+",
  },
  {
    20, 10,
    "+-------^----------+" \
    "|                  |" \
    "|                  |" \
    "|                  |" \
    "|                  |" \
    "|                  |" \
    "|                  |" \
    "|                X |" \
    "|                  |" \
    "+------------------+",
  },
};
#define LEVEL_MAX sizeof(LEVELS) / sizeof(LEVELS[0])

struct {
  unsigned int from, level, destination;
} PORTALS[] = {
  { 0, 1, 1 },
  { 1, 2, 0 },
  { 1, 0, 0 },
  { 2, 1, 0 },
  { 2, 3, 0 },
  { 3, 2, 1 },
};
#define PORTAL_MAX sizeof(PORTALS) / sizeof(PORTALS[0])

struct {
  unsigned int x, y;
  int health;
  unsigned int healthMax;
} gPlayer = {
  0, 0,
  0, 4,
};

enum CollisionType {
  COLLISION_NONE,
  COLLISION_WALL,
  COLLISION_PLAYER,
  COLLISION_ENEMY,
};
enum CollisionType CheckCollision(unsigned int x, unsigned int y, enum CollisionType ignore) {
  unsigned int tile = x + y * gLevel.width;
  if (ignore != COLLISION_WALL && gLevel.tiles[tile].frames[gLevel.tiles[tile].frame-1] != ' ') {
    return COLLISION_WALL;
  } else if (ignore != COLLISION_PLAYER && x == gPlayer.x && y == gPlayer.y) {
    return COLLISION_PLAYER;
  } else {
    // enemies, etc
  }
  return COLLISION_NONE;
}

char PlayerControllerInit(unsigned int x, unsigned int y) {
  gPlayer.x = x;
  gPlayer.y = y;
  if (gPlayer.health <= 0) {
    gPlayer.health = gPlayer.healthMax;
  }
  return ' ';
}
void PlayerControllerTick() {
  unsigned int healthbar_size = 2 + gPlayer.healthMax;
  mvaddch(1, gGame.width - healthbar_size - 1, '[');
  for (unsigned int health = 1; health < gPlayer.healthMax + 1; ++health) {
    if (health <= gPlayer.health) {
      addch('=');
    } else {
      addch(' ');
    }
  }
  addch(']');
  
  unsigned int oldPlayerX = gPlayer.x, oldPlayerY = gPlayer.y;
  
  switch(gGame.input) {
  case KEY_LEFT:
    --gPlayer.x;
    break;
  case KEY_RIGHT:
    ++gPlayer.x;
    break;
  case KEY_UP:
    --gPlayer.y;
    break;
  case KEY_DOWN:
    ++gPlayer.y;
    break;
  }
  
  // TODO: Add a collision hook to special tiles.
  // NOTE: frame-1 is the last rendered frame... So it's what we want to check.
  int tile = gPlayer.x + gPlayer.y * gLevel.width;
  if (gPlayer.x == oldPlayerX && gPlayer.y == oldPlayerY) {
    // player didn't move
  } else if (gLevel.tiles[tile].collision != NULL) {
    gLevel.tiles[tile].collision(gPlayer.x, gPlayer.y);
  } else if (CheckCollision(gPlayer.x, gPlayer.y, COLLISION_PLAYER) != COLLISION_NONE) {
    gPlayer.x = oldPlayerX;
    gPlayer.y = oldPlayerY;
  }

  mvaddch(gPlayer.y, gPlayer.x, 'x');
}
void PlayerHealth(int delta) {
  gPlayer.health += delta;
  if (gPlayer.health <= 0) {
    // reset_game();
    level_load(0);
  }
}

unsigned int enemy = 0;
unsigned int EnemyInit(unsigned int x, unsigned int y) {
  enemy = gLevel.enemyCount++;
  gLevel.enemies = realloc(gLevel.enemies, sizeof(gLevel.enemies[0]) * gLevel.enemyCount);
  gLevel.enemies[enemy].x = x;
  gLevel.enemies[enemy].y = y;
  return enemy;
}

void EnemyTick() {
  mvaddch(gLevel.enemies[enemy].y, gLevel.enemies[enemy].x, gLevel.enemies[enemy].frames[gLevel.enemies[enemy].frame]);
  if (gLevel.enemies[enemy].frames[++gLevel.enemies[enemy].frame] == 0) {
    gLevel.enemies[enemy].frame = 0;
  }
  if (++enemy == gLevel.enemyCount) enemy = 0;
}

char EnemyChaserInit(unsigned int x, unsigned int y) {
  enemy = EnemyInit(x, y);
  gLevel.enemies[enemy].frame = 0;
  gLevel.enemies[enemy].frames = "Aa";
  return ' ';
}

void EnemyChaserTick() {
  // dumb chase
  unsigned int oldX = gLevel.enemies[enemy].x, oldY = gLevel.enemies[enemy].y;
  if (gLevel.enemies[enemy].x < gPlayer.x) {
    ++gLevel.enemies[enemy].x;
  } else if (gLevel.enemies[enemy].x > gPlayer.x) {
    --gLevel.enemies[enemy].x;
  } else if (gLevel.enemies[enemy].y < gPlayer.y) {
    ++gLevel.enemies[enemy].y;
  } else if (gLevel.enemies[enemy].y > gPlayer.y) {
    --gLevel.enemies[enemy].y;
  }
  
  enum CollisionType collision = CheckCollision(gLevel.enemies[enemy].x, gLevel.enemies[enemy].y, COLLISION_NONE);
  if (collision != COLLISION_NONE) {
    if (collision == COLLISION_PLAYER) {
      PlayerHealth(-1);
    }
    gLevel.enemies[enemy].x = oldX;
    gLevel.enemies[enemy].y = oldY;
  }
  
  EnemyTick();
}

void EnemyChaserCollision(unsigned int x, unsigned int y) {
  //
}

unsigned int portal;
char PortalInit(unsigned int x, unsigned int y) {
  gLevel.portals[portal].x = x;
  gLevel.portals[portal].y = y;
  ++portal;
  return 0;
}

void PortalCollision(unsigned int x, unsigned int y) {
  for (portal = 0; portal < gLevel.portalsCount; ++portal) {
    if (gLevel.portals[portal].x == x && gLevel.portals[portal].y == y) {
      unsigned int destination = gLevel.portals[portal].destination;
      level_load(gLevel.portals[portal].level);
      gPlayer.x = gLevel.portals[destination].x;
      gPlayer.y = gLevel.portals[destination].y;
      // TODO: Without setting (input = 0), input gets processed twice.
      //       I kind of like that, but it may need to be changed later.
      level_draw();
      return;
    }
  }
}

struct {
  char tile;
  char (*init)(unsigned int x, unsigned int y);
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
    'A',
    EnemyChaserInit,
    EnemyChaserTick,
    EnemyChaserCollision,
  },
  {
    '[',
    PortalInit,
    NULL,
    PortalCollision,
  },
  {
    ']',
    PortalInit,
    NULL,
    PortalCollision,
  },
  {
    'v',
    PortalInit,
    NULL,
    PortalCollision,
  },
  {
    '^',
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

void level_unload(bool full) {
  if (gLevel.tiles != NULL) {
    unsigned int tile = 0;
    for (unsigned int tile_y = 0; tile_y < gLevel.height; ++tile_y) {
      for (unsigned int tile_x = 0; tile_x < gLevel.width; ++tile_x) {
        free(gLevel.tiles[tile].frames);
        ++tile;
      }
    }
  }
  if (full) {
    free(gLevel.tiles);
    free(gLevel.portals);
    free(gLevel.specials);
    free(gLevel.enemies);
  }
}

void level_load(unsigned int level) {
  level_unload(false);

  gLevel.width = LEVELS[level].width;
  gLevel.height = LEVELS[level].height;
  gLevel.tiles = realloc(gLevel.tiles, sizeof(gLevel.tiles[0]) * gLevel.width * gLevel.height);
  gLevel.specialCount = 0;

  gLevel.portalsCount = 0;
  for (portal = 0; portal < PORTAL_MAX; ++portal) {
    if (PORTALS[portal].from == level) {
      gLevel.portals = realloc(gLevel.portals, sizeof(gLevel.portals[0]) * (gLevel.portalsCount+1));
      gLevel.portals[gLevel.portalsCount].level = PORTALS[portal].level;
      gLevel.portals[gLevel.portalsCount].destination = PORTALS[portal].destination;
      ++gLevel.portalsCount;
    }
  }
  portal = 0;
  
  gLevel.enemyCount = 0;
  enemy = 0;
  
  unsigned int tile = 0;
  for (unsigned int tile_y = 0; tile_y < gLevel.height; ++tile_y) {
    for (unsigned int tile_x = 0; tile_x < gLevel.width; ++tile_x) {
      gLevel.tiles[tile].frame = 0;
      gLevel.tiles[tile].frames = NULL;
      gLevel.tiles[tile].collision = NULL;
      
      char ch = LEVELS[level].tiles[tile];
      
      for (unsigned int i = 0; i < TILES_SPECIAL_MAX; ++i) {
        if (ch == TILES_SPECIAL[i].tile) {
          if (TILES_SPECIAL[i].init != NULL) {
            char init = TILES_SPECIAL[i].init(tile_x, tile_y);
            if (init != 0) ch = init;
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
      
      for (unsigned int i = 0; i < TILES_ANIM_MAX; ++i) {
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
  unsigned int tile = 0;
  
  move(0, 0);
  for (unsigned int tile_y = 0; tile_y < gLevel.height; ++tile_y) {
    for (unsigned int tile_x = 0; tile_x < gLevel.width; ++tile_x) {
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
  
  //mvprintw(gGame.height - 2, 1, "%u", gGame.elapsedTime);

  refresh();
}

int main() {
  //struct timeval lastTime;

  WINDOW *win = initscr();
  keypad(win, true);
  curs_set(0);
  raw();
  noecho();
  timeout(gSpeed);
  
  start_color();
  //
  
  gGame.width = getmaxx(win);
  gGame.height = getmaxy(win);
  
  level_load(0);
  //gettimeofday(&gGame.lastTime, NULL);
  while ((gGame.input = getch()) != 'q') {
    level_draw();
    /*gettimeofday(&lastTime, NULL);
    gGame.elapsedTime = 1000 * (gGame.lastTime.tv_sec - lastTime.tv_sec) + (gGame.lastTime.tv_usec - lastTime.tv_usec);
    gGame.lastTime = lastTime;*/
  }
  level_unload(true);
  
  keypad(win, false);
  curs_set(1);
  echo();
  noraw();
  delwin(win);
  endwin();

  return 0;
}
