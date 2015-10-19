//#!/bin/cinit -lncurses

#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ncurses.h>

void level_load(unsigned int lvl);
void level_draw();

#define gSpeed 250
#define gNanoSpeed (gSpeed * 1000)

int input;

int win_width, win_height;
int tile_x, tile_y;

struct {
  int width, height;
  struct {
    int frame;
    char *frames;
    void (*collision)();
  } *tiles;
  
  int specialCount;
  struct {
    void (*tick)();
  } *specials;
  
  int portalsCount;
  struct {
    int x, y;
    int level;
    int destination;
  } *portals;
  
  int enemyCount;
  struct {
    int frame;
    char *frames;
    int x, y;
  } *enemies;
} gLevel = {
  0, 0, NULL, 0, NULL, 0, NULL, 0, NULL
};

struct {
  unsigned int width;
  unsigned int height;
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
  int from;
  int level;
  int destination;
} PORTALS[] = {
  { 0, 1, 1 },
  { 1, 2, 0 },
  { 1, 0, 0 },
  { 2, 1, 0 },
  { 2, 3, 0 },
  { 3, 2, 1 },
};
#define PORTAL_MAX sizeof(PORTALS) / sizeof(PORTALS[0])

int player_x, player_y;

enum CollisionType {
  COLLISION_NONE,
  COLLISION_WALL,
  COLLISION_PLAYER,
  COLLISION_ENEMY,
};
enum CollisionType CheckCollision(int x, int y, enum CollisionType ignore) {
  int tile = x + y * gLevel.width;
  if (ignore != COLLISION_WALL && gLevel.tiles[tile].frames[gLevel.tiles[tile].frame-1] != ' ') {
    return COLLISION_WALL;
  } else if (ignore != COLLISION_PLAYER && x == player_x && y == player_y) {
    return COLLISION_PLAYER;
  } else {
    // enemies, etc
  }
  return COLLISION_NONE;
}

int player_health = 0, player_health_max = 4;
char PlayerControllerInit() {
  player_x = tile_x;
  player_y = tile_y;
  if (player_health == 0) {
    player_health = player_health_max;
  }
  return ' ';
}
void PlayerControllerTick() {
  int healthbar_size = 2 + player_health_max;
  mvaddch(1, win_width - healthbar_size - 1, '[');
  for (int health = 1; health < player_health_max + 1; ++health) {
    if (health <= player_health) {
      addch('=');
    } else {
      addch(' ');
    }
  }
  addch(']');
  
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
  if (player_x == oldPlayerX && player_y == oldPlayerY) {
    // player didn't move
  } else if (gLevel.tiles[tile].collision != NULL) {
    gLevel.tiles[tile].collision(player_x, player_y);
  } else if (CheckCollision(player_x, player_y, COLLISION_PLAYER) != COLLISION_NONE) {
    player_x = oldPlayerX;
    player_y = oldPlayerY;
  }

  mvaddch(player_y, player_x, 'x');
}
void PlayerHealth(int delta) {
  player_health += delta;
  if (player_health <= 0) {
    // reset_game();
    level_load(0);
  }
}

int enemy = 0;
int EnemyInit() {
  enemy = gLevel.enemyCount++;
  gLevel.enemies = realloc(gLevel.enemies, sizeof(gLevel.enemies[0]) * gLevel.enemyCount);
  gLevel.enemies[enemy].x = tile_x;
  gLevel.enemies[enemy].y = tile_y;
  return enemy;
}

void EnemyTick() {
  mvaddch(gLevel.enemies[enemy].y, gLevel.enemies[enemy].x, gLevel.enemies[enemy].frames[gLevel.enemies[enemy].frame]);
  if (gLevel.enemies[enemy].frames[++gLevel.enemies[enemy].frame] == 0) {
    gLevel.enemies[enemy].frame = 0;
  }
  if (++enemy == gLevel.enemyCount) enemy = 0;
}

char EnemyChaserInit() {
  enemy = EnemyInit();
  gLevel.enemies[enemy].frame = 0;
  gLevel.enemies[enemy].frames = "Aa";
  return ' ';
}

void EnemyChaserTick() {
  // dumb chase
  int oldX = gLevel.enemies[enemy].x, oldY = gLevel.enemies[enemy].y;
  if (gLevel.enemies[enemy].x < player_x) {
    ++gLevel.enemies[enemy].x;
  } else if (gLevel.enemies[enemy].x > player_x) {
    --gLevel.enemies[enemy].x;
  } else if (gLevel.enemies[enemy].y < player_y) {
    ++gLevel.enemies[enemy].y;
  } else if (gLevel.enemies[enemy].y > player_y) {
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

void EnemyChaserCollision(int x, int y) {
  //
}

int portal;
char PortalInit() {
  gLevel.portals[portal].x = tile_x;
  gLevel.portals[portal].y = tile_y;
  ++portal;
  return 0;
}

void PortalCollision(int x, int y) {
  for (portal = 0; portal < gLevel.portalsCount; ++portal) {
    if (gLevel.portals[portal].x == x && gLevel.portals[portal].y == y) {
      int destination = gLevel.portals[portal].destination;
      level_load(gLevel.portals[portal].level);
      player_x = gLevel.portals[destination].x;
      player_y = gLevel.portals[destination].y;
      level_draw();
      return;
    }
  }
}

struct {
  char tile;
  char (*init)();
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
    int tile = 0;
    for (tile_y = 0; tile_y < gLevel.height; ++tile_y) {
      for (tile_x = 0; tile_x < gLevel.width; ++tile_x) {
        free(gLevel.tiles[tile].frames);
        ++tile;
      }
    }
  }
  if (full) {
    free(gLevel.tiles);
    free(gLevel.portals);
    free(gLevel.enemies);
  }
}

void level_load(unsigned int level) {
  level_unload(false);

  int tile = 0;
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
  
  for (tile_y = 0; tile_y < gLevel.height; ++tile_y) {
    for (tile_x = 0; tile_x < gLevel.width; ++tile_x) {
      gLevel.tiles[tile].frame = 0;
      gLevel.tiles[tile].frames = NULL;
      gLevel.tiles[tile].collision = NULL;
      
      char ch = LEVELS[level].tiles[tile];
      
      for (int i = 0; i < TILES_SPECIAL_MAX; ++i) {
        if (ch == TILES_SPECIAL[i].tile) {
          if (TILES_SPECIAL[i].init != NULL) {
            char init = TILES_SPECIAL[i].init();
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
  
  win_width = getmaxx(win);
  win_height = getmaxy(win);
  
  level_load(0);
  while (input != 'q') {
    gettimeofday(&start, NULL);
    input = getch();
    gettimeofday(&end, NULL);
    unsigned int diff = 1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    level_draw();
    //if (diff < gNanoSpeed) usleep(gNanoSpeed - diff);
  }
  level_unload(true);
  
  keypad(win, false);
  curs_set(1);
  echo();
  noraw();
  endwin();
  
  return 0;
}
