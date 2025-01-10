/* scroller.h */

#ifndef SCROLLER_H
#define SCROLLER_H

#define FONT_HEIGHT 21
#define FIRST_CHAR  '@'
#define LAST_CHAR   'Z'

extern int char_xpos[];

extern char scroller_text[];

int text_width(char *text);
void draw_char(BITMAP *font, BITMAP *dest, char c, int x, int y);
void draw_text(BITMAP *font, BITMAP *dest, char *text, int x, int y);
void draw_sine_text(BITMAP *font, BITMAP *dest, char *text, int x, int y, int phase);
void draw_jump_text(BITMAP *font, BITMAP *dest, char *text, int x, int y, int phase);
void draw_rot_text(BITMAP *font, BITMAP *dest, char *text, int x, int y, int phase);
void wobble_stretch(BITMAP *src, BITMAP *dest, int x, int y, int phase);

#endif
