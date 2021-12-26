#include "char_img.h"
#include <stdio.h>
#include <windows.h>
#include <malloc.h>

#include <WinUser.h>                 //为了可以使用GetAsyncKeyState函数
#pragma comment(lib,"User32.lib")

#define cTRUE               1
#define cFALSE              0

//用户交互函数 按键输入
char ui_key_is_pressed(char Key)
{
    if(GetAsyncKeyState(Key)&0x8000)
        return cTRUE;//按键按下
    else
        return cFALSE;//没按
}

//用户交互函数 屏幕输出
void ui_putchar(char a)
{
    putchar(a);
}

/*
0------------>x,w,i
|
|
|
|
|
v
y,h,j

*/

static HANDLE handle;
void ui_init()
{
    CONSOLE_CURSOR_INFO cinfo;
    handle = GetStdHandle(STD_OUTPUT_HANDLE);

    cinfo.bVisible = 0;
    cinfo.dwSize = 1;

    SetConsoleCursorInfo(handle,&cinfo);
}

void ui_set_xy(short x, short y)
{
    COORD coord = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}
//------------------------------------------------------------------------
#define IMG_GET_PIXEL(img,i,j)  (*(img->buffer + (j)*(img->w) +(i)))
#define IMG_CHAR_NONE           (' ')

void img_foreach(struct st_img *img,void (*every_pixel)(struct st_img *img,int i,int j,void *arg),void *arg)
{
    int i,j;
    for(j=0;j<img->h;j++)
        for(i=0;i<img->w;i++)
        {
            every_pixel(img,i,j,arg);
        }
}

static void pixel_fill(struct st_img *img,int i,int j,void *arg)
{
    IMG_GET_PIXEL(img,i,j) = *(char *)arg;
}

void img_fill(struct st_img *img,char fill_char)
{
    img_foreach(img,pixel_fill,&fill_char);
}

static void pixel_put(struct st_img *img,int i,int j,void *arg)
{
    ui_set_xy(i,j);
    ui_putchar(IMG_GET_PIXEL(img,i,j));
}

void img_print(struct st_img *img)
{
     img_foreach(img,pixel_put,NULL);
}

void img_roll_lr(struct st_img *img)
{
    int i,j;

    for(j=0;j<img->h;j++)
        for(i=0;i<img->w / 2;i++)
        {
            char exchange_pixel = IMG_GET_PIXEL(img,i,j);
            IMG_GET_PIXEL(img,i,j) = IMG_GET_PIXEL(img,img->w - 1 - i,j);
            IMG_GET_PIXEL(img,img->w - 1 - i,j) = exchange_pixel;
        }
}

void img_roll_ud(struct st_img *img)
{
    int i,j;

    for(j=0;j<img->h / 2;j++)
        for(i=0;i<img->w;i++)
        {
            char exchange_pixel = IMG_GET_PIXEL(img,i,j);
            IMG_GET_PIXEL(img,i,j) = IMG_GET_PIXEL(img,   i,img->h - 1 - j);
            IMG_GET_PIXEL(img,i,img->h - 1 - j) = exchange_pixel;
        }
}

struct st_point img_check(struct st_img *img_back,struct st_img *img_icon,int img_icon_x,int img_icon_y)
{
    int i,j;
    struct st_point point_overlap = {-1,-1};

    for(j=0;j<img_icon->h;j++)
        for(i=0;i<img_icon->w;i++)
        {
            if(IMG_GET_PIXEL(img_icon,i,j) != IMG_CHAR_NONE)
            {
                if(IMG_GET_PIXEL(img_back,i+img_icon_x,j+img_icon_y)!= IMG_CHAR_NONE)
                {
                    point_overlap.x = i+img_icon_x;
                    point_overlap.y = j+img_icon_y;

                    return point_overlap;
                }
            }
        }

    return point_overlap;
}

int img_copy(struct st_img *img_src,struct st_img *img_dst)
{
    int i,j;
    if((img_src->h != img_dst->h)
        || (img_src->w != img_dst->w))
        {
            return cFALSE;
        }

    for(j=0;j<img_dst->h;j++)
        for(i=0;i<img_dst->w;i++)
        {
            IMG_GET_PIXEL(img_src,i,j) = IMG_GET_PIXEL(img_dst,i,j);
        }
       return cTRUE;
}

void img_write(struct st_img *img_back,struct st_img *img_icon,int x,int y)
{
    int i,j;

    for(j=0;j<img_icon->h;j++)
        for(i=0;i<img_icon->w;i++)
        {
            if(     (i + x < img_back->w)
               &&   (j + y < img_back->h)
               &&   (i + x >= 0)
               &&   (j + y >= 0)
               )
            {
                char pixel = IMG_GET_PIXEL(img_icon,i ,j );
                if(pixel != IMG_CHAR_NONE)
                {
                    IMG_GET_PIXEL(img_back, i + x, j + y) =pixel;
                }
            }
        }
}

void img_erase(struct st_img *img_back,struct st_img *img_icon,int x,int y)
{
    int i,j;

    for(j=0;j<img_icon->h;j++)
        for(i=0;i<img_icon->w;i++)
        {
            if(     (i + x < img_back->w)
               &&   (j + y < img_back->h)
               &&   (i + x >= 0)
               &&   (j + y >= 0)
                )
            {
                char pixel = IMG_GET_PIXEL(img_icon,i ,j );
                if(pixel != IMG_CHAR_NONE)
                {
                 IMG_GET_PIXEL(img_back, i+ x, j+ y) = IMG_CHAR_NONE;
                }
            }
        }
}

void animation_init(struct st_animation *p_animation)
{
    p_animation->img_num = 0;
}

void animation_reset(struct st_animation *p_animation)
{
    p_animation->img_cur = 0;
}

void animation_add(struct st_animation *p_animation,struct st_img *p_img)
{
    if(p_animation->img_num < ANIMATION_MAX_IMG)
    {
        p_animation->pimg[p_animation->img_num] = p_img;
        p_animation->img_num++;
    }
}

struct st_img *animation_next(struct st_animation *p_animation)
{
    struct st_img *ret = 0;

    if(p_animation->img_num == 0)
    {
        return 0;
    }

    if(p_animation->img_cur < p_animation->img_num)
    {
        ret = p_animation->pimg[p_animation->img_cur];
        p_animation->img_cur++;
        return ret;
    }
    else
    {
        p_animation->img_cur = 1;

        return p_animation->pimg[0];
    }
}

void animation_setcur(struct st_animation *p_animation,int img_cur)
{
    if(img_cur < p_animation->img_num)
    {
        p_animation->img_cur = img_cur;
    }
}

void main()
{
    int x=0,y=0;
    int i = 0;
    char screen_buffer[20][80];

    char back_buffer[20][80]={
    "                                       **                                       ",
    "                                                                                ",
    "                                                                                ",
    "                                                                                ",
    "                                        0000                                    ",
    "                                                                                ",
    "                                         9999                                   ",
    "                                                                                ",
    "                                                                                ",
    "                                                                                ",
    "                                                  444                           ",
    "                                                                                ",
    "                                                                                ",
    "                                                                                ",
    "                                                                                ",
    "                                                                                ",
    "                                    |||||||                                     ",
    "                                                                                ",
    "                                                                                ",
    "                                                                                ",
    };


    char none_buffer[6][10] = {
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "          "};
    char tank_buffer[6][10] = {
    "+--------+",
    "|12345678|",
    "| ^ ^    |",
    "|  -     |",
    "|        |",
    "+--------+"};

    char drum_buffer[6][10] = {
    "          ",
    "          ",
    "    +     ",
    "  +++++   ",
    "    +     ",
    "          "};

    struct st_img img_back = {20,80,&back_buffer};
    struct st_img img_icon = {6,10,&drum_buffer};
    struct st_img img_screen = {20,80,&screen_buffer}; //back层和icon层叠加起来的效果


    //测试动画
    char ani_img_1[6][10] = {
    "          ",
    "          ",
    "          ",
    "          ",
    "          ",
    "          "};
    char ani_img_2[6][10] = {
    "          ",
    "          ",
    "          ",
    "    +     ",
    "          ",
    "          "};
    char ani_img_3[6][10] = {
    "          ",
    "          ",
    "    +     ",
    "    +     ",
    "    +     ",
    "          "};
    char ani_img_4[6][10] = {
    "          ",
    "          ",
    "    +     ",
    "   +++    ",
    "    +     ",
    "          "};
    char ani_img_5[6][10] = {
    "          ",
    "          ",
    "    +     ",
    "  +++++   ",
    "    +     ",
    "          "};
    char ani_img_6[6][10] = {
    "          ",
    "    +     ",
    "    +     ",
    "  +++++   ",
    "    +     ",
    "    +     "};
    char ani_img_7[6][10] = {
    "          ",
    "    +     ",
    "    +     ",
    " +++++++  ",
    "    +     ",
    "    +     "};
    char ani_img_8[6][10] = {
    "          ",
    "    +     ",
    "    +     ",
    "++++ ++++ ",
    "    +     ",
    "    +     "};
    char ani_img_9[6][10] = {
    "          ",
    "    +     ",
    "  + + +   ",
    " ++   ++  ",
    "  + + +   ",
    "    +     "};
    char ani_img_10[6][10] = {
    "          ",
    "    +     ",
    "  +   +   ",
    " +     +  ",
    "  +   +   ",
    "    +     "};

    struct st_animation ani_test ;

    struct st_img img_ani[10] =
    {
    {6,10,&ani_img_1},
    {6,10,&ani_img_2},
    {6,10,&ani_img_3},
    {6,10,&ani_img_4},
    {6,10,&ani_img_5},
    {6,10,&ani_img_6},
    {6,10,&ani_img_7},
    {6,10,&ani_img_8},
    {6,10,&ani_img_9},
    {6,10,&ani_img_10},
    };

    animation_init(&ani_test);
    animation_reset(&ani_test);

    for(i=0;i<10;i++)
    {
        animation_add(&ani_test,&img_ani[i]);
    }

    ui_init();
    while(1)
    {
        img_fill(&img_screen, ' ');
        if(ui_key_is_pressed('A'))
        {
            x--;
        }
        if(ui_key_is_pressed('D'))
        {
            x++;
        }
        if(ui_key_is_pressed('W'))
        {
            y--;
        }
        if(ui_key_is_pressed('S'))
        {
            y++;
        }

        if(ui_key_is_pressed('E'))
        {
            if(img_icon.buffer == &none_buffer)
            {
                img_icon.buffer = &drum_buffer;
            }
            if(img_icon.buffer == &drum_buffer)
            {
                img_icon.buffer = &tank_buffer;
            }
            else
            {
                img_icon.buffer = &drum_buffer;
            }
        }

        if(ui_key_is_pressed('M'))
        {   //test animation
            img_icon = *animation_next(&ani_test);
        }
        if(ui_key_is_pressed('L'))
        {
            img_roll_lr(&img_icon);
        }
        if(ui_key_is_pressed('U'))
        {
            img_roll_ud(&img_icon);
        }

         //if(ui_key_is_pressed('C'))
        {
            struct st_point point = img_check(&img_back,&img_icon,x,y);
            if(point.x == -1 && point.y == -1)
            {
                printf("                                     ");
            }
            else
            {
                printf("overlap: x = %d y = %d",point.x,point.y);
            }
        }

        img_write(&img_screen, &img_back, 0, 0);
        img_write(&img_screen, &img_icon, x, y);


        img_print(&img_screen);
    }
}
