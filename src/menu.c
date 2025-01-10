#include <stdio.h>
#include <allegro.h>

#include "menu.h"
#include "game.h"
#include "anim.h"
#include "pyramid.h"
#include "scroller.h"
#include "lost.h"
#include "map.h"

unsigned int menu = FALSE;
unsigned int menu_item;
unsigned int sub_menu = 0;
int sub_menu_item;
char sub_menu_list[8][128];
int volume_max[] = {5, 5, 1, 1, 1, 1};
int phase = 0;
int text_w;
float scroller_x = 650;
int i, j, k=0;

void _init_menu()
{
    init_pyramid();
    text_w = text_width(scroller_text);
    memset(&sub_menu_list, 0, sizeof(sub_menu_list));
}

void _reset_menu()
{
    scroller_x = 650;
    stop_sample(data[GHOST_SPL].dat);
    stop_sample(data[FIRE_CRACKLE].dat);
//    play_midi(data[INDY_THEME].dat, 1);
}

/* menu control routine */
void _update_menu()
{
// printf("%d\n", volume[0]);

    if (sub_menu)
    {
        if (key[KEY_ESC])
        {
            sub_menu = 0;
            play_sample(data[PICKUP_SPL].dat, 255, 127, 1000, 0);
        }

        if (key[KEY_UP])
        {
            if (k != KEY_UP)
            {
                k = KEY_UP;
                sub_menu_item--;
                play_sample(data[PYRAMID_ENTER].dat, 255, 127, 800, 0);
            }
        }
        else if (k == KEY_UP)
            k = 0;

        if (key[KEY_DOWN])
        {
            if (k != KEY_DOWN)
            {
                k = KEY_DOWN;
                sub_menu_item++;
                play_sample(data[PYRAMID_ENTER].dat, 255, 127, 800, 0);
            }
        }
        else if (k == KEY_DOWN)
            k = 0;

        if (sub_menu == MENU_SETUP)
        {
            sub_menu_item = (sub_menu_item + 6) % 6;

            if (key[KEY_LEFT])
            {
                if (k != KEY_LEFT)
                {
                    k = KEY_LEFT;
                    volume[sub_menu_item]--;

                    if (sub_menu_item < 2)
                    {
                        //set_volume(51*volume[0], 51*volume[1]);
                        al_duh_set_volume(dp, volume[1]/5.0f);
                    }

                    play_sample(data[PYRAMID_ENTER].dat, vol(255), 127, 1000, 0);
                }
            }
            else if (k == KEY_LEFT)
                k = 0;

            if (key[KEY_RIGHT])
            {
                if (k != KEY_RIGHT)
                {
                    k = KEY_RIGHT;
                    volume[sub_menu_item]++;

                    if (sub_menu_item < 2)
                    {
                        //set_volume(51*volume[0], 51*volume[1]);
                        al_duh_set_volume(dp, volume[1]/5.0f);
                    }

                    play_sample(data[PYRAMID_ENTER].dat, vol(255), 127, 1000, 0);
                }
            }
            else if (k == KEY_RIGHT)
                k = 0;

            if (volume[sub_menu_item] < 0)
                volume[sub_menu_item] = 0;
            else if (volume[sub_menu_item] > volume_max[sub_menu_item])
                volume[sub_menu_item] = volume_max[sub_menu_item];

#ifndef __DJGPP__
            if (volume[5] && is_windowed_mode())
                set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0);
            else if (!volume[5] && !is_windowed_mode())
                set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
#endif
        }
        else if (sub_menu == MENU_PLAY_MAP)
        {
            if (sub_menu_item < 0)
                sub_menu_item = 0;
            else if (sub_menu_item >= num_mapfiles)
                sub_menu_item = num_mapfiles-1;

            memset(&sub_menu_list, 0, sizeof(sub_menu_list));

            int i0 = 0, i1 = num_mapfiles;
            if (num_mapfiles > 5)
            {
                i0 = sub_menu_item - 2;
                if (i0 < 0)
                    i0 = 0;
                else if (i0 > num_mapfiles-5)
                    i0 = num_mapfiles-5;

                i1 = i0 + 5;
                if (i1 > num_mapfiles)
                    i1 = num_mapfiles;
            }

            for (int i=i0; i<i1; i++)
                strncpy(sub_menu_list[i-i0], map_files[i], strlen(map_files[i])-4);

            if (key[KEY_ENTER])
            {
                if (k == 0)
                {
                    k = KEY_ENTER;
                    play_sample(data[PYRAMID_ENTER].dat, 255, 127, 1000, 0);
                    char filename[1024];
                    sprintf(filename, "maps/%s", map_files[sub_menu_item]);
                    play_map(filename);
                }
            }
            else if (k == KEY_ENTER)
                k = 0;
        }
    }
    else
    {
        if (angle%64 == 0)
        {
            if (key[KEY_ENTER])
            {
                // menu_item = ((256-angle)%256)/64;
                menu_item = ((256-angle)%256)>>6;
                k = KEY_ENTER;

                switch (menu_item)
                {
                case MENU_PLAY_GAME:
                    play_sample(data[PYRAMID_ENTER].dat, 255, 127, 1000, 0);
                    anim = TRUE;
                    animation = ANIM_PLAY_GAME;

                    menu = FALSE;
                    break;

                case MENU_EXIT_GAME:
                    stop_midi();
                    play_sample(data[PYRAMID_ENTER].dat, 255, 127, 1000, 0);
//    done = TRUE;
                    menu = FALSE;
                    anim = TRUE;
                    animation = ANIM_OUTRO;
                    break;

                case MENU_SETUP:
                    play_sample(data[PYRAMID_ENTER].dat, 255, 127, 1000, 0);
                    sub_menu = MENU_SETUP;
                    sub_menu_item = 0;
                    memset(&sub_menu_list, 0, sizeof(sub_menu_list));
                    strcpy(sub_menu_list[0], "sound");
                    strcpy(sub_menu_list[1], "music");
                    strcpy(sub_menu_list[2], "limit fps");
                    strcpy(sub_menu_list[3], "show fps");
                    strcpy(sub_menu_list[4], "lights on");
                    strcpy(sub_menu_list[5], "fullscrn.");
                    break;

                case MENU_PLAY_MAP:
                    play_sample(data[PYRAMID_ENTER].dat, 255, 127, 1000, 0);
                    sub_menu = MENU_PLAY_MAP;
                    sub_menu_item = 0;
                    memset(&sub_menu_list, 0, sizeof(sub_menu_list));
                    find_maps();
                    for (int i=0; i<5 && i<num_mapfiles; i++)
                        strncpy(sub_menu_list[i], map_files[i], strlen(map_files[i])-4);
                    //strcpy(sub_menu_list[0], "map pack");
                    //strcpy(sub_menu_list[1], "random map");
                    break;
                }
            }
        }

        update_pyramid();
    }

    phase = (phase+2)%256;

    if (scroller_x < -1.2*text_w)
        scroller_x = SCREEN_W;
    else
        scroller_x -= 0.6;
}

void draw_menu()
{
    char str[24];

    blit(data[MENU].dat, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    draw_jump_text(data[FONT].dat, buffer, scroller_text, scroller_x, SCREEN_H-2*FONT_HEIGHT, phase);

    wobble_stretch(data[ARROW_LEFT].dat, buffer, 130+30, 350, phase);
    wobble_stretch(data[ARROW_RIGHT].dat, buffer, 344-30, 350, phase);

    draw_pyramid(buffer);

    if (sub_menu)
    {
        for (i=0; i<6; i++)
        {
            if (sub_menu_list[i][0] == '\0')
                break;

            if (sub_menu == MENU_PLAY_MAP)
            {
                if ((num_mapfiles > 5 && sub_menu_item > 1 && sub_menu_item < num_mapfiles-2 && i == 2) ||
                        (num_mapfiles > 5 && sub_menu_item >= num_mapfiles-2 && i == (5-(num_mapfiles-sub_menu_item))) ||
                        (num_mapfiles > 5 && sub_menu_item <= 1 && i == sub_menu_item) ||
                        (num_mapfiles <= 5 && i == sub_menu_item))
                    //draw_rot_text(data[FONT].dat, buffer, sub_menu_list[i], 360, 200+i*FONT_HEIGHT, phase);
                    textout_ex(buffer, data[NCEN].dat, sub_menu_list[i], 380, 200+i*22, makecol(255,255,0), -1);
                else
                    //draw_text(data[FONT].dat, buffer, sub_menu_list[i], 360, 200+i*FONT_HEIGHT);
                    textout_ex(buffer, data[NCEN].dat, sub_menu_list[i], 380, 200+i*22, makecol(200,150,0), -1);
            }
            else
            {
                if (i == sub_menu_item)
                    draw_rot_text(data[FONT].dat, buffer, sub_menu_list[i], 360, 200+i*FONT_HEIGHT, phase);
                else
                    draw_text(data[FONT].dat, buffer, sub_menu_list[i], 360, 200+i*FONT_HEIGHT);
            }

            if (sub_menu == MENU_SETUP)
            {
                if (i == 0 || i == 1)
                {
                    for (j=0; j<volume[i]; j++)
                        draw_sprite(buffer, data[SPEAKER].dat, 520+j*16, 200+i*(FONT_HEIGHT+1));

                    for (j; j<5; j++)
                        draw_sprite(buffer, data[SPEAKER_OFF].dat, 520+j*16, 200+i*(FONT_HEIGHT+1));
                }
                else if (i >= 2 || i <= 5)
                {
                    if (i == 4)
                        volume[i] ? sprintf(str, "always") : sprintf(str, "individ.");
                    else
                        volume[i] ? sprintf(str, "yes") : sprintf(str, "no");


                    if (i == sub_menu_item)
                        draw_rot_text(data[FONT].dat, buffer, str, 520, 200+i*FONT_HEIGHT, phase);
                    else
                        draw_text(data[FONT].dat, buffer, str, 520, 200+i*FONT_HEIGHT);
                }
            }
        }
    }

    if (volume[3])
        textprintf_right_ex(buffer, font, SCREEN_W-10, 10, makecol(255, 255, 255), -1, "%d", fps);

    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    frame_count++;
}

void draw_menu_bitmap(BITMAP *bmp)
{
    blit(data[MENU].dat, bmp, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    draw_pyramid(bmp);
}

