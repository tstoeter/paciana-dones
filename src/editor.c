#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>

#include "editor.h"
#include "map.h"
#include "lost.h"

DATAFILE *data;
BITMAP *buffer;
BITMAP *level_b;
char save_path[480] = "";
int dark = 0;

/* set the map data to 0 */
void new_map()
{
    int i, j;

    /* set all map elements to 0 */
    for (i=0; i<MAP_SIZE_X; i++)
        for (j=0; j<MAP_SIZE_Y; j++)
            map[i][j] = 0;

    editor.current_tile = TILE1;
    editor.last_tile = 0;
}

void draw_interface()
{
    int i;

    clear_to_color(buffer, makecol(0, 0, 0));

    textout_ex(buffer, font, "P", 16, 120+0*48+12, makecol(255,255,255), -1);
    textout_ex(buffer, font, "M", 112, 120+0*48+12, makecol(255,255,255), -1);
    draw_rle_sprite(buffer, data[PACIANA_FRONT].dat, 32, 120+0*48);
    draw_rle_sprite(buffer, data[MUMMY_FRONT].dat, 72, 120+0*48);

    textout_ex(buffer, font, "X", 16, 120+1*48+12, makecol(255,255,255), -1);
    textout_ex(buffer, font, "R", 112, 120+1*48+12, makecol(255,255,255), -1);
    draw_rle_sprite(buffer, data[DYNAMITE].dat, 32, 120+1*48);
    draw_rle_sprite(buffer, data[STONE].dat, 72, 120+1*48);

    textout_ex(buffer, font, "K", 16, 120+2*48+12, makecol(255,255,255), -1);
    textout_ex(buffer, font, "E", 112, 120+2*48+12, makecol(255,255,255), -1);
    draw_rle_sprite(buffer, data[KEY].dat, 32, 120+2*48);
    draw_rle_sprite(buffer, data[DOOR].dat, 72, 120+2*48);

    textout_ex(buffer, font, "WASD", 72, 120+3*48+12, makecol(255,255,255), -1);
    draw_sprite(buffer, data[SCARAB1].dat, 32, 120+3*48);

    for (int i=1; i<=5; i++)
    {
        textprintf_ex(buffer, font, 16, 120+(3+i)*48+12, makecol(255,255,255), -1, "%d", i);
        textprintf_ex(buffer, font, 112, 120+(3+i)*48+12, makecol(255,255,255), -1, "%d", i+5);
    }

    textprintf_ex(buffer, font, 112, 120+8*48+12, makecol(255,255,255), makecol(0,0,0), "0 ");

    draw_rle_sprite(buffer, data[TILE1].dat, 32, 120+4*48);
    draw_rle_sprite(buffer, data[TILE2].dat, 32, 120+5*48);
    draw_rle_sprite(buffer, data[TILE3].dat, 32, 120+6*48);
    draw_rle_sprite(buffer, data[TILE4].dat, 32, 120+7*48);
    draw_rle_sprite(buffer, data[TILE5].dat, 32, 120+8*48);

    draw_rle_sprite(buffer, data[TILE6].dat, 72, 120+4*48);
    draw_rle_sprite(buffer, data[TILE7].dat, 72, 120+5*48);
    draw_rle_sprite(buffer, data[TILE8].dat, 72, 120+6*48);
    draw_rle_sprite(buffer, data[TILE9].dat, 72, 120+7*48);
    draw_rle_sprite(buffer, data[TILE10].dat, 72, 120+8*48);

    textout_centre_ex(buffer, font, " F1      F2      F3      F4      F5      ESC ", SCREEN_W/2, 36, makecol(255, 255, 255), -1);
    textout_centre_ex(buffer, font, "Help    Save    Load    Play     New     Exit", SCREEN_W/2, 48, makecol(255, 255, 255), -1);
}

void update_screen()
{
    int i;

    show_mouse(NULL);

    draw_map(level_b, 0, 0, MAP_SIZE_X, MAP_SIZE_Y, 1);

    for (i=0; i<MAP_SIZE_X; i++)
    {
        if (i < MAP_SIZE_Y)
            hline(level_b, 0, i*TILE_SIZE, SCREEN_W-1, makecol(255, 255, 255));

        vline(level_b, i*TILE_SIZE, 0, SCREEN_H-1, makecol(255, 255, 255));
    }

    blit(level_b, buffer, 0, 0, 160, 120, 640, 480);
    blit(buffer, screen, 0, 0, 0, 0, 800, 600);

    show_mouse(screen);
}

/* init routine */
void init_editor()
{
    allegro_init();
    install_keyboard();
    install_timer();
    install_mouse();

    set_color_depth(16);
    set_palette(desktop_palette);

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0) != 0)
    {
        allegro_exit();
        printf("Error setting graphics mode");
        exit(-1);
    }

    show_mouse(screen);
//set_mouse_range(160, 120, SCREEN_W-1, SCREEN_H-1);

    data = load_datafile("lost.dat");

    if (!data)
    {
        allegro_exit();
        printf("Error loading data file");
        exit(-1);
    }

    buffer = create_bitmap(800, 600);
    level_b = create_bitmap(640, 480);

    clear(screen);
    clear(buffer);
    clear(level_b);

    draw_interface();

    new_map();
    update_screen();
}

void exit_editor()
{
    unload_datafile(data);

    destroy_bitmap(buffer);
    destroy_bitmap(level_b);

    allegro_exit();

    printf("Thank you for using the level editor!\n");
}

int edit_map()
{
    int x, y;
    int tile;

    if (!mouse_b)
        return 0;

    if (mouse_b & 1)
        tile = editor.current_tile;
    else
        tile = 0;

    /* compute mouse position */
    x = (mouse_x - 160) / TILE_SIZE;
    y = (mouse_y - 120) / TILE_SIZE;

    if ((x == editor.x) && (y == editor.y) && (tile == editor.last_tile))
        return 0;

    map[x][y] = tile;

    editor.x = x;
    editor.y = y;
    editor.last_tile = tile;

    return 1;
}

void help_dialog()
{
    DIALOG dialog[] =
    {
        /* (dialog proc)        (x)   (y)   (w)   (h)   (fg)  (bg)  (key)      (flags)  (d1)                    (d2)  (dp)                                    (dp2) (dp3) */
        { _gui_shadow_box_proc, 0,    0,    400,  400,  0,    0,    0,         0,       0,                      0,    NULL,                                   NULL, NULL  },
        { d_ctext_proc,         200,  16,   0,    0,    255,  0,    0,         0,       0,                      0,    "Paciana Dones Map Editor Help",        NULL, NULL  },
        { d_textbox_proc,       8,    40,   384,  320,  255,  0,    0,         0,       0,                      0,    &editor_help,                           NULL, NULL  },
        { d_button_proc,        164,  370,  64,   16,   255,  0,    KEY_ESC,   D_EXIT,  0,                      0,    "OK",                                   NULL, NULL  },
        { NULL,                 0,    0,    0,    0,    0,    0,    0,         0,       0,                      0,    NULL,                                   NULL, NULL  }
    };

    centre_dialog(dialog);
    set_dialog_color(dialog, makecol(0, 0, 0), makecol(255, 255, 255));

    popup_dialog(dialog, 0);
}

void run_editor()
{
    int i, done;
    char path[480];

    done = 0;
    strcpy(path, "./maps/");

    while (!done)
    {
        rest(1);

        if (edit_map())
            update_screen();

        if (key[KEY_ESC])
        {
            /* don't exit dialog right away */
            while (key[KEY_ESC]);

            if (alert("Really want to quit?", NULL, NULL, "Yes", "Cancel", KEY_ENTER, KEY_ESC) == 1)
                done = 1;

            /* prevent from calling right again */
            while (key[KEY_ESC]);
        }
        else if (key[KEY_F1])
        {
            help_dialog();
            update_screen();

            /* prevent from calling exit dialog when KEY_ESC was pressed to cancel */
            while (key[KEY_ESC]);
        }
        else if (key[KEY_F2])
        {
            if (file_select_ex("Save map", path, "map", 480, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT) != 0)
            {
                if (save_map(path, 0) == 3)
                {
                    if (alert(map_error, "Override?", NULL, "Yes", "Cancel", KEY_ENTER, KEY_ESC) == 1)
                    {
                        save_map(path, 1);
                        strcpy(save_path, path);
                    }
                }
                else
                    strcpy(save_path, path);
            }

            /* prevent from calling exit dialog when KEY_ESC was pressed to cancel */
            while (key[KEY_ESC]);
        }
        else if(key[KEY_F3])
        {
            if (file_select_ex("Load map", path, "map", 480, OLD_FILESEL_WIDTH, OLD_FILESEL_HEIGHT) != 0)
            {
                if ((i = load_map(path, 1)) != 0)
                    alert(map_error, NULL, NULL, "OK", NULL, KEY_ENTER, KEY_ESC);
                strcpy(save_path, path);
            }

            update_screen();

            /* prevent from calling exit dialog when KEY_ESC was pressed to cancel */
            while (key[KEY_ESC]);
        }
        else if (key[KEY_F4])
        {
            // play current map, but save first
            if (strcmp(save_path, "") != 0)
            {
                char cmd[1024];
#ifdef __linux__
                sprintf(cmd, "./paciana \"%s\"", save_path);
#else
                sprintf(cmd, "paciana.exe \"%s\"", save_path);
#endif
                puts(cmd);
                system(cmd);
            }
            else
                alert("Save map first!", NULL, NULL, "OK", NULL, KEY_ENTER, KEY_ESC);

            /* prevent from calling exit dialog when KEY_ESC was pressed to cancel */
            while (key[KEY_ESC]);
        }
        else if (key[KEY_F5])
        {
            strcpy(path, "./maps/");
            new_map();
            update_screen();

            /* prevent from calling exit dialog when KEY_ESC was pressed to cancel */
            while (key[KEY_ESC]);
        }
        else if (key[KEY_1])
            editor.current_tile = TILE1;
        else if (key[KEY_2])
            editor.current_tile = TILE2;
        else if (key[KEY_3])
            editor.current_tile = TILE3;
        else if (key[KEY_4])
            editor.current_tile = TILE4;
        else if (key[KEY_5])
            editor.current_tile = TILE5;
        else if (key[KEY_6])
            editor.current_tile = TILE6;
        else if (key[KEY_7])
            editor.current_tile = TILE7;
        else if (key[KEY_8])
            editor.current_tile = TILE8;
        else if (key[KEY_9])
            editor.current_tile = TILE9;
        else if (key[KEY_0])
            editor.current_tile = TILE10;
        else if (key[KEY_W])
            editor.current_tile = SCARAB1+0;
        else if (key[KEY_A])
            editor.current_tile = SCARAB1+3;
        else if (key[KEY_S])
            editor.current_tile = SCARAB1+2;
        else if (key[KEY_D])
            editor.current_tile = SCARAB1+1;
        else if (key[KEY_M])
            editor.current_tile = MUMMY_FRONT;
        else if (key[KEY_P])
            editor.current_tile = PACIANA_FRONT;
        else if (key[KEY_X])
            editor.current_tile = DYNAMITE;
        else if (key[KEY_R])
            editor.current_tile = STONE;
        else if (key[KEY_K])
            editor.current_tile = KEY;
        else if (key[KEY_E])
            editor.current_tile = DOOR;
    }
}

int main()
{
    init_editor();

    run_editor();

    exit_editor();

    return 0;
}
END_OF_MAIN();

