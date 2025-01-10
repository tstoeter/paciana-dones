/* game.h */

#ifndef GAME_H
#define GAME_H

#include "map.h"
#include <aldumb.h>

extern DATAFILE *data;

extern BITMAP *buffer;

extern volatile int game_time, frame_time;
extern volatile int fps, frame_count;

/* define directions */
typedef enum
{
    NO_DIR,
    NORTH,
    EAST,
    SOUTH,
    WEST
} direction;

struct
{
    int x, y;
//    int old_x, old_y;
    direction dir, old_dir;
    int keys;
    int update;
    int push;
    int dynamite;
    int dead;
    RLE_SPRITE *sprite;
} player;

#define MAX_DYNAMITE    1
#define DYNAMITE_TIME   300

struct enemy_t
{
    int x, y;
    int old_x, old_y;
    direction dir, old_dir;
    int dead;
    int pause;
    RLE_SPRITE *sprite;
} enemy;

struct enemy_t scarab[8];

extern unsigned int done;
extern unsigned int game_over;
extern unsigned int pause_game;
extern unsigned long int enemy_move;
extern unsigned int flash;
extern unsigned int plmap;
extern int dark;

extern int volume[6];
extern unsigned char colmap[MAP_SIZE_X][MAP_SIZE_Y];

extern AL_DUH_PLAYER *dp;
extern DUH *menuduh, *gameduh;
int vol(int v);

void game_timer();
void fps_proc();
void init_game();
void update_game();
void draw_game();
void play_game();
void exit_game();
void reset_game();

void move_enemy();
void move_scarabs();
void move_player();
int  collision();

void play_map(char *filename);

#endif
