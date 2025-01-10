/* blending.h */

#ifndef BLENDING_H
#define BLENDING_H

unsigned long _mul_blend_pixel16(unsigned long src, unsigned long dst);
void _mul_blend16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h);
void mul_blend16(BITMAP *src, BITMAP *dst, int x, int y);
void mul_blend16a(BITMAP *src, BITMAP *dst, int x, int y, int a);
unsigned long _trans_blend_pixel16(unsigned long src, unsigned long dst);
void _trans_blend16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h);
void trans_blend16(BITMAP *src, BITMAP *dst, int x, int y);
unsigned long _add_blend_pixel16(unsigned long src, unsigned long dst);
void _add_blend16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h);
void add_blend16(BITMAP *src, BITMAP *dst, int x, int y);

#endif
