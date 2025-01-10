#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

#include "game.h"
#include "map.h"
#include "lost.h"
#include "menu.h"
#include "anim.h"


void run_game()
{
    while (!done)
    {
        while (game_time > 0)
        {
//   poll_keyboard();
            al_poll_duh(dp);

            if (menu)
                _update_menu();
            else if (anim)
                update_anim();
            else
                update_game();

            game_time--;
        }

        if (volume[2] == 0 || frame_time > 0)
        {
            if (menu)
                draw_menu();
            else if (anim)
            {
                draw_anim();
            }
            else
                draw_game();

            frame_time = 0;
        }

        rest(1);
    }
}

int main(int argc, char *argv[])
{
    init_game();

    if (argc == 2)
    {
        printf("Playing map %s\n", argv[1]);
        play_map(argv[1]);
        plmap = TRUE;
    }

    run_game();

    exit_game();

    return 0;
}
END_OF_MAIN();

