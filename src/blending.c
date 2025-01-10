#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "blending.h"


unsigned long _mul_blend_pixel16(unsigned long src, unsigned long dst)
{
    if ((src == 0) || (dst == 0))
        return 0;

    unsigned char src_r = (src >> 0) & 0x1F;
    unsigned char src_g = (src >> 5) & 0x3F;
    unsigned char src_b = (src >> 11) & 0x1F;

    unsigned char dst_r = (dst >> 0) & 0x1F;
    unsigned char dst_g = (dst >> 5) & 0x3F;
    unsigned char dst_b = (dst >> 11) & 0x1F;

    dst_r = (src_r * dst_r) >> 5;
    dst_g = (src_g * dst_g) >> 6;
    dst_b = (src_b * dst_b) >> 5;

    return (dst_r << 0) | (dst_g << 5) | (dst_b << 11);
}

void _mul_blend16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h)
{
    unsigned short *s, *d;
    int i, j;

    bmp_select(dst);

    for (j=0; j<h; j++)
    {
        s = (unsigned short *)(src->line[src_y + j] + (src_x << 1));
        d = (unsigned short *)(bmp_write_line(dst,  dst_y + j) + (dst_x << 1));

        for (i=w; i; i--)
        {
            bmp_write16(d, _mul_blend_pixel16(*s, *d));
            s++;
            d++;
        }
    }

    bmp_unwrite_line(dst);
    release_bitmap(dst);
}

void mul_blend16(BITMAP *src, BITMAP *dst, int x, int y)
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

//_mul_blend16(src, dst, src_x, src_y, dst_x, dst_y, w, h);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

    set_multiply_blender(255, 255, 255, 255);
    draw_trans_sprite(dst, src, x, y);

    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}

void mul_blend16a(BITMAP *src, BITMAP *dst, int x, int y, int a)
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

//_mul_blend16(src, dst, src_x, src_y, dst_x, dst_y, w, h);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

    set_multiply_blender(255, 255, 255, a);
    draw_trans_sprite(dst, src, x, y);

    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}


unsigned long _trans_blend_pixel16(unsigned long src, unsigned long dst)
{
    if ((src == 0) && (dst == 0))
        return 0;

    if (src == 0xFFFF)
        return dst;

    unsigned char dst_r = (dst >> 0) & 0x1F;
    unsigned char dst_g = (dst >> 5) & 0x3F;
    unsigned char dst_b = (dst >> 11) & 0x1F;

    if (src == 0) // factor .75
        return (((dst_r>>1)+(dst_r>>2)) << 0) | (((dst_g>>1)+(dst_g>>2)) << 5) | (((dst_b>>1)+(dst_b>>2)) << 11);

    unsigned char src_r = (src >> 0) & 0x1F;
    unsigned char src_g = (src >> 5) & 0x3F;
    unsigned char src_b = (src >> 11) & 0x1F;

    dst_r = (src_r + dst_r) >> 1;
    dst_g = (src_g + dst_g) >> 1;
    dst_b = (src_b + dst_b) >> 1;

    return (dst_r << 0) | (dst_g << 5) | (dst_b << 11);
}

void _trans_blend16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h)
{
    unsigned short *s, *d;
    int i, j;

    bmp_select(dst);

    for (j=0; j<h; j++)
    {
        s = (unsigned short *)(src->line[src_y + j] + (src_x << 1));
        d = (unsigned short *)(bmp_write_line(dst,  dst_y + j) + (dst_x << 1));

        for (i=w; i; i--)
        {
            bmp_write16(d, _trans_blend_pixel16(*s, *d));
            s++;
            d++;
        }
    }

    bmp_unwrite_line(dst);
    release_bitmap(dst);
}

void trans_blend16(BITMAP *src, BITMAP *dst, int x, int y)
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

//_trans_blend16(src, dst, src_x, src_y, dst_x, dst_y, w, h);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

    set_trans_blender(0, 0, 0, 127);
    draw_trans_sprite(dst, src, x, y);

    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}

unsigned long _add_blend_pixel16(unsigned long src, unsigned long dst)
{
    if ((src == 0) || (dst == 0))
        return src + dst;

    unsigned char src_r = (src >> 0) & 0x1F;
    unsigned char src_g = (src >> 5) & 0x3F;
    unsigned char src_b = (src >> 11) & 0x1F;

    unsigned char dst_r = (dst >> 0) & 0x1F;
    unsigned char dst_g = (dst >> 5) & 0x3F;
    unsigned char dst_b = (dst >> 11) & 0x1F;

    dst_r = (src_r + dst_r);
    dst_g = (src_g + dst_g);
    dst_b = (src_b + dst_b);

    dst_r = (dst_r > 0x1F) ? 0x1F : dst_r;
    dst_g = (dst_g > 0x3F) ? 0x3F : dst_g;
    dst_b = (dst_b > 0x1F) ? 0x1F : dst_b;

    return (dst_r << 0) | (dst_g << 5) | (dst_b << 11);
}

void _add_blend16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h)
{
    unsigned short *s, *d;
    int i, j;

    bmp_select(dst);

    for (j=0; j<h; j++)
    {
        s = (unsigned short *)(src->line[src_y + j] + (src_x << 1));
        d = (unsigned short *)(bmp_write_line(dst,  dst_y + j) + (dst_x << 1));

        for (i=w; i; i--)
        {
            bmp_write16(d, _add_blend_pixel16(*s, *d));
            s++;
            d++;
        }
    }

    bmp_unwrite_line(dst);
    release_bitmap(dst);
}

void add_blend16(BITMAP *src, BITMAP *dst, int x, int y)
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

//_add_blend16(src, dst, src_x, src_y, dst_x, dst_y, w, h);
    drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);

    set_add_blender(255, 255, 255, 255);
    draw_trans_sprite(dst, src, x, y);

    drawing_mode(DRAW_MODE_SOLID, NULL, 0, 0);
}
