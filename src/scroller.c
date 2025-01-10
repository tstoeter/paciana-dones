#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <allegro.h>

#include "scroller.h"

/* each characters x position of variable width bitmap font, last x position is the bitmap width to avoid out of bounds errors */
int char_xpos[] = {0, 5, 24, 42, 59, 77, 92, 108, 127, 144, 151, 165, 183, 199, 221, 240, 259, 276, 297, 314, 331, 348, 366, 383, 406, 422, 438, 454};

char scroller_text[] = "welcome  to  paciana  dones,  a  pacman/sokoban  inspired  and  indiana  jones  themed  maze  game,  finally  released  after  too  many  years  of  unsteady  development.";

/* convert a character to its corresponding index to use on the bitmap font */
int ctoi(char c)
{
    /* character out of bounds */
    if ((c < FIRST_CHAR) || (c > LAST_CHAR))
        return 0;

    return c - FIRST_CHAR;
}

int char_width(char c)
{
    int i = ctoi(toupper(c));
    return char_xpos[i+1]-char_xpos[i]-1;
}

int text_width(char *text)
{
    char *c = text;
    int width = 0;

    while (*c != '\0')
    {
        width += char_width(*c);
        c++;
    }

    return width;
}

/* draw a single specified character from the bitmap font */
void draw_char(BITMAP *font, BITMAP *dest, char c, int x, int y)
{
    int i = ctoi(toupper(c));
    masked_blit(font, dest, char_xpos[i], 0, x, y, char_width(c), FONT_HEIGHT);
}

void draw_rot_char(BITMAP *font, BITMAP *dest, char c, int x, int y, int phase)
{
    BITMAP *char_bmp;
    int i = ctoi(toupper(c));

    char_bmp = create_sub_bitmap(font, char_xpos[i], 0, char_width(c), FONT_HEIGHT);
    rotate_sprite(dest, char_bmp, x, y, 16*fixsin(itofix(phase)));

    destroy_bitmap(char_bmp);
}

/* print a text using the bitmap font */
void draw_text(BITMAP *font, BITMAP *dest, char *text, int x, int y)
{
    char *c = text;
    int cx = x;

    while ((*c != '\0') && (cx < dest->w))
    {
        if (*c != ' ')
            draw_char(font, dest, *c, cx, y);

        cx += char_width(*c) + 2;
        c++;
    }
}

/* print the text along a nice sine wave */
void draw_sine_text(BITMAP *font, BITMAP *dest, char *text, int x, int y, int phase)
{
    char *c = text;
    int cx = x, cy;

    /* end of string and bitmap was not reached */
    while ((*c != '\0') && (cx < dest->w))
    {



        /* only draw if character is available */
        if (*c != ' ')
        {
//   cy = y + 8*sin((cx + phase)*3.14/180);	/* the neat co/sine wave */
            cy = y + fixtoi( 8 * fixsin(itofix(cx+phase)) );
            draw_char(font, dest, *c, cx, cy);
        }

        cx += char_width(*c) + 1;
        c++;
    }
}

/* print a text using the bitmap font */
void draw_jump_text(BITMAP *font, BITMAP *dest, char *text, int x, int y, int phase)
{
    char *c = text;
    int cx = x, cy;

    while ((*c != '\0') && (cx < dest->w))
    {
        /* while the characters are outside the scope of our bitmap */
        if (cx < -2*FONT_HEIGHT)
        {
            cx += char_width(*c) + 1;	/* increase characters x position */
            c++;				/* immediately move to next character */
            continue;			/* and run next loop */
        }

        if (*c != ' ')
        {
            cy = y -abs(fixtoi( 32*cx/SCREEN_W * fixsin(itofix(cx+phase)) ));
            draw_char(font, dest, *c, cx, cy);
        }

        cx += char_width(*c) + 2;
        c++;
    }
}

/* print a text using the bitmap font */
void draw_rot_text(BITMAP *font, BITMAP *dest, char *text, int x, int y, int phase)
{
    char *c = text;
    int cx = x;

    while ((*c != '\0') && (cx < dest->w))
    {
        /* while the characters are outside the scope of our bitmap */
        if (cx < -2*FONT_HEIGHT)
        {
            cx += char_width(*c) + 1;	/* increase characters x position */
            c++;				/* immediately move to next character */
            continue;			/* and run next loop */
        }

        if (*c != ' ')
            draw_rot_char(font, dest, *c, cx, y, phase);

        cx += char_width(*c) + 2;
        c++;
    }
}

void wobble_stretch(BITMAP *src, BITMAP *dest, int x, int y, int phase)
{
    int w = src->w, h = src->h;
    float s = fixtof(fixsin(itofix(phase)));
    float c = fixtof(fixcos(itofix(phase+64)));

    w += (src->w * s)/8;
    h += (src->h * c)/8;

    x -= w/2;
    y -= h/2;

    masked_stretch_blit(src, dest, 0, 0, src->w, src->h, x, y, w, h);
}
