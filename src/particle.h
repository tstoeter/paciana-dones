/* particles.h */

#ifndef PARTICLES_H
#define PARTICLES_H

#include "map.h"
#include "game.h"

#define FIRE_SPOTS	16
#define FIRE_LIFE	6

#define EXPL_SPOTS	64
#define EXPL_LIFE	64

#define SPARKLES	16
#define SPARKLE_LIFE	6

typedef struct
{
    int life;
    int x, y;
    int dx, dy;
} PARTICLE;

struct
{
    int life;
    int points[8];
    direction dir;
} whip;

typedef struct
{
    float life;
    float x, y;
    float dx, dy;
} PARTICLE_F;

extern PARTICLE fire[FIRE_SPOTS];
extern PARTICLE_F explosion[EXPL_SPOTS];
extern PARTICLE sparkle[SPARKLES];

extern BITMAP *expl_bmp[EXPL_LIFE];
extern BITMAP *dust_bmp[EXPL_LIFE];

extern int fire_x, fire_y;
extern int sparkle_x, sparkle_y;

#define DANGER_ZONE 63
extern unsigned char explmap[MAP_SIZE_X][MAP_SIZE_Y];

void respawn_firespot(int i, int x, int y);
void init_fire();
void update_fire();
void draw_fire(BITMAP *bmp);
void respawn_explspot(int i, int x, int y);
void init_explosion();
void update_explosion();
void draw_explosion(BITMAP *bmp, int dust);
void respawn_sparkle(int i, int x, int y);
void init_sparkles();
void update_sparkles();
void draw_sparkles(BITMAP *bmp);
void init_whip();
void update_whip();
void draw_whip(BITMAP *bmp);

void create_explosion();
void create_dust();

void draw_explosions();

void fill_explmap(int x, int y);
int is_in_dangerzone(int x, int y, int w, int h);

void clear_explmap();

#endif
