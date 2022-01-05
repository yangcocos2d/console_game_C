#include "char_img.h"

#define TANK_NUM    5
#define TANK_WIDTH  12
#define TANK_HIGH   6

char tank_buffer_up[TANK_HIGH][TANK_WIDTH] =
{

    "     ||     ",
    "  -++||++-  ",
    "  -  OO  -  ",
    "  -  OO  -  ",
    "  -||||||-  ",
    "            ",
};

char tank_buffer_down[TANK_HIGH][TANK_WIDTH] =
{
    "            ",
    "  -++||++-  ",
    "  -  OO  -  ",
    "  -  OO  -  ",
    "  -||||||-  ",
    "     ||     ",
};

char tank_buffer_left[TANK_HIGH][TANK_WIDTH] =
{
    "           ",
    "  | | | |  ",
    "-----OO +  ",
    "-----OO +  ",
    "  | | | |  ",
    "           ",
};

char tank_buffer_right[TANK_HIGH][TANK_WIDTH] =
{
    "           ",
    "  | | | |  ",
    "  + OO-----",
    "  + OO-----",
    "  | | | |  ",
    "           ",
};

#define SCREEN_WIDTH    80
#define SCREEN_HIGH     20

struct st_img img_screen;
char screen_buffer[SCREEN_HIGH][SCREEN_WIDTH] =
{
     "+----------------------------------------------------------------------------+",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "|                                                                            |",
     "+----------------------------------------------------------------------------+",
};

#define MOVE_UP         1
#define MOVE_DOWN       2
#define MOVE_LEFT       3
#define MOVE_RIGHT      4

void spirit_tank_foreach(void *p,int index,void *argv)
{
    struct st_spirit *tank = p;
    struct st_spirit tank_backup =*(struct st_spirit *)p;
    struct st_img *img_tank;
    struct st_point point_overlap = {-1,-1};
    char *param = argv;
    char move = 0;

    img_tank = animation_getcur(&tank_backup.animation);
    img_erase(&img_screen, img_tank, tank->pos.x, tank->pos.y);

    if (index == 0)
    {
        if(param[0] == 'W')
        {
            move = MOVE_UP;
        }
        if(param[0] == 'S')
        {
            move = MOVE_DOWN;
        }
        if(param[0] == 'A')
        {
            move = MOVE_LEFT;
        }
        if(param[0] == 'D')
        {
            move = MOVE_RIGHT;
        }

    }
    else
    {
        move = rand() % 10;
    }

    if (move == MOVE_UP)
    {
        tank_backup.pos.y--;
        animation_setcur(&tank_backup.animation, 0);
    }
    if (move == MOVE_DOWN)
    {
        animation_setcur(&tank_backup.animation, 1);
        tank_backup.pos.y++;
    }
    if (move == MOVE_LEFT)
    {
        animation_setcur(&tank_backup.animation, 2);
        tank_backup.pos.x--;
    }
    if (move == MOVE_RIGHT)
    {
        animation_setcur(&tank_backup.animation, 3);
        tank_backup.pos.x++;
    }

    img_tank = animation_getcur(&tank_backup.animation);

    point_overlap = img_check(&img_screen, img_tank, tank_backup.pos.x, tank_backup.pos.y);

    if(point_overlap.x == -1)
    {
        *tank = tank_backup;
    }
    else
    {

    }

    img_tank = animation_getcur(&tank->animation);
    img_write(&img_screen, img_tank, tank->pos.x, tank->pos.y);
}

int main()
{
    int i = 0;
    unsigned int sys_screen_counter = 0, sys_key_counter = 0;
    struct st_img img_tank_up,img_tank_down,img_tank_left,img_tank_right;
    struct st_animation animation_tank;
    struct st_spirit spirit_tank_array[TANK_NUM];
    struct st_pocket pocket_tank;

    ui_init();
    srand(time(0));

    img_tank_up.buffer = tank_buffer_up;
    img_tank_up.h = TANK_HIGH;
    img_tank_up.w = TANK_WIDTH;

    img_tank_down = img_tank_up;
    img_tank_down.buffer = tank_buffer_down;

    img_tank_left = img_tank_up;
    img_tank_left.buffer = tank_buffer_left;

    img_tank_right = img_tank_up;
    img_tank_right.buffer = tank_buffer_right;

    img_screen.buffer = screen_buffer;
    img_screen.h = SCREEN_HIGH;
    img_screen.w = SCREEN_WIDTH;

    animation_init(&animation_tank);
    animation_add(&animation_tank,&img_tank_up);
    animation_add(&animation_tank,&img_tank_down);
    animation_add(&animation_tank,&img_tank_left);
    animation_add(&animation_tank,&img_tank_right);

    for(i=0;i<TANK_NUM;i++)
    {
        spirit_tank_array[i].animation = animation_tank;
        spirit_tank_array[i].pos.x = i*SCREEN_WIDTH/TANK_NUM + 1;
        spirit_tank_array[i].pos.y = TANK_HIGH;
    }

    pocket_init(&pocket_tank,&spirit_tank_array[0],&spirit_tank_array[TANK_NUM-1],sizeof(struct st_spirit));

    while(1)
    {

        char KeyCode[5] =" ";
        sys_screen_counter++;
        sys_key_counter++;

        if (sys_key_counter > 100)
        {
            sys_key_counter = 0;

            if (ui_key_is_pressed('W'))
            {
                KeyCode[0] = 'W';
            }
            if (ui_key_is_pressed('S'))
            {
                KeyCode[0] = 'S';
            }
            if (ui_key_is_pressed('A'))
            {
                KeyCode[0] = 'A';
            }
            if (ui_key_is_pressed('D'))
            {
                KeyCode[0] = 'D';
            }
            pocket_foreach(&pocket_tank, spirit_tank_foreach, KeyCode);
            img_print(&img_screen);
        }

        if(sys_screen_counter > 1000)
        {
            sys_screen_counter = 0;
        }
    }

    return 0;
}
