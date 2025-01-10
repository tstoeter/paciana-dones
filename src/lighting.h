/* lighting.h */

#ifndef LIGHTING_H
#define LIGHTING_H

#define RAYDIUS 96

#define LIGHT_SIZE 224

extern BITMAP *lightmask;
extern BITMAP *flash_bmp;

int ray_collision(int x, int y);
void render_lightmask();
void draw_lighting();
int is_in_raydius(int x, int y, int w, int h);
void draw_flash();

#endif

