/* anim.h */

#ifndef ANIM_H
#define ANIM_H

#define ANIM_GAME_OVER      1
#define ANIM_NEXT_LEVEL     2
#define ANIM_GAME_COMPLETE  3
#define ANIM_PLAY_GAME      4
#define ANIM_BACK_MENU      5
#define ANIM_INTRO          6
#define ANIM_OUTRO          7

extern unsigned int anim;
extern unsigned int animation;
extern unsigned int anim_frame;
extern unsigned int init_anim;

extern unsigned int intro;

void pixelize_bitmap(BITMAP *bmp, int sq);
void update_anim();
void draw_anim();

#endif

