/* pyramid.h */

#ifndef PYRAMID_H
#define PYRAMID_H

#define NUM_VERTICES 5
#define NUM_FACES    4

/* triangle data structure */
typedef struct
{
    /* indices for vertex list */
    int v1, v2, v3;
} T3D;

extern V3D vertex[NUM_VERTICES];
extern T3D pyramid[NUM_FACES];

extern V3D vout[NUM_VERTICES];
extern int angle, yrot;

void init_pyramid();
void update_pyramid();
void draw_pyramid(BITMAP *bmp);

#endif
