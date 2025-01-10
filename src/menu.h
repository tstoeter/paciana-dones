/* menu.h */

#ifndef MENU_H
#define MENU_H

#define MENU_PLAY_GAME 0
#define MENU_SETUP  1
#define MENU_EXIT_GAME 2
#define MENU_PLAY_MAP   3

extern unsigned int menu;
extern unsigned int menu_item;

void _init_menu();
void _reset_menu();
void _update_menu();
void draw_menu();
void draw_menu_bitmap(BITMAP *bmp);

#endif
