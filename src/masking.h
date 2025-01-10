/* masking.h */

#ifndef MASKING_H
#define MASKING_H

void _mask_bitmap16(BITMAP *src, BITMAP *dst, int src_x, int src_y, int dst_x, int dst_y, int w, int h);
void mask_bitmap16(BITMAP *src, BITMAP *dst, int x, int y);

#endif

