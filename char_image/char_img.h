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

struct st_spirit
{
    struct st_point pos;
    struct st_animation animation;
    void *user_data;
};

struct st_pocket
{
    void *pocket_start;
    void *pocket_end;
    void *pocket_curr;
    int   size_cell;
};

//GUI
extern void ui_init();
char ui_key_is_pressed(char Key);
//图像
extern void img_foreach(struct st_img *img,void (*every_pixel)(struct st_img *img,int i,int j,void *arg),void *arg);
extern void img_fill(struct st_img *img,char fill_char);
extern void img_print(struct st_img *img);
extern struct st_point img_check(struct st_img *img_back,struct st_img *img_icon,int img_icon_x,int img_icon_y);
extern int img_copy(struct st_img *img_src,struct st_img *img_dst);
extern void img_write(struct st_img *img_back,struct st_img *img_icon,int x,int y);
extern void img_erase(struct st_img *img_back,struct st_img *img_icon,int x,int y);
//动画
extern void animation_init(struct st_animation *p_animation);
extern void animation_reset(struct st_animation *p_animation);
extern void animation_add(struct st_animation *p_animation,struct st_img *p_img);
extern struct st_img *animation_next(struct st_animation *p_animation);
extern void animation_setcur(struct st_animation *p_animation,int img_cur);
struct st_img *animation_getcur(struct st_animation *p_animation);
//对象管理
extern void pocket_init(struct st_pocket *pocket,void * start,void *end,int size_cell);
extern void * pocket_getcur(struct st_pocket *pocket);
extern int pocket_num(struct st_pocket *pocket);
extern void pocket_foreach(struct st_pocket *pocket,void(*pfun)(void *p,int index,void *argv),void *argv);

#endif