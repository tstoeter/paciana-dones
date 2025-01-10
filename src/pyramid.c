#include <stdio.h>
#include <allegro.h>

#include "pyramid.h"
#include "game.h"
#include "lost.h"

V3D vertex[NUM_VERTICES] =
{
    /* vertex array containing the vertices of the pyramid
       x, y, z - 3d coordinates of vertex
       u, v    - texture mapping coordinates, set later
       c       - actually colour value, but abused to define texture map
            x        y        z  u  v  c
    */
    {-32<<16,  32<<16, -32<<16, 0, 0, 0}, // A 0
    { 32<<16,  32<<16, -32<<16, 0, 0, 1}, // B 1
    { 32<<16,  32<<16,  32<<16, 0, 0, 2}, // C 2
    {-32<<16,  32<<16,  32<<16, 0, 0, 3}, // D 3
    {      0, -32<<16,       0, 0, 0, 0}  // E 4
};

/* triangle array containing indices for vertex array
   that build up the triangle */
T3D pyramid[NUM_FACES] =
{
    {0, 4, 1},
    {1, 4, 2},
    {2, 4, 3},
    {3, 4, 0}
};

/* output vertices for drawing to screen */
V3D vout[NUM_VERTICES];
int angle = 0, yrot = 0;


void init_pyramid()
{
    MATRIX m;
    int i;

    set_projection_viewport(0, 0, SCREEN_W, SCREEN_H);

    /* create the transformation matrix */
    get_translation_matrix(&m, itofix(-25), 0, 0);

    /* and multiply it with all vertices */
    for (i=0; i<NUM_VERTICES; i++)
        apply_matrix(&m, vertex[i].x, vertex[i].y, vertex[i].z, &vertex[i].x, &vertex[i].y, &vertex[i].z);
}

void update_pyramid()
{
    MATRIX m;
    int i;

// poll_keyboard();

    /* only if one shape of the pyramid is facing you
       allow the pyramid to be rotated */
    if (angle%64 == 0)
    {
        angle %= 256; /* keep angle between 0..255 */
        yrot = 0; /* stop rotation */

        /* until user forces a new one */
        if (key[KEY_RIGHT])
        {
            yrot = -1, angle--;
            play_sample(data[STONE3_SPL].dat, vol(127), 127, 1000, 0);
        }
        else if (key[KEY_LEFT])
        {
            yrot = 1, angle++;
            play_sample(data[STONE3_SPL].dat, vol(127), 127, 1000, 0);
        }
    }
    else
        angle+=yrot; /* otherwise go on with currently running rotation */

    /* create the transformation matrix */
    get_translation_matrix(&m, itofix(25), 0, 0);

    /* and multiply it with all vertices */
    for (i=0; i<NUM_VERTICES; i++)
        apply_matrix(&m, vertex[i].x, vertex[i].y, vertex[i].z, &vertex[i].x, &vertex[i].y, &vertex[i].z);

    /* create the transformation matrix */
    get_y_rotate_matrix(&m, itofix(yrot));

    /* and multiply it with all vertices */
    for (i=0; i<NUM_VERTICES; i++)
        apply_matrix(&m, vertex[i].x, vertex[i].y, vertex[i].z, &vertex[i].x, &vertex[i].y, &vertex[i].z);

    /* create the transformation matrix */
    get_translation_matrix(&m, itofix(-25), 0, 0);

    /* and multiply it with all vertices */
    for (i=0; i<NUM_VERTICES; i++)
        apply_matrix(&m, vertex[i].x, vertex[i].y, vertex[i].z, &vertex[i].x, &vertex[i].y, &vertex[i].z);
}

void draw_pyramid(BITMAP *bmp)
{
    BITMAP *tex;
    int i, c;

    /* copy vertex list to output list to work with */
    memcpy(&vout, &vertex, NUM_VERTICES*sizeof(V3D));

    for (i=0; i<NUM_VERTICES; i++)
    {
        /* project vertices to screen coordinates */
        persp_project(vertex[i].x, vertex[i].y, vertex[i].z+itofix(128), &vout[i].x, &vout[i].y);
        vout[i].z += itofix(128);
    }

    /* as the pyramid has so few shapes z-sorting is not required
       and back-face culling is sufficent to prevent near triangles to be overdrawn by far triangles,
       the far triangles in the pyramid are culled */

    /* loop through pyramid ;) and render visible shapes */
    for (i=0; i<NUM_FACES; i++)
    {
        /* cull back-faces */
        if (polygon_z_normal(&vout[pyramid[i].v1], &vout[pyramid[i].v2], &vout[pyramid[i].v3]) >= 0)
        {
            /* set texture map */
            tex = data[TEX_PLAYGAME + vertex[pyramid[i].v1].c].dat;

            /* set texture coordinates */
            vout[pyramid[i].v1].u =          0;
            vout[pyramid[i].v1].v = tex->h<<16;
            vout[pyramid[i].v2].u = tex->w<<15;
            vout[pyramid[i].v2].v =          0;
            vout[pyramid[i].v3].u = tex->w<<16;
            vout[pyramid[i].v3].v = tex->h<<16;

            /* compute z-flat shading, very simple and looks the same as usual flat shading on the pyramid */
            c = fixtoi(vout[pyramid[i].v1].z + vout[pyramid[i].v3].z);
            vout[pyramid[i].v1].c = vout[pyramid[i].v2].c = vout[pyramid[i].v3].c = c;

            drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
            set_trans_blender(0, 0, 0, 127);

            /* draw the triangle lit and perspectiv-correctly textured */
            triangle3d(bmp, POLYTYPE_PTEX_LIT, tex, &vout[pyramid[i].v1], &vout[pyramid[i].v2], &vout[pyramid[i].v3]);

            solid_mode();
        }
    }
}

