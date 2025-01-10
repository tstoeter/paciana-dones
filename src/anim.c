#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "anim.h"
#include "game.h"
#include "map.h"
#include "lost.h"
#include "blending.h"
#include "lighting.h"
#include "menu.h"
#include "scroller.h"
#include "particle.h"


unsigned int anim = TRUE;
unsigned int animation = ANIM_INTRO;
unsigned int anim_frame = 0;
unsigned int init_anim = 1;

unsigned int intro = 1;


void pixelize_bitmap(BITMAP *src, int sq)
{
    int i, j;
    unsigned int c;

    for (j=0; j<src->h>>1; j+=sq)
    {
        for (i=0; i<src->w>>1; i+=sq)
        {
            // I
            rectfill(src, SCREEN_W/2+i-(sq>>1), SCREEN_H/2-j+(sq>>1), SCREEN_W/2+i+(sq>>1), SCREEN_H/2-j-(sq>>1), getpixel(src, SCREEN_W/2+i, SCREEN_H/2-j));

            // II
            rectfill(src, SCREEN_W/2+i-(sq>>1), SCREEN_H/2+j-(sq>>1), SCREEN_W/2+i+(sq>>1), SCREEN_H/2+j+(sq>>1), getpixel(src, SCREEN_W/2+i, SCREEN_H/2+j));

            // III
            rectfill(src, SCREEN_W/2-i+(sq>>1), SCREEN_H/2+j-(sq>>1), SCREEN_W/2-i-(sq>>1), SCREEN_H/2+j+(sq>>1), getpixel(src, SCREEN_W/2-i, SCREEN_H/2+j));

            // IV
            rectfill(src, SCREEN_W/2-i+(sq>>1), SCREEN_H/2-j+(sq>>1), SCREEN_W/2-i-(sq>>1), SCREEN_H/2-j-(sq>>1), getpixel(src, SCREEN_W/2-i, SCREEN_H/2-j));
        }
    }
}

void update_anim()
{
    int k;

    anim_frame++;

    switch (animation)
    {
    case ANIM_PLAY_GAME:
    {
        if (anim_frame == 1)
        {
            clear_keybuf();

            al_stop_duh(dp);
            dp = al_start_duh(gameduh, 2, 0, volume[1]/5.0f, 4096, 22050);

            reset_game();

            level = 0;
            load_map_from_datafile(data[LEVEL1+level].dat);

            if (volume[4])
                dark = 0;

            update_map = TRUE;
            next_map = TRUE;
            next_level = FALSE;
        }
        else if (anim_frame == 255)
        {
            if (!keypressed())
                anim_frame--;
        }
        else if (anim_frame >= 384)
        {
            animation = ANIM_NEXT_LEVEL;
            anim_frame = 48;
            init_anim = 2;
        }
    }
    break;

    case ANIM_NEXT_LEVEL:
    {
        if (anim_frame >= 95)
        {
            anim = FALSE;
            animation = 0;
            anim_frame = 0;
            init_anim = 1;
        }
        else if (anim_frame == 47)
        {
            init_anim = 2;

            if (!keypressed())
                anim_frame--;
//            while (!keypressed());
            //              game_time = 0;
        }
        else if (anim_frame == 46)
            clear_keybuf();
    }
    break;

    case ANIM_GAME_OVER:
    {
        if (anim_frame <= 1)
        {
            init_anim = 1;
        }
        else if (anim_frame >= 47)
        {
            anim_frame = 47;
            if (key[KEY_ENTER] || key[KEY_Y] || key[KEY_SPACE])
            {
                reset_game();
                load_map_from_datafile(data[LEVEL1+level].dat);
                update_map = TRUE;

                if (volume[4])
                    dark = 0;

                animation = ANIM_NEXT_LEVEL;
                anim_frame = 48;
                init_anim = 2;
            }
            else if (key[KEY_ESC] || key[KEY_N])
            {
                _reset_menu();
                animation = ANIM_BACK_MENU;
                anim_frame = 0;
                init_anim = 1;
            }
        }
    }
    break;

    case ANIM_GAME_COMPLETE:
    {
        if (anim_frame >= 47)
        {
            clear_keybuf();
            do
            {
                k = readkey();
                k >>= 8;
            }
            while (k != KEY_ESC && k != KEY_ENTER);
            game_time = 0;

            /* go to the menu */
            //  anim = FALSE;
            animation = ANIM_BACK_MENU;
            anim_frame = 0;
            init_anim = 1;
            _reset_menu();
        }
    }
    break;

    case ANIM_BACK_MENU:
    {
        if (anim_frame == 1)
        {
            al_stop_duh(dp);
            dp = al_start_duh(menuduh, 2, 0, volume[1]/5.0f, 4096, 22050);
        }

        if (anim_frame >= 60)
        {
            if (!intro)
                _reset_menu();

            intro = 0;

            /* stop animation */
            anim = FALSE;
            animation = 0;
            anim_frame = 0;
            init_anim = 1;

            level = 0;

            /* then exit to menu */
            menu = TRUE;
        }
    }
    break;

    case ANIM_INTRO:
    {
        if (anim_frame == 32)
            play_sample(data[JINGLE].dat, 255, 127, 1000, 0);
        if (anim_frame >= 767)
        {
            animation = ANIM_BACK_MENU;
            anim_frame = 0;
            init_anim = 1;
        }
    }
    break;

    case ANIM_OUTRO:
    {
        if (anim_frame <= 1)
        {
            init_anim = 1;
        }
        else if (anim_frame >= 127)
        {
            animation = 0;
            done = TRUE;
        }
    }
    break;

    default:
        break;
    }
}


void draw_anim()
{
    switch (animation)
    {
    case ANIM_INTRO:
    {
        if (init_anim == 1)
        {
            blit(data[LOGO].dat, level_b, 0, 0, 0, 0, 640, 480);
            init_anim = 0;
        }

        if (anim_frame <= 255)
            clear_to_color(buffer, makecol(anim_frame, anim_frame, anim_frame));
        else if (anim_frame <= 511)
            clear_to_color(buffer, makecol(255, 255, 255));
        else if (anim_frame > 511)
            clear_to_color(buffer, makecol(767-anim_frame, 767-anim_frame, 767-anim_frame));

        mul_blend16(level_b, buffer, 0, 0);
    }
    break;

    case ANIM_PLAY_GAME:
    {
        if (init_anim == 1)
        {
            blit(data[GAME_START].dat, level_b, 0, 0, 0, 0, 640, 480);
            init_anim = 0;
        }

        if (anim_frame <= 255)
            clear_to_color(buffer, makecol(anim_frame, anim_frame, anim_frame));
        else if (anim_frame > 255)
            clear_to_color(buffer, makecol((383-anim_frame)*2, 2*(383-anim_frame), 2*(383-anim_frame)));

        mul_blend16(level_b, buffer, 0, 0);
    }
    break;

    case ANIM_NEXT_LEVEL:
    {
        /* next level animation initialisation */
        if (init_anim == 1)
        {
            blit(screen, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            init_anim = 0;
        }

        if (init_anim == 2)
        {
            draw_map(level_b, 0, 0, MAP_SIZE_X, MAP_SIZE_Y, 0);
            blit(level_b, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);
            draw_rle_sprite(buffer, data[PACIANA_FRONT].dat, player.x, player.y);

            render_lightmask();
            draw_lighting(255);
            blit(buffer, lightmask, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, 0, 0, LIGHT_SIZE, LIGHT_SIZE);

            init_anim = 0;
        }

        /* and finally the actual next level animation */
        if (anim_frame < 48)
        {
            pixelize_bitmap(buffer, anim_frame);

            //   if (!anim_play_game)
            trans_blend16(data[NEXT_LEVEL].dat, buffer, 0, 0);
        }
        else
        {
            clear_bitmap(buffer);
            blit(lightmask, buffer, 0, 0, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);
            pixelize_bitmap(buffer, 48-anim_frame/2);
        }
    }
    break;

    case ANIM_GAME_OVER:
    {
        if (init_anim == 1)
        {
            blit(screen, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            init_anim = 0;
        }

        pixelize_bitmap(buffer, anim_frame);
        trans_blend16(data[GAME_OVER].dat, buffer, 0, 0);
    }
    break;

    case ANIM_GAME_COMPLETE:
    {
        if (init_anim == 1)
        {
            blit(screen, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
            init_anim = 0;
        }

        pixelize_bitmap(buffer, anim_frame);
        trans_blend16(data[GAME_COMPLETE].dat, buffer, 0, 0);
    }
    break;

    case ANIM_BACK_MENU:
    {
        if (init_anim == 1)
        {
            draw_menu_bitmap(level_b);
            init_anim = 0;
        }

        clear_to_color(buffer, makecol(2*anim_frame, 2*anim_frame, 2*anim_frame));
        mul_blend16(level_b, buffer, 0, 0);
    }
    break;

    case ANIM_OUTRO:
    {
        if (init_anim == 1)
        {
            blit(data[MENU].dat, level_b, 0, 0, 0, 0, 640, 480);
            draw_text(data[FONT].dat, level_b, "thank you for playing", SCREEN_W/2-text_width("thank you for playing")/2, SCREEN_H/2-FONT_HEIGHT/2);
            init_anim = 0;
        }

        clear_to_color(buffer, makecol(255-2*anim_frame, 255-2*anim_frame, 255-2*anim_frame));
        mul_blend16(level_b, buffer, 0, 0);
    }
    break;

    default:
        break;
    }

    if (volume[3])
        textprintf_right_ex(buffer, font, SCREEN_W-10, 10, makecol(255, 255, 255), -1, "%d", fps);

    blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    frame_count++;
}
