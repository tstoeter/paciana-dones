/* map.h */

#ifndef MAP_H
#define MAP_H

#define TILE_SIZE       32

#define MAP_SIZE_X      20
#define MAP_SIZE_Y      15

#define NUM_LEVELS      10

#define MAP_ID  "pd"
#define MAP_VER  "1"

extern BITMAP *level_b;

extern unsigned char map[MAP_SIZE_X][MAP_SIZE_Y];
extern unsigned int num_keys;

extern unsigned int level;

extern unsigned int next_level;
extern unsigned int next_map;
extern unsigned int update_map;

extern unsigned char map_error[256];

#define MAX_MAP_FILES      256
extern unsigned char map_files[MAX_MAP_FILES][256];
extern unsigned int num_mapfiles;

int load_map(char *filename, unsigned char editor);
int load_map_from_datafile(void *dat);
int save_map(char *filename, unsigned char override);
void draw_map(BITMAP *bmp, int mx, int my, int mw, int mh, unsigned char editor);
int check_map();
void find_maps();

#endif

