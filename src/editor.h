/* editor.h */

#ifndef EDITOR_H
#define EDITOR_H

char editor_help[] = "Using the level editor is pretty easy, to select a tile press its corresponding key on the keyboard. Click the left mouse button to place a tile under the cursor or the right mouse button to delete a tile. The function keys can be used in their intended manner printed in the menu bar.\n\n"
                     "The level editor, originally being only a development tool, invokes a few pitfalls and rules one must know to create valid, working and playable maps.\n\n"
                     "First off, a valid map must have a closed border of tiles, to prevent the player and enemy from moving outside the map. The map may fill the whole screen or as well be smaller. The most upper left corner tile is used as reference for floor tiles and has to be set. Placing dynamite just below this floor reference tile, will make the map dark without light. Every map has to contain the player's initial positions, at least one key for the player to collect and one door which leads to the next level. Preferably no map should have a free walkable 2x2 or greater square of empty tiles and each wall tile should be directly connected to another one.\n\n"
                     "The behaviour of the mummy is rather simple, at every crossroads it checks for all free directions and randomly decides in which of these directions to go next. Plus the mummy moves two times slower than the player. After all the player's sight is very limited in dark maps and meeting the mummy can result in quite tough situations. To create a fair and playable map, it should contain some possibilities to avoid the mummy, i.e. there should be more than one path to collect all keys and reach the door to get to the next level and only few or short dead ends.\n\n"
                     "The scarabs only follow their initial direction and walk the opposite way, when they hit an obstacle. The rocks can be destroyed with previously collected dynamite or pushed further out of the way, but also into another way.\n\n"
                     "Still this map editor is a development tool, but following these few guidelines should lead to working and playable maps.\n\n"
                     "Happy editing!";

struct
{
    int x, y;
    int last_tile;
    int current_tile;
} editor;

extern DATAFILE *data;
extern BITMAP *buffer;

void draw_interface();
void init_editor();
int edit_map();
void run_editor();
void exit_editor();

#endif

