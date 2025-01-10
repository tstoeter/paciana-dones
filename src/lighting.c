#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "lighting.h"
#include "blending.h"
#include "game.h"
#include "map.h"
#include "lost.h"
#include "masking.h"

BITMAP *lightmask;
BITMAP *flash_bmp;


/* check if a certain pixel lies inside a wall tile */
int ray_collision(int x, int y)
{
// int tile = map[(int)(x / (TILE_SIZE))][(int)(y / (TILE_SIZE))];
    int tile = map[x >> 5][y >> 5];

    if ((tile == 0) || (tile == DOOR) || (tile == KEY) || (tile == STONE) || (tile == DYNAMITE) || (tile == DYNAMITE_ACTIVE))
        return 0;

    return 1;
}

/* bresenham line drawing algorithm */
void ray_cast(int x2, int y2)
{
    int x=0, y=0, x_inc, y_inc;
    int i, d, dx, dy, dx2, dy2;
    int c=makecol(255,255,255);

    /* delta x and delta y */
    dx = x2;
    dy = y2;

    if (dx < 0)
    {
        x_inc = -1;
        dx = -dx;
    }
    else
        x_inc = 1;

    /* adjust delta y for negatively sloped lines */
    if (dy < 0)
    {
        y_inc = -1;
        dy = -dy;
    }
    else
        y_inc = 1;

    /* compute initial value
       of decision variable */
    d = (dy << 1) - dx;

    /* compute the increment variables by which
       the decision variable will be incremented */
    dx2 = dx << 1;
    dy2 = dy << 1;

    if (dx > dy)
    {
        d = dy2 - dx;

        for (i=0; i<=dx; i++)
        {
            /* as long as the ray has not hit a wall */
            if (ray_collision(x+player.x+TILE_SIZE/2, y+player.y+TILE_SIZE/2))
            {
                circlefill(lightmask, LIGHT_SIZE/2+x, LIGHT_SIZE/2+y, 8, c);
                return;
            }

            putpixel(lightmask, LIGHT_SIZE/2+x, LIGHT_SIZE/2+y, c);

            if (d >= 0)
            {
                d -= dx2;
                y += y_inc;
            }

            d += dy2;
            x += x_inc;
        }
    }
    else
    {
        d = dx2 - dy;

        for (i=0; i<=dy; i++)
        {
            if (ray_collision(x+player.x+TILE_SIZE/2, y+player.y+TILE_SIZE/2))
            {
                circlefill(lightmask, LIGHT_SIZE/2+x, LIGHT_SIZE/2+y, 8, c);
                return;
            }

            putpixel(lightmask, LIGHT_SIZE/2+x, LIGHT_SIZE/2+y, c);

            if (d >= 0)
            {
                d -= dy2;
                x += x_inc;
            }

            d += dx2;
            y += y_inc;
        }
    }
}

/* ray_cast from the player away in all direction along a square
   to generate a mask that contains realistically lit areas */
void render_lightmask()
{
    int i;

    clear(lightmask);

    for (i=-RAYDIUS; i<RAYDIUS; i++)
    {
        ray_cast(i, -RAYDIUS);
        ray_cast(i, RAYDIUS);

        ray_cast(-RAYDIUS, i);
        ray_cast(RAYDIUS, i);
    }
}

void draw_lighting(int a)
{
// masked_blit vs. draw_sprite
// masked_blit(lightmask, buffer, 0, 0, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);
// draw_sprite(buffer, lightmask, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);
// blit(lightmask, screen, 0, 0, 0, 0, LIGHT_SIZE, LIGHT_SIZE);
// mask_bitmap16(lightmask, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);
// mul_blend16(data[FLARE].dat, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);

// mul_blend16(data[FLARE].dat, lightmask, 0, 0);
// mul_blend16(lightmask, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);

    mul_blend16a(lightmask, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, 255);
    mul_blend16a(data[FLARE].dat, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, a);

// set_multiply_blender(0, 0, 0, 255);
// draw_trans_sprite(buffer, data[FLARE].dat, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);

}

int is_in_raydius(int x, int y, int w, int h)
{
    if ((x > player.x+TILE_SIZE/2-LIGHT_SIZE/2) &&
            (y > player.y+TILE_SIZE/2-LIGHT_SIZE/2) &&
            (x+w < player.x+TILE_SIZE/2+LIGHT_SIZE/2) &&
            (y+h < player.y+TILE_SIZE/2+LIGHT_SIZE/2))
        return 1;

    return 0;
}

void draw_flash()
{
//    blit(data[FLARE].dat, flash_bmp, 0, 0, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);
//    blit(lightmask, flash_bmp, 0, 0, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);
//    mul_blend16(data[FLARE].dat, flash_bmp, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);
//    mask_bitmap16(lightmask, flash_bmp, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);
//    add_blend16(data[FLARE].dat, flash_bmp, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);

//      mask_bitmap16(data[FLARE].dat, lightmask, 0, 0);
    mul_blend16(data[FLARE].dat, lightmask, 0, 0);
    add_blend16(lightmask, flash_bmp, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);


//    clear_to_color(lightmask, makecol(4*flash, 4*flash, 4*flash));
//    trans_blend16(lightmask, flash_bmp, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2);

    mul_blend16(flash_bmp, buffer, 0, 0);
}
