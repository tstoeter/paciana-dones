#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "masking.h"


void _mask_bitmap16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h)
{
    unsigned long *s, *d;
    int i, j;

    bmp_select(dst);

    for (j=0; j<h; j++)
    {
        s = (unsigned long *)(src->line[src_y + j] + (src_x << 1));
        d = (unsigned long *)(bmp_write_line(dst,  dst_y + j) + (dst_x << 1));

        for (i=w>>1; i; i--)
        {
            bmp_write32(d, *s & *d);

            s++;
            d++;
        }
    }

    bmp_unwrite_line(dst);
    release_bitmap(dst);
}

void mask_bitmap16(BITMAP *src, BITMAP *dst, int x, int y)
{
    int w, h;
    int src_x = 0, src_y = 0, dst_x = x, dst_y = y;

    w = src->w;
    h = src->h;

    /* clip the image */
    if (dst_x < 0)
    {
        w += dst_x;
        src_x -= dst_x;
        dst_x = 0;
    }

    if (dst_y < 0)
    {
        h += dst_y;
        src_y -= dst_y;
        dst_y = 0;
    }

    if (dst_x + w >= dst->w)
        w -= dst_x + w - dst->w;

    if (dst_y + h >= dst->h)
        h -= dst_y + h - dst->h;

    /* nothing to do? */
    if (w < 1 || h < 1)
        return;

// _mask_bitmap16(src, dst, src_x, src_y, dst_x, dst_y, w, h);
    masked_blit(src, dst, src_x, src_y, dst_x, dst_y, w, h);
}
