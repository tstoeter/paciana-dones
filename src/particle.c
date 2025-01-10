#include <allegro.h>
#include <string.h>
#include <math.h>
#include "particle.h"
#include "game.h"
#include "lost.h"
#include "map.h"
#include "blending.h"


PARTICLE fire[FIRE_SPOTS];
PARTICLE_F explosion[EXPL_SPOTS];
PARTICLE sparkle[SPARKLES];

BITMAP *expl_bmp[EXPL_LIFE];
BITMAP *dust_bmp[EXPL_LIFE];

int fire_x, fire_y;
int sparkle_x, sparkle_y;

unsigned char explmap[MAP_SIZE_X][MAP_SIZE_Y];

void clear_explmap()
{
    memset(&explmap, 0, sizeof(explmap));
}

void fill_explmap(int x, int y)
{
    if ((x < 0) || (x > MAP_SIZE_X-1))
        return;

    if ((y < 0) || (y > MAP_SIZE_Y-1))
        return;

    if (explmap[x][y])
        return;

    if (map[x][y] == 0)
    {
        explmap[x][y] = DANGER_ZONE;
        return;
    }

    if (map[x][y] == STONE)
    {
        map[x][y] = 0;
        colmap[x][y] = 0;
        explmap[x][y] = STONE;
        return;
    }

    if (map[x][y] == DYNAMITE || map[x][y] == DYNAMITE_ACTIVE)
    {
        map[x][y] = 0;
        colmap[x][y] = 0;
        explmap[x][y] = DYNAMITE;

        fill_explmap(x-1, y);
        fill_explmap(x+1, y);
        fill_explmap(x, y-1);
        fill_explmap(x, y+1);
    }
}

int is_in_dangerzone(int x, int y, int w, int h)
{
    if ((explmap[x/TILE_SIZE][y/TILE_SIZE] > STONE ||
            explmap[(x+w-1)/TILE_SIZE][y/TILE_SIZE] > STONE ||
            explmap[x/TILE_SIZE][(y+h-1)/TILE_SIZE] > STONE ||
            explmap[(x+w-1)/TILE_SIZE][(y+h-1)/TILE_SIZE] > STONE) && flash < 16)
        return 1;

    return 0;
}

void draw_explmap()
{
    int i, j;

    for (i=0; i<MAP_SIZE_X; i++)
        for (j=0; j<MAP_SIZE_Y; j++)
            if (explmap[i][j])
                rectfill(buffer, i*TILE_SIZE, j*TILE_SIZE, i*TILE_SIZE+4, j*TILE_SIZE+4, makecol(255, 255, 0));
}


void respawn_firespot(int i, int x, int y)
{
    fire[i].x = x + -1 + rand() % 3;
    fire[i].y = y + rand() % 2;

    fire[i].life = FIRE_LIFE/2 + rand() % FIRE_LIFE/2;
    fire[i].dx = 0;
    fire[i].dy = -1 - rand() % 3;
}

void init_fire()
{
    int i;

    for (i=0; i<FIRE_SPOTS; i++)
        respawn_firespot(i, player.x+fire_x, player.y+fire_y);
}

void update_fire()
{
    int i;

    for (i=0; i<FIRE_SPOTS; i++)
    {
        if (fire[i].life > 0)
        {
            fire[i].life--;
            fire[i].x += fire[i].dx;
            fire[i].y += fire[i].dy;
        }
        else
            respawn_firespot(i, player.x+fire_x, player.y+fire_y);
    }
}

void draw_fire(BITMAP *bmp)
{
    int i;

//	drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
//	set_add_blender(0, 0, 0, 127);

    for (i=0; i<FIRE_SPOTS; i++)
        //	circlefill(bmp, fire[i].x, fire[i].y, 1, makecol(255-fire[i].life<<3, 63-fire[i].life, 0));
        add_blend16(data[SPARKLE].dat, bmp, fire[i].x, fire[i].y);

//    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}

void respawn_explspot(int i, int x, int y)
{
    explosion[i].x = x + -2 + rand() % 5;
    explosion[i].y = y + -2 + rand() % 5;

    explosion[i].life = EXPL_LIFE/4 + rand() % EXPL_LIFE/2;
    explosion[i].dx = -5 + rand() % 9;
    explosion[i].dy = -5 + rand() % 9;
}

void init_explosion()
{
    int i;

    for (i=0; i<EXPL_SPOTS; i++)
        respawn_explspot(i, 32-8, 32-8);
}

void update_explosion()
{
    int i;

    for (i=0; i<EXPL_SPOTS; i++)
    {
        if (explosion[i].life > 0)
        {
            explosion[i].life--;
            explosion[i].x += explosion[i].dx/10;
            explosion[i].y += explosion[i].dy/10;
        }
    }
}

void draw_explosion(BITMAP *bmp, int dust)
{
    int i;

//	drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

    for (i=0; i<EXPL_SPOTS; i++)
    {
//		set_add_blender(0, 0, 0, explosion[i].life<<4);
//		circlefill(bmp, explosion[i].x, explosion[i].y, 4+abs(explosion[i].dx), makecol(255-explosion[i].life<<3, 63-explosion[i].life, 0));
        if (explosion[i].life > 0)
        {
            if (dust == 0)
                add_blend16(data[FIRESPOT].dat, bmp, (int)(explosion[i].x), (int)(explosion[i].y));
            else
                mul_blend16(data[DUSTSPOT].dat, bmp, (int)(explosion[i].x), (int)(explosion[i].y));
        }
    }
}

void respawn_sparkle(int i, int x, int y)
{
    sparkle[i].x = x;
    sparkle[i].y = y;

    sparkle[i].life = SPARKLE_LIFE/2 + (rand() % SPARKLE_LIFE/2);
    sparkle[i].dx = -1 + rand() % 3;
    sparkle[i].dy = -1 - rand() % 3;
}

void init_sparkles()
{
    int i;

    for (i=0; i<SPARKLES; i++)
        respawn_sparkle(i, sparkle_x, sparkle_y);
}

void update_sparkles()
{
    int i;

    for (i=0; i<SPARKLES; i++)
    {
        if (sparkle[i].life > 0)
        {
            sparkle[i].life--;
            sparkle[i].x += sparkle[i].dx;
            sparkle[i].y += sparkle[i].dy;

            //if (sparkle[i].dy <= 0)
            sparkle[i].dy++;
        }
        else
            respawn_sparkle(i, sparkle_x, sparkle_y);
    }
}

void draw_sparkles(BITMAP *bmp)
{
    int i;

    for (i=0; i<SPARKLES; i++)
        putpixel(bmp, sparkle[i].x, sparkle[i].y, makecol(255, 255, 0));
}

void create_explosion()
{
    int i;

    init_explosion();

    for (i=0; i<EXPL_LIFE; i++)
    {
        expl_bmp[i] = create_bitmap(64, 64);
        clear(expl_bmp[i]);
        draw_explosion(expl_bmp[i], 0);
        update_explosion();
    }
}

void create_dust()
{
    int i;

    init_explosion();

    for (i=0; i<EXPL_LIFE; i++)
    {
        dust_bmp[i] = create_bitmap(64, 64);
        clear_to_color(dust_bmp[i], makecol(255,255,255));
        draw_explosion(dust_bmp[i], 1);
        update_explosion();
    }
}

void draw_explosions()
{
    int i, j;

    for (j=0; j<MAP_SIZE_Y; j++)
    {
        for (i=0; i<MAP_SIZE_X; i++)
        {
            if (explmap[i][j] == DYNAMITE)
                add_blend16(expl_bmp[flash-1], buffer, i*TILE_SIZE-16, j*TILE_SIZE-16);
            else if (explmap[i][j] == STONE)
                mul_blend16(dust_bmp[flash-1], buffer, i*TILE_SIZE-16, j*TILE_SIZE-16);
        }
    }
}

void init_whip()
{
    whip.life = 64;

    whip.points[0] = TILE_SIZE/2;
    whip.points[1] = TILE_SIZE/2;
    whip.points[6] = TILE_SIZE/2;
    whip.points[7] = TILE_SIZE/2;

    whip.points[3] = rand()%TILE_SIZE;
    whip.points[4] = rand()%TILE_SIZE;
    whip.points[3] = TILE_SIZE/4 + rand()%TILE_SIZE/2;
    whip.points[4] = TILE_SIZE/4 + rand()%TILE_SIZE/2;

    if (player.dir != NO_DIR)
        whip.dir = player.dir;
    else
        whip.dir = player.old_dir;

    int dist = TILE_SIZE+TILE_SIZE/2;

    if (whip.dir == NORTH)
        whip.points[7] = -dist+TILE_SIZE/2;
    else if (whip.dir == EAST)
        whip.points[6] = +dist+TILE_SIZE/2;
    else if (whip.dir == SOUTH)
        whip.points[7] = +dist+TILE_SIZE/2;
    else if (whip.dir == WEST)
        whip.points[6] = -dist+TILE_SIZE/2;
}

void update_whip()
{
    if (whip.life > 0)
        whip.life--;
}

void draw_whip(BITMAP *bmp)
{
    if (whip.life > 0)
    {
        int pts[8];

        for (int i=0; i<4; i++)
        {
            pts[2*i] = player.x + whip.points[2*i];
            pts[2*i+1] = player.y + whip.points[2*i+1];
        }

        int a = whip.life-48;
        if (a < 0) a = 0;

        int offset = 10*sin(a/5.0)-5;

        pts[2] += offset;
        pts[3] += offset;
        pts[4] += offset;
        pts[5] += offset;

        pts[6] = a*pts[0]/64 + (64-a)*pts[6]/64 + offset;
        pts[7] = a*pts[1]/64 + (64-a)*pts[7]/64 + offset;

        int col = -(whip.life*whip.life)/8 + 11*whip.life;
        spline(bmp, pts, makecol(col,col,col));
    }
}

