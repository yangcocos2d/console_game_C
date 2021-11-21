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

int img_check(struct st_img *img1,struct st_img *img2)
{
    int h_min,w_min,i,j;
    h_min = img1->h < img2->h ? img1->h : img2->h;
    w_min = img1->w < img2->w ? img1->w : img2->w;

    for(j=0;j<h_min;j++)
        for(i=0;i<w_min;i++)
        {
            if( IMG_GET_PIXEL(img1,i,j)!=IMG_CHAR_NONE
                && IMG_GET_PIXEL(img2,i,j)!=IMG_CHAR_NONE)
            {
                return cTRUE;
            }
        }
    return cFALSE;
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

void main()
{
    int x=0,y=0;
    char back_buffer[20][80];


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
    "+--------+",
    "|    ||  |",
    "| ----   |",
    "||0000|  |",
    "| ----   |",
    "+--------+"};

    struct st_img img_back = {20,80,&back_buffer};
    struct st_img img_icon = {6,10,&none_buffer};


    ui_init();
    while(1)
    {

        img_fill(&img_back,' ');

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

        if(ui_key_is_pressed('L'))
        {
            img_roll_lr(&img_icon);
        }
        if(ui_key_is_pressed('U'))
        {
            img_roll_ud(&img_icon);
        }
        img_write(&img_back, &img_icon, x, y);
        img_print(&img_back);
    }
}
