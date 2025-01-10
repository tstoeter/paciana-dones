#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <aldumb.h>
#include <math.h>

#include "game.h"
#include "map.h"
#include "lighting.h"
#include "menu.h"
#include "lost.h"
#include "anim.h"
#include "particle.h"
#include "blending.h"


DATAFILE *data;
BITMAP *buffer;
BITMAP *level_b;
BITMAP *flame_b;

volatile int game_time = 0;
volatile int frame_time = 0;
volatile int fps = 0, frame_count = 0;

unsigned int done = FALSE;
unsigned int game_over = FALSE;
unsigned int pause_game = FALSE;
unsigned long int enemy_move = FALSE;

unsigned int dynamite_count = 0;
unsigned int dynamite_x, dynamite_y;
unsigned int flash = 0;
unsigned int flash_intensity;

unsigned int stone_x, stone_y;
unsigned int stone_dead;

unsigned char colmap[MAP_SIZE_X][MAP_SIZE_Y];

int dark = 0;
unsigned int plmap = FALSE;

void draw_colmap();

/* sound and music volume, limit fps and show fps */
int volume[6] = {4, 2, 1, 0, 0, 0};

int l = 0;
int secol = 0;

/* timer callback for controlling the game speed */
void game_timer()
{
    game_time++;
    frame_time = 1;
}
END_OF_FUNCTION(game_timer);

/* timer callback to compute the farmes per second */
void fps_proc()
{
    fps = frame_count;
    frame_count = 0;
}
END_OF_FUNCTION(fps_proc);

DUH *menuduh, *gameduh;
AL_DUH_PLAYER *dp;

int vol(int v)
{
    int nv = volume[0]/5.0f * v;
    if (nv > 255) nv = 255;
    return nv;
}

/* init routine */
void init_game()
{
    allegro_init();
    atexit(&dumb_exit);
    dumb_register_stdfiles();

    install_keyboard();

    set_color_depth(16);

#ifdef __DJGPP__
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 640, 480, 0, 0) != 0)
#else
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0) != 0)
#endif
    {
        allegro_exit();
        printf("Error setting graphics mode");
        exit(-1);
    }

    install_timer();

    srand(time(NULL));

    LOCK_VARIABLE(game_time);
    LOCK_VARIABLE(frame_time);
    LOCK_FUNCTION(game_timer);

    LOCK_VARIABLE(fps);
    LOCK_FUNCTION(fps_proc);

    install_int_ex(game_timer, BPS_TO_TIMER(100));
    install_int_ex(fps_proc, BPS_TO_TIMER(1));

//	dumb_register_dat_xm(DAT_ID('X','M',' ',' '));
    data = load_datafile("lost.dat");

    if (!data)
    {
        allegro_exit();
        printf("Error loading data file");
        exit(-1);
    }

    if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) != 0)
    {
        allegro_exit();
        printf("Error initialising sound");
        exit(-1);
    }

    /* 255 / 5 * volume (in 5 discrete values) */
// set_volume(51*volume[0], 51*volume[1]);

    menuduh = dumb_load_xm_quick("music/menu.xm");
    gameduh = dumb_load_xm_quick("music/game.xm");
//    menuduh = data[MENU_MUS].dat;
//    gameduh = data[GAME_MUS].dat;

    buffer = create_bitmap(640, 480);
    level_b = create_bitmap(640, 480);
    lightmask = create_bitmap(LIGHT_SIZE, LIGHT_SIZE);
    flash_bmp = create_bitmap(640, 480);

    create_explosion();
    create_dust();

    clear(screen);
    clear(buffer);
    clear(level_b);

    _init_menu();
    _reset_menu();
}

void exit_game()
{
    remove_int(game_timer);
    remove_int(fps_proc);

    unload_datafile(data);

    destroy_bitmap(buffer);
    destroy_bitmap(level_b);
    destroy_bitmap(lightmask);

    unload_duh(menuduh);
    unload_duh(gameduh);
    dumb_exit();
    allegro_exit();
}

void update_game_sound()
{
    /* only if there's an enemy present */
    if (enemy.x == 0 && enemy.y == 0)
        return;

    int pan = 127 + (((enemy.x-player.x) * 256) / (2*SCREEN_W));
    double r = sqrt((player.x-enemy.x)*(player.x-enemy.x) + (player.y-enemy.y)*(player.y-enemy.y));
    int vol = 32768 / r - 48;

    adjust_sample(data[GHOST_SPL].dat, vol, pan, 300, 1);
}

void play_map(char *filename)
{
    reset_game();

    menu = FALSE;
    anim = FALSE;

    level = 0;
    load_map(filename, 0);
    flash = 0;

    if (volume[4])
        dark = 0;

    update_map = TRUE;
    next_map = FALSE;
    next_level = FALSE;
}

void enemy_whip_logic()
{
    if (enemy.pause > 0)
    {
        enemy.pause--;

        if (enemy.pause % 2)
        {
            direction d = 1 + (enemy.pause/2 % 4);

            switch (d)
            {
            case NORTH:
                enemy.sprite = data[MUMMY_BACK].dat;
                break;

            case EAST:
                enemy.sprite = data[MUMMY_RIGHT].dat;
                break;

            case SOUTH:
                enemy.sprite = data[MUMMY_FRONT].dat;
                break;

            case WEST:
                enemy.sprite = data[MUMMY_LEFT].dat;
                break;
            }
        }
    }

    if (enemy.dir == 0)
    {
        switch (enemy.dir)
        {
        case NORTH:
            enemy.sprite = data[MUMMY_BACK].dat;
            break;

        case EAST:
            enemy.sprite = data[MUMMY_RIGHT].dat;
            break;

        case SOUTH:
            enemy.sprite = data[MUMMY_FRONT].dat;
            break;

        case WEST:
            enemy.sprite = data[MUMMY_LEFT].dat;
            break;
        }
    }

    int wx = whip.points[6]+player.x;
    int wy = whip.points[7]+player.y;

    int minx = MIN(wx, player.x+TILE_SIZE/2);
    int miny = MIN(wy, player.y+TILE_SIZE/2);
    int maxx = MAX(wx, player.x+TILE_SIZE/2);
    int maxy = MAX(wy, player.y+TILE_SIZE/2);

    int r = 2;
    if (whip.life > 32 && enemy.dead == 0 &&
            !(minx-r > enemy.x+TILE_SIZE || maxx+r < enemy.x ||
              miny-r > enemy.y+TILE_SIZE || maxy+r < enemy.y))
    {
        enemy.pause = 128;
        play_sample(data[GRUNT_SPL].dat, vol(255), 127, 1000, 0);
    }

    for (int i=0; i<8; i++)
    {
        if (whip.life > 32 && scarab[i].dead == 0 &&
                !(minx-r > scarab[i].x+TILE_SIZE || maxx+r < scarab[i].x ||
                  miny-r > scarab[i].y+TILE_SIZE || maxy+r < scarab[i].y))
        {
            scarab[i].dead = 1;
            play_sample(data[SQUISH_SPL].dat, vol(127), 127, 1000, 0);
        }
    }
}

void update_game()
{
    al_poll_duh(dp);

// poll_keyboard();
    update_game_sound();
    update_fire();
    update_whip();

    enemy_whip_logic();

    /* exit to menu if ESC has been pressed */
    if (key[KEY_ESC])
    {
        if (plmap)
            done = TRUE;

        anim = TRUE;
        animation = ANIM_GAME_OVER;
        anim_frame = 1;
    }
    else if ((key[KEY_ALT] || key[KEY_ALTGR]) && player.dynamite && !dynamite_count)
    {
        dynamite_count = 1;
        player.dynamite--;
        play_sample(data[FUSE_SPL].dat, vol(127), 127, 3000, 0);
//     init_explosion();

        dynamite_x = player.x/TILE_SIZE;
        dynamite_y = player.y/TILE_SIZE;

        map[dynamite_x][dynamite_y] = DYNAMITE_ACTIVE;
        update_map = TRUE;

//     fill_explmap(sparkle_x+1, sparkle_y);
//     fill_explmap(sparkle_x, player.y/TILE_SIZE-1);
//     fill_explmap(player.x/TILE_SIZE, player.y/TILE_SIZE+1);

        sparkle_x = dynamite_x*TILE_SIZE + 17;
        sparkle_y = dynamite_y*TILE_SIZE + 3;

        init_sparkles();
    }

    if ((key[KEY_LCONTROL] || key[KEY_RCONTROL]) && whip.life <= 0)
    {
        init_whip();
        play_sample(data[WHIP_SPL].dat, vol(127), 127, 1700, 0);
    }

    if (dynamite_count > DYNAMITE_TIME)
    {
        dynamite_count = 0;
        flash = 1;
        fill_explmap(dynamite_x, dynamite_y);
        update_map = TRUE;
        stop_sample(data[FUSE_SPL].dat);
        play_sample(data[EXPLOSION_SPL].dat, vol(127), 127, 1000, 0);
    }
    else if (dynamite_count > 0)
    {
        update_sparkles();
        dynamite_count++;
    }

    if (flash > 255)
    {
        flash = 0;
        clear_explmap();
    }
    else if (flash > 0)
    {
//   update_explosion();

        flash++;
    }
    /* and pause the game if P has been pressed */
    else if (key[KEY_P]) /* why not game_time here?? */
    {
        while (key[KEY_P]); /* prevent switching between paused and not paused when pressing P a bit longer */

        if (pause_game == TRUE)
            pause_game = FALSE;
        else
            pause_game = TRUE;
    }

    if (next_level)
    {
        if (plmap)
            done = TRUE;

        if (level < NUM_LEVELS-1) // to be checked!
        {
            reset_game();
            level++;
            load_map_from_datafile(data[LEVEL1+level].dat);
            if (volume[4]) dark = 0;
            update_map = TRUE;
            next_map = TRUE;

            if (level > 0) // do next level animation
            {
                anim = TRUE;
                animation = ANIM_NEXT_LEVEL;
                anim_frame = 1;

                clear_keybuf();
//    clear_key();
// keyboard hack
                remove_keyboard();
                install_keyboard();
            }
        }
        else
        {
            anim = TRUE;
            animation = ANIM_GAME_COMPLETE;
            anim_frame = 1;
        }

        next_level = FALSE;
    }

    if (!pause_game)
    {
        int hit_scarab = 0;
        for (int i=0; i<8; i++)
            hit_scarab += collision(scarab[i]);

        /* player has been caught or went into exlposion */
        if ((collision(enemy) || is_in_dangerzone(player.x, player.y, TILE_SIZE, TILE_SIZE) || hit_scarab) > 0 && !player.dead)
        {
            //    done = TRUE;     // run death/game over animation: pixelization effect?
            player.dead = 1;
            whip.life = 0;
            play_sample(data[PAIN_SPL].dat, vol(127), 127, 1000, 0);
            //anim_game_over = 1; // do game over animation
//   blit(level_b, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
        }

        if (player.dead > 63)
        {
            if (plmap)
                done = TRUE;

            anim = TRUE;
            animation = ANIM_GAME_OVER;
//   init_anim = 1;
            anim_frame = 1;
        }
        else if (player.dead > 0)
            player.dead++;

        if (!enemy.dead && is_in_dangerzone(enemy.x, enemy.y, TILE_SIZE, TILE_SIZE))
        {
            enemy.dead = 1;
            play_sample(data[GRUNT_SPL].dat, vol(255), 127, 1000, 0);
        }

        if (enemy.dead > 63)
        {
            enemy.dead = 64;
            enemy.x = enemy.y = 0;
            stop_sample(data[GHOST_SPL].dat);
        }
        else if (enemy.dead > 0)
            enemy.dead++;

        for (int i=0; i<8; i++)
        {
            if (!scarab[i].dead && is_in_dangerzone(scarab[i].x, scarab[i].y, TILE_SIZE, TILE_SIZE))
            {
                scarab[i].dead = 1;
                play_sample(data[SQUISH_SPL].dat, vol(127), 127, 1000, 0);
            }

            if (scarab[i].dead > 63)
            {
                scarab[i].dead = 64;
                colmap[scarab[i].x][scarab[i].y] = 0;
                colmap[scarab[i].old_x][scarab[i].old_y] = 0;
                scarab[i].x = scarab[i].y = 0;
                scarab[i].old_x = scarab[i].old_y = 0;
                scarab[i].dir = NO_DIR;
            }
            else if (scarab[i].dead > 0)
                scarab[i].dead++;
        }

        enemy_move++;

        if (player.push)
        {
            if (enemy_move % 2)
                move_player();

            if (is_in_dangerzone(stone_x, stone_y, TILE_SIZE, TILE_SIZE))
            {
                player.push = 0;
                stone_dead = 1;
                //colmap[stone_x][stone_y] = 0;
                memset(&colmap, 0, sizeof(colmap));
            }
        }
        else
            move_player();

        if (stone_dead > 63)
            stone_dead = 0;
        else if (stone_dead > 0)
            stone_dead++;

        /* move enemy two times slower than player */
        if ((enemy_move % 5 < 2) && (enemy.x != 0) && (!enemy.dead) && (!enemy.pause)) // and only if there is an enemy at all
            move_enemy();

        if (enemy_move % 5 == 0)
            move_scarabs();
    }
}

void draw_game()
{
    if (update_map)
    {
        draw_map(level_b, 0, 0, MAP_SIZE_X, MAP_SIZE_Y, 0);
        update_map = FALSE;
        player.update = TRUE;
    }

    if (flash || !dark)
        blit(level_b, buffer, 0, 0, 0, 0, 640, 480);
    else
        blit(level_b, buffer, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);

    for (int i=0; i<8; i++)
    {
        if (scarab[i].dir == NO_DIR)
            continue;

        int rot = (scarab[i].dir-1)*64;

        if (scarab[i].dead && scarab[i].dead < 64)
            mul_blend16(dust_bmp[scarab[i].dead-1], buffer, scarab[i].x-16, scarab[i].y-16);
        else if (!scarab[i].dead && (is_in_raydius(scarab[i].x, scarab[i].y, TILE_SIZE, TILE_SIZE) || flash || !dark))
            rotate_sprite(buffer, data[SCARAB1+(enemy_move/16)%4].dat, scarab[i].x, scarab[i].y, itofix(rot));
    }

    if (!player.dead)
        draw_whip(buffer);

    if (player.push)
        draw_rle_sprite(buffer, data[STONE].dat, stone_x, stone_y);

    if (player.dead)
    {
        mul_blend16(dust_bmp[player.dead-1], buffer, player.x-16, player.y-16);
    }
    else if (player.sprite == data[PACIANA_LEFT].dat)
    {
        draw_fire(buffer);
        draw_rle_sprite(buffer, player.sprite, player.x, player.y);
    }
    else
    {
        draw_rle_sprite(buffer, player.sprite, player.x, player.y);
        draw_fire(buffer);
    }

    if (enemy.dead > 0 && enemy.dead < 64)
        mul_blend16(dust_bmp[enemy.dead-1], buffer, enemy.x-16, enemy.y-16);
    else if (enemy.dead == 0 && (is_in_raydius(enemy.x, enemy.y, TILE_SIZE, TILE_SIZE) || flash || !dark))
        draw_rle_sprite(buffer, enemy.sprite, enemy.x, enemy.y);

    if (stone_dead)
        mul_blend16(dust_bmp[stone_dead-1], buffer, stone_x-16, stone_y-16);

    if (flash)
    {
        if (flash < 64)
            draw_explosions();
        //draw_explmap();

        if (dark)
        {
            //flash_intensity = fixtoi(fixsin(itofix(flash/2)) / 64);
            //flash_intensity = fixtoi(fixsin(itofix(flash<<1)) << 7);
            flash_intensity = (int)(sin(flash/80.0)*exp((256-flash)/35.0));
            //printf("%d %d\n", flash, flash_intensity);
            clear_to_color(flash_bmp, makecol(flash_intensity, flash_intensity, flash_intensity));
            render_lightmask();
            draw_flash();

        }
        //   draw_explosion(buffer);
        //   add_blend16(expl_bmp[flash-1], buffer, player.x-16, player.y-16);
        //   mul_blend16(dust_bmp[flash-1], buffer, player.x, player.y);
        if (flash < 64)
            blit(buffer, screen, 0, 0, rand()%(1+((256-flash)/8)), rand()%(1+((256-flash)/10)), 640, 480);
        else
            blit(buffer, screen, 0, 0, 0, 0, 640, 480);

        clear(buffer);
    }
    else
    {
        if (dynamite_count && (is_in_raydius(sparkle_x-10, sparkle_y-10, 20, 20) || !dark))
            draw_sparkles(buffer);

        if (dark)
        {
            render_lightmask();
            draw_lighting(255);
            //blit(buffer, screen, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, player.x+TILE_SIZE/2-LIGHT_SIZE/2, player.y+TILE_SIZE/2-LIGHT_SIZE/2, LIGHT_SIZE, LIGHT_SIZE);
        }

        if (volume[3]) // fps?
            textprintf_right_ex(buffer, font, SCREEN_W-10, 10, makecol(255, 255, 255), -1, "%d", fps);

        blit(buffer, screen, 0, 0, 0, 0, 640, 480);
        clear(buffer);
    }
    /*
        player.old_x = player.x;
        player.old_y = player.y;
        enemy.old_x = enemy.x;
        enemy.old_y = enemy.y;
    */
    frame_count++;

//    draw_colmap();
}

void move_player()
{
    int px, py;

    if ((player.x%TILE_SIZE == 0) && (player.y%TILE_SIZE == 0))
    {
        px = player.x/TILE_SIZE;
        py = player.y/TILE_SIZE;

        // if player was previously pushing a stone
        if (player.push)
        {
            //    memset(&colmap, 0, sizeof(colmap));
            colmap[px][py] = 0; // stone was previously at player's current location

            switch (player.dir)
            {
            case NORTH:
                map[px][py-1] = STONE;
                break;

            case EAST:
                map[px+1][py] = STONE;
                break;

            case SOUTH:
                map[px][py+1] = STONE;
                break;

            case WEST:
                map[px-1][py] = STONE;
                break;

            default:
                break;
            }

            update_map = TRUE;
        }

        if (player.dir != NO_DIR)
            player.old_dir = player.dir;

        player.dir = NO_DIR;
        player.push = 0;
        player.update = FALSE;

        if (key[KEY_UP])
        {
            player.update = TRUE;
            player.sprite = data[PACIANA_BACK].dat;
            fire_x = 29/*-3*/;
            fire_y = 12/*-16*/;

            if (map[px][py-1] == 0)
            {
                player.dir = NORTH;
                player.y--;
            }
            else if (map[px][py-1] == KEY)
            {
                map[px][py-1] = 0;
                update_map = TRUE;

                play_sample(data[KEYS_SPL].dat, vol(127), 127, 1000, 0);

                player.keys++;
                player.dir = NORTH;
                player.y--;
            }
            else if (map[px][py-1] == DOOR)
            {
                if (player.keys == num_keys)
                {
                    play_sample(data[UNLOCK_SPL].dat, vol(127), 127, 1000, 0);
                    next_level = TRUE;
                }
                else
                {
                    if (l == 0)
                    {
                        play_sample(data[LOCKED_SPL].dat, vol(127), 127, 1000, 0);
                        l = 1;
                    }
                }
            }
            else if (map[px][py-1] == STONE)
            {
                if (map[px][py-2] == 0 && colmap[px][py-2] == 0)
                {
                    play_sample(data[STONE3_SPL].dat, vol(127), 127, 1000, 0);
                    map[px][py-1] = 0;
                    colmap[px][py-1] = STONE;
                    colmap[px][py-2] = STONE;
                    update_map = TRUE;
                    player.push = 1;
                    player.dir = NORTH;
                    player.y--;
                    stone_x = player.x;
                    stone_y = player.y-TILE_SIZE;
                }
            }
            else if (map[px][py-1] == DYNAMITE)
            {
                if (player.dynamite < MAX_DYNAMITE)
                {
                    play_sample(data[PICKUP_SPL].dat, vol(127), 127, 1000, 0);
                    player.dynamite++;
                    map[px][py-1] = 0;
                    update_map = TRUE;
                    player.dir = NORTH;
                    player.y--;
                }
            }
        }
        else if (key[KEY_RIGHT])
        {
            player.update = TRUE;
            player.sprite = data[PACIANA_RIGHT].dat;
            fire_x = 12/*-3*/;
            fire_y = 17/*-16*/;

            if (map[px+1][py] == 0)
            {
                player.dir = EAST;
                player.x++;
            }
            else if (map[px+1][py] == KEY)
            {
                map[px+1][py] = 0;
                update_map = TRUE;
                play_sample(data[KEYS_SPL].dat, vol(127), 127, 1000, 0);

                player.keys++;
                player.dir = EAST;
                player.x++;
            }
            else if (map[px+1][py] == DOOR)
            {
                if (player.keys == num_keys)
                {
                    play_sample(data[UNLOCK_SPL].dat, vol(127), 127, 1000, 0);
                    next_level = TRUE;
                }
                else
                {
                    if (l == 0)
                    {
                        play_sample(data[LOCKED_SPL].dat, vol(127), 127, 1000, 0);
                        l = 1;
                    }
                }
            }
            else if (map[px+1][py] == STONE)
            {
                if (map[px+2][py] == 0 && colmap[px+2][py] == 0)
                {
                    play_sample(data[STONE3_SPL].dat, vol(127), 127, 1000, 0);
                    map[px+1][py] = 0;
                    colmap[px+1][py] = STONE;
                    colmap[px+2][py] = STONE;
                    update_map = TRUE;
                    player.push = 1;
                    player.dir = EAST;
                    player.x++;
                    stone_x = player.x+TILE_SIZE;
                    stone_y = player.y;
                }
            }
            else if (map[px+1][py] == DYNAMITE)
            {
                if (player.dynamite < MAX_DYNAMITE)
                {
                    play_sample(data[PICKUP_SPL].dat, vol(127), 127, 1000, 0);
                    player.dynamite++;
                    map[px+1][py] = 0;
                    update_map = TRUE;
                    player.dir = EAST;
                    player.x++;
                }
            }
        }
        else if (key[KEY_DOWN])
        {
            player.update = TRUE;
            player.sprite = data[PACIANA_FRONT].dat;
            fire_x = 0/*-3*/;
            fire_y = 15/*-16*/;

            if (map[px][py+1] == 0)
            {
                player.dir = SOUTH;
                player.y++;
            }
            else if (map[px][py+1] == KEY)
            {
                map[px][py+1] = 0;
                update_map = TRUE;
                play_sample(data[KEYS_SPL].dat, vol(127), 127, 1000, 0);

                player.keys++;
                player.dir = SOUTH;
                player.y++;
            }
            else if (map[px][py+1] == DOOR)
            {
                if (player.keys == num_keys)
                {
                    play_sample(data[UNLOCK_SPL].dat, vol(127), 127, 1000, 0);
                    next_level = TRUE;
                }
                else
                {
                    if (l == 0)
                    {
                        play_sample(data[LOCKED_SPL].dat, vol(127), 127, 1000, 0);
                        l = 1;
                    }
                }
            }
            else if (map[px][py+1] == STONE)
            {
                if (map[px][py+2] == 0 && colmap[px][py+2] == 0)
                {
                    play_sample(data[STONE3_SPL].dat, vol(127), 127, 1000, 0);
                    map[px][py+1] = 0;
                    colmap[px][py+1] = STONE;
                    colmap[px][py+2] = STONE;
                    update_map = TRUE;
                    player.push = 1;
                    player.dir = SOUTH;
                    player.y++;
                    stone_x = player.x;
                    stone_y = player.y+TILE_SIZE;
                }
            }
            else if (map[px][py+1] == DYNAMITE)
            {
                if (player.dynamite < MAX_DYNAMITE)
                {
                    play_sample(data[PICKUP_SPL].dat, vol(127), 127, 1000, 0);
                    player.dynamite++;
                    map[px][py+1] = 0;
                    update_map = TRUE;
                    player.dir = SOUTH;
                    player.y++;
                }
            }
        }
        else if (key[KEY_LEFT])
        {
            player.update = TRUE;
            player.sprite = data[PACIANA_LEFT].dat;
            fire_x = 17/*-3*/;
            fire_y = 17/*-16*/;

            if (map[px-1][py] == 0)
            {
                player.dir = WEST;
                player.x--;
            }
            else if (map[px-1][py] == KEY)
            {
                map[px-1][py] = 0;
                update_map = TRUE;
                play_sample(data[KEYS_SPL].dat, vol(127), 127, 1000, 0);

                player.keys++;
                player.dir = WEST;
                player.x--;
            }
            else if (map[px-1][py] == DOOR)
            {
                if (player.keys == num_keys)
                {
                    play_sample(data[UNLOCK_SPL].dat, vol(127), 127, 1000, 0);
                    next_level = TRUE;
                }
                else
                {
                    if (l == 0)
                    {
                        play_sample(data[LOCKED_SPL].dat, vol(127), 127, 1000, 0);
                        l = 1;
                    }
                }
            }
            else if (map[px-1][py] == STONE)
            {
                if (map[px-2][py] == 0 && colmap[px-2][py] == 0)
                {
                    play_sample(data[STONE3_SPL].dat, vol(127), 127, 1000, 0);
                    map[px-1][py] = 0;
                    colmap[px-1][py] = STONE;
                    colmap[px-2][py] = STONE;
                    update_map = TRUE;
                    player.push = 1;
                    player.dir = WEST;
                    player.x--;
                    stone_x = player.x-TILE_SIZE;
                    stone_y = player.y;
                }
            }
            else if (map[px-1][py] == DYNAMITE)
            {
                if (player.dynamite < MAX_DYNAMITE)
                {
                    play_sample(data[PICKUP_SPL].dat, vol(127), 127, 1000, 0);
                    player.dynamite++;
                    map[px-1][py] = 0;
                    update_map = TRUE;
                    player.dir = WEST;
                    player.x--;
                }
            }
        }

        if (!player.update)
            l = 0;
    }
    switch (player.dir)
    {
    case NORTH:
        player.y--;
        stone_y -= player.push;
        break;

    case EAST:
        player.x++;
        stone_x += player.push;
        break;

    case SOUTH:
        player.y++;
        stone_y += player.push;
        break;

    case WEST:
        player.x--;
        stone_x -= player.push;
        break;
    }
}

int opposite_dir(int dir)
{
    if (dir == WEST)
        return EAST;
    else if (dir == EAST)
        return WEST;
    else if (dir == NORTH)
        return SOUTH;
    else if (dir == SOUTH)
        return NORTH;
    else
        return NO_DIR;
}

int get_free_dirs(int *list)
{
    int x, y;
    int i, j, c;
// int stone = NO_DIR;

    x = enemy.x / TILE_SIZE;
    y = enemy.y / TILE_SIZE;

    c = 0;

    /* if there is a walkable direction add it to the list of directions */

    if ((map[x+1][y] == 0) && (colmap[x+1][y] == 0))
        list[c++] = EAST;

    if ((map[x-1][y] == 0) && (colmap[x-1][y] == 0))
        list[c++] = WEST;

    if ((map[x][y+1] == 0) && (colmap[x][y+1] == 0))
        list[c++] = SOUTH;

    if ((map[x][y-1] == 0) && (colmap[x][y-1] == 0))
        list[c++] = NORTH;

    /* and if there is more than one walkable direction
       delete the one the enemy just came from
       to not let him turn around out of a sudden */
    if (c > 1)
    {
        for (i=0; i<c; i++)
        {
            if (list[i] == opposite_dir(enemy.dir))
            {
                c--;
                break;
            }
        }

        /* delete direction and close gap in list */
        for (j=i; j<c; j++)
            list[j] = list[j+1];
    }

    /* return number of free directions */
    return c;
}

void move_scarabs()
{
    for (int i=0; i<8; i++)
    {
        if (scarab[i].dir == NO_DIR)
            continue;

        int x = scarab[i].x / TILE_SIZE;
        int y = scarab[i].y / TILE_SIZE;
        int on_grid = ((scarab[i].x%TILE_SIZE == 0) && (scarab[i].y%TILE_SIZE == 0));
        int nx=0, ny=0;

        switch (scarab[i].dir)
        {
        case NORTH:
            ny--;
            if (!on_grid || !map[x][y-1] && colmap[x][y-1] != STONE)
                scarab[i].y--;
            else
                scarab[i].dir = SOUTH;
            break;

        case EAST:
            nx++;
            if (!on_grid || !map[x+1][y] && colmap[x+1][y] != STONE)
                scarab[i].x++;
            else
                scarab[i].dir = WEST;
            break;

        case SOUTH:
            ny++;
            if (!on_grid || !map[x][y+1] && colmap[x][y+1] != STONE)
                scarab[i].y++;
            else
                scarab[i].dir = NORTH;
            break;

        case WEST:
            nx--;
            if (!on_grid || !map[x-1][y] && colmap[x-1][y] != STONE)
                scarab[i].x--;
            else
                scarab[i].dir = EAST;
            break;
        }

        if (on_grid)
        {
            colmap[x][y] = SCARAB1;
            colmap[x+nx][y+ny] = SCARAB1;
            colmap[scarab[i].old_x][scarab[i].old_y] = 0;
            scarab[i].old_x = x;
            scarab[i].old_y = y;
        }
    }
}

void move_enemy()
{
    int free_dirs[4];
    int num_free_dirs;
    int x, y;

    x = enemy.x/TILE_SIZE;
    y = enemy.y/TILE_SIZE;

// enemy is perfectly placed on one single tile
    if ((enemy.x%TILE_SIZE == 0) && (enemy.y%TILE_SIZE == 0))
    {
        // make place where enemy just came from walkable again in collision map
        switch (enemy.dir)
        {
        case NORTH:
            colmap[x][y+1] = 0;
            break;

        case EAST:
            colmap[x-1][y] = 0;
            break;

        case SOUTH:
            colmap[x][y-1] = 0;
            break;

        case WEST:
            colmap[x+1][y] = 0;
            break;

//   case NO_DIR
        }

        // search for a new way to go
        num_free_dirs = get_free_dirs(free_dirs);

        if (num_free_dirs > 0)
            enemy.dir = free_dirs[rand()%num_free_dirs];

        switch (enemy.dir)
        {
        case NORTH:
            enemy.y--;
            enemy.sprite = data[MUMMY_BACK].dat;
            colmap[x][y-1] = MUMMY_FRONT;
            break;

        case EAST:
            enemy.x++;
            enemy.sprite = data[MUMMY_RIGHT].dat;
            colmap[x+1][y] = MUMMY_FRONT;
            break;

        case SOUTH:
            enemy.y++;
            enemy.sprite = data[MUMMY_FRONT].dat;
            colmap[x][y+1] = MUMMY_FRONT;
            break;

        case WEST:
            enemy.x--;
            enemy.sprite = data[MUMMY_LEFT].dat;
            colmap[x-1][y] = MUMMY_FRONT;
            break;
        }
    }
    else
    {
        switch (enemy.dir)
        {
        case NORTH:
            enemy.y--;
            break;

        case EAST:
            enemy.x++;
            break;

        case SOUTH:
            enemy.y++;
            break;

        case WEST:
            enemy.x--;
            break;
        }
    }
}

/* bounding sphere collision detection */
int collision(struct enemy_t e)
{
    if (e.dead)
        return 0;

    int r = (player.x-e.x)*(player.x-e.x) + (player.y-e.y)*(player.y-e.y);

    if (r < TILE_SIZE*TILE_SIZE)
        return 1;

    return 0;
}

void draw_colmap()
{
    int i, j;

    for (i=0; i<MAP_SIZE_X; i++)
        for (j=0; j<MAP_SIZE_Y; j++)
            if (colmap[i][j])
                rectfill(screen, i*TILE_SIZE, j*TILE_SIZE, i*TILE_SIZE+4, j*TILE_SIZE+4, makecol(255, 255, 0));
}

void reset_game()
{
    memset(&player, 0, sizeof(player));
    memset(&enemy, 0, sizeof(enemy));
    memset(scarab, 0, sizeof(scarab));
    memset(colmap, 0, sizeof(colmap));
    memset(&whip, 0, sizeof(whip));

    dark = 0;
    num_keys = 0;
    player.keys = 0;
    player.sprite = data[PACIANA_FRONT].dat;
    player.dead = player.dynamite = 0;
    fire_x = 0/*-3*/;
    fire_y = 15/*-16*/;
    enemy.x = enemy.y = enemy.dir = enemy.dead = 0;
    enemy.sprite = data[MUMMY_FRONT].dat;

    flash = 0;
    dynamite_count = 0;
    dynamite_x = dynamite_y = 0;
    stone_dead = 0;
    stone_x = stone_y = 0;
//	fire_x = fire_y = 0;
    sparkle_x = sparkle_y = 0;

    clear_explmap();

    memset(&fire, 0, sizeof(fire));
    memset(&explosion, 0, sizeof(explosion));
    memset(&sparkle, 0, sizeof(sparkle));

    stop_sample(data[GHOST_SPL].dat);
    stop_sample(data[FIRE_CRACKLE].dat);
    play_sample(data[GHOST_SPL].dat, 0, 0, 300, 1);
    play_sample(data[FIRE_CRACKLE].dat, vol(64), 127, 1000, 1);
}
