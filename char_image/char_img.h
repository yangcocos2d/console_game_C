#ifndef __CHAR_IMG_H__
#define __CHAR_IMG_H__

struct st_img
{
    int h;
    int w;
    char *buffer;
};

#define ANIMATION_MAX_IMG   10
struct st_animation
{
    struct st_img* pimg[ANIMATION_MAX_IMG];
    int img_cur;
    int img_num;
};

struct st_point
{
    int x;
    int y;
};

#endif