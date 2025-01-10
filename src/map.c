#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "game.h"
#include "map.h"
#include "lost.h"

unsigned char map[MAP_SIZE_X][MAP_SIZE_Y];
unsigned int num_keys;

unsigned int level = 6;
unsigned int next_level = FALSE;
unsigned int next_map = FALSE;
unsigned int update_map = FALSE;
unsigned char map_error[256];
unsigned char map_files[MAX_MAP_FILES][256];
unsigned int num_mapfiles = 0;

/* map loading routine */
int load_map(char *filename, unsigned char editor)
{
    FILE *file;
    char test_id[8];
    char test_ver[8];
    int i, j;

    /* used for checking map for validity,
       each map has to contain 1 player, 1 enemy, 1 door and 3 keys */
    int num_players, num_enemies, num_doors, num_scarabs;
    enemy.x = enemy.y = enemy.dir = 0;
    enemy.dead = 64;

    num_players = num_enemies = num_doors = num_scarabs = 0;

    /* open file in binary mode */
    file = fopen(filename, "rb");

    /* if an error occured display error message and exit */
    if (!file)
    {
        strcpy(map_error, "Map file could not be opened.");
        return 1;
    }

    /* read MAP_ID */
    fread(test_id, sizeof(char), strlen(MAP_ID)+1, file);

    /* read MAP_VER */
    fread(test_ver, sizeof(char), strlen(MAP_VER)+1, file);

    /* if invalid MAP_ID or MAP_VER display error message and exit */
    if ((strcmp(test_ver, MAP_VER) != 0) || (strcmp(test_id, MAP_ID) != 0))
    {
        fclose(file);

        strcpy(map_error, "Wrong map type or version.");
        return 2;
    }

    /* read map data */
    fread(map, sizeof(unsigned char), MAP_SIZE_X*MAP_SIZE_Y, file);

    fclose(file);

    /* get player's and enemy's initial positions */
    for (j=0; j<MAP_SIZE_Y; j++)
    {
        for (i=0; i<MAP_SIZE_X; i++)
        {
            /* set player's variables */
            if (map[i][j] == PACIANA_FRONT)
            {
                if (!editor)
                    map[i][j] = 0;

                num_players++;

                player.x = /*player.old_x =*/ i * TILE_SIZE;
                player.y = /*player.old_y =*/ j * TILE_SIZE;

                player.dir = player.old_dir = NO_DIR;
                player.dead = 0;
                player.dynamite = 0;
            }
            /* set enemy's variables */
            else if (map[i][j] == MUMMY_FRONT)
            {
                if (!editor)
                    map[i][j] = 0;

                num_enemies++;

                enemy.x = /*enemy.old_x =*/ i * TILE_SIZE;
                enemy.y = /*enemy.old_y =*/ j * TILE_SIZE;
                enemy.dead = 0;
            }
            else if (map[i][j] >= SCARAB1 && map[i][j] <= SCARAB4)
            {
                int c = num_scarabs;
                scarab[c].dir = scarab[c].old_dir = 1+map[i][j]-SCARAB1;

                if (!editor)
                    map[i][j] = 0;

                scarab[c].x = /*scarab[c].old_x =*/ i * TILE_SIZE;
                scarab[c].y = /*scarab[c].old_y =*/ j * TILE_SIZE;
                scarab[c].dead = 0;

                num_scarabs++;
            }
            /* also check for the other requiered items (door and keys) */
            else if (map[i][j] == KEY)
                num_keys++;
            else if (map[i][j] == DOOR)
                num_doors++;
        }
    }

    if (map[0][1] == DYNAMITE && !editor)
    {
        dark = 1;
        map[0][1] = map[1][0];
    }

    if (map[0][0] == 0 && !editor)
        map[0][0] = TILE1;

    if ((num_players != 1) || (num_enemies != 1) || (num_doors != 1))
    {
        sprintf(map_error, "Incorrect number of players (%d), enemies (%d) or doors (%d) contained in map.", num_players, num_enemies, num_doors);
        return 3;
    }

    return 0;
}

/* map loading routine */
int load_map_from_datafile(void *dat)
{
    int i, j;

    /* used for checking map for validity,
       each map has to contain one player, one enemy and one door */
    int num_players, num_enemies, num_doors, num_scarabs;

    num_players = num_enemies = num_doors = num_scarabs = 0;
    enemy.x = enemy.y = enemy.dir = 0;
    enemy.dead = 64;

    memcpy(&map, dat+5, MAP_SIZE_X*MAP_SIZE_Y);

    /* get player's and enemy's initial positions */
    for (j=0; j<MAP_SIZE_Y; j++)
    {
        for (i=0; i<MAP_SIZE_X; i++)
        {
            /* set player's variables */
            if (map[i][j] == PACIANA_FRONT)
            {
                map[i][j] = 0;

                num_players++;

                player.x = /*player.old_x =*/ i * TILE_SIZE;
                player.y = /*player.old_y =*/ j * TILE_SIZE;
                player.dir = player.old_dir = NO_DIR;
                player.dead = 0;
                player.dynamite = 0;
            }
            /* set enemy's variables */
            else if (map[i][j] == MUMMY_FRONT)
            {
                map[i][j] = 0;

                num_enemies++;

                enemy.x = /*enemy.old_x =*/ i * TILE_SIZE;
                enemy.y = /*enemy.old_y =*/ j * TILE_SIZE;
                enemy.dead = 0;
            }
            else if (map[i][j] >= SCARAB1 && map[i][j] <= SCARAB4)
            {
                int c = num_scarabs;
                scarab[c].dir = scarab[c].old_dir = 1+map[i][j]-SCARAB1;

                map[i][j] = 0;

                scarab[c].x = /*scarab[c].old_x =*/ i * TILE_SIZE;
                scarab[c].y = /*scarab[c].old_y =*/ j * TILE_SIZE;
                scarab[c].dead = 0;

                num_scarabs++;
            }
            /* also check for the other requiered items (door and keys) */
            else if (map[i][j] == KEY)
                num_keys++;
            else if (map[i][j] == DOOR)
                num_doors++;
        }
    }

    if (map[0][1] == DYNAMITE)
    {
        dark = 1;
        map[0][1] = map[1][0];
    }

    if (map[0][0] == 0)
        map[0][0] = TILE1;

    if ((num_players != 1) || (num_enemies != 1) || (num_doors != 1))
    {
        sprintf(map_error, "Incorrect number of players (%d), enemies (%d) or doors (%d) contained in map.", num_players, num_enemies, num_doors);
        return 3;
    }

    return 0;
}

/* map saving routine (used in editor only) */
int save_map(char *filename, unsigned char override)
{
    FILE *file;
    int i, j;

    /* used for checking map for validity,
       each map has to contain one player, one enemy and one door */
    int num_players, num_enemies, num_doors;

    num_players = num_enemies = num_doors = 0;

    /* check for map validity */
    for (j=0; j<MAP_SIZE_Y; j++)
    {
        for (i=0; i<MAP_SIZE_X; i++)
        {
            if (map[i][j] == PACIANA_FRONT)
                num_players++;
            else if (map[i][j] == MUMMY_FRONT)
                num_enemies++;
            else if (map[i][j] == DOOR)
                num_doors++;
            else if (map[i][j] == KEY)
                num_keys++;
        }
    }

    if (!override)
    {
        if ((num_players != 1) || (num_enemies != 1) || (num_doors != 1))
        {
            sprintf(map_error, "Incorrect number of players (%d), enemies (%d) or doors (%d) contained in map.", num_players, num_enemies, num_doors);
            return 3;
        }
    }

    /* open the file */
    file = fopen(filename, "wb");

    /* if error display message box */
    if (!file)
    {
        strcpy(map_error, "Map file could not be saved.");
        alert(map_error, NULL, NULL, "OK", NULL, KEY_ESC, KEY_ENTER);
        return 1;
    }

    /* write the MAP_ID */
    fwrite(MAP_ID, sizeof(char), strlen(MAP_ID)+1, file);

    /* write MAP_VER */
    fwrite(MAP_VER, sizeof(char), strlen(MAP_VER)+1, file);

    /* write the map data */
    fwrite(map, sizeof(unsigned char), MAP_SIZE_X*MAP_SIZE_Y, file);

    /* close file */
    fclose(file);

    return 0;
}

/* map drawing routine */
void draw_map(BITMAP *bmp, int mx, int my, int mw, int mh, unsigned char editor)
{
    int i, j;
    int tile, floor;

    /* loop through whole map */
    for (j=my; j<my+mh; j++)
    {
        for (i=mx; i<mx+mw; i++)
        {
            tile = map[i][j];

            if (tile == DYNAMITE_ACTIVE)
                tile = DYNAMITE;

            /* if there's no tile or the player, enemy, key or door tile */
            if ((tile == 0) || (tile == KEY) || (tile == DOOR) || (tile == STONE) || (tile == DYNAMITE) || (tile == PACIANA_FRONT) || (tile == MUMMY_FRONT) || ((tile >= SCARAB1) && (tile <= SCARAB4)))
            {
                /* in the game */
                if (!editor)
                {
                    /* fill the empty space with a floor tile, the shaded tile of map[0][0] */
                    floor = map[0][0]; //map[0][0] = map[1][0];
                    set_trans_blender(0, 0, 0, 0);
                    draw_lit_rle_sprite(bmp, data[floor].dat, i*TILE_SIZE, j*TILE_SIZE, 191);

                    if (map[0][1] == 0)
                        draw_lit_rle_sprite(bmp, data[floor].dat, 0, 0, 191);
                }
                /* but in the editor */
                else
                    /* draw a black rectangle */
                    rectfill(bmp, i*TILE_SIZE, j*TILE_SIZE, i*TILE_SIZE+TILE_SIZE, j*TILE_SIZE+TILE_SIZE, 0);

                /* later draw the player, enemy, key or door on top of the floor tile or black rectangle */
                if ((tile == PACIANA_FRONT) || (tile == MUMMY_FRONT) || (tile == KEY) || (tile == DOOR) || (tile == STONE) || (tile == DYNAMITE))
                    draw_rle_sprite(bmp, data[tile].dat, i*TILE_SIZE, j*TILE_SIZE);
                else if (tile >= SCARAB1 && tile <= SCARAB4)
                {
                    int angle = (tile-SCARAB1)*64;
                    rotate_sprite(bmp, data[SCARAB1].dat, i*TILE_SIZE, j*TILE_SIZE, itofix(angle));
                }
            }
            /* if it is a usual wall tile though */
            else
            {
                /* just draw this one then */
                draw_rle_sprite(bmp, data[tile].dat, i*TILE_SIZE, j*TILE_SIZE);
            }
        }
    }
}

void find_maps()
{
    int i=0;
    memset(map_files, 0, sizeof(map_files));
    struct al_ffblk info;
    if (al_findfirst("maps/*.map", &info, FA_ALL) == 0)
    {
        do
        {
            /* Do something useful here with info.name. */
            strcpy(map_files[i++], info.name);
        }
        while (al_findnext(&info) == 0 && i < MAX_MAP_FILES);

        al_findclose(&info);
    }

    num_mapfiles = i;
}
