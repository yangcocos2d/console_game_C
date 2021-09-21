#include<stdio.h>
#include<windows.h>
#include<malloc.h>

#define cOk         1
#define cFail       0

#include<WinUser.h>                 //为了可以使用GetAsyncKeyState函数
#pragma comment(lib,"User32.lib")

//用户交互函数 按键输入 
char ui_key_is_pressed(char Key)
{
    if(GetAsyncKeyState(Key)&0x8000)
        return 1;//按键按下
    else 
        return 0;//没按
}

//用户交互函数 屏幕输出
void ui_putchar(char a)
{
    putchar(a);
}

void ui_set_xy(short x, short y) 
{    
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); 
    COORD coord = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

/*
O----->SCREEN_MAX_Y
|
|
|
v
SCREEN_MAX_X
*/
#define SCREEN_MAX_X    20
#define SCREEN_MAX_Y    70
#define SATURATION(x)   {if(x<0) x = 0;}

struct snake_node
{
    int x;
    int y;
};

#define     QUEUE_LEN       100
struct queue
{
    int head_index;
    int tail_index;
    void *queue_data[QUEUE_LEN];
};

void queue_init(struct queue *q)
{
    q->head_index=q->tail_index = 0;
}

void queue_circle_next(int *q_index)
{
    (*q_index)++;
    if((*q_index) >= QUEUE_LEN)
    {
        (*q_index) = 0;
    }
}

void queue_in(struct queue *q,void *new_data)
{
    queue_circle_next(&q->tail_index);

    if(q->tail_index == q->head_index)
    {
        queue_circle_next(&q->head_index);
    }
    q->queue_data[q->tail_index] = new_data;
}

void *queue_out(struct queue *q)
{
    if(q->head_index != q->tail_index)
    {
        queue_circle_next(&q->head_index);
        return q->queue_data[q->head_index];
    }
    else
    {
        return NULL;
    }
}

void queue_foreach(struct queue *q,void (*queue_node_do)(void *))
{
    int i = q->head_index;
    if(q->head_index != q->tail_index)
    {
        while(i!=q->tail_index)
        {
            queue_circle_next(&i);
            queue_node_do(q->queue_data[i]);
        }
    }
}

void snake_put_node(struct snake_node *node,char put_c)
{
    ui_set_xy(node->x,node->y);
    ui_putchar(put_c);

    //printf("i am node : %d %d\r\n",node->x,node->y);
}

void queue_node_do(void *queue_data_node)
{
    struct snake_node *node = (struct snake_node *)queue_data_node;
    snake_put_node(node,'O');
}

#define KEY_GET(x,key) {if(ui_key_is_pressed(key)) x = key;}
#define GAME_LOOP_COUNTER_MAX                   100000

int main(void)
{
    int i = 0;
    int game_loop_counter = 0;
    int game_snake_dir = 0;
    int  node_food_flash = 0;

    struct queue        *queue_snake = NULL;
    struct snake_node   *node_food = NULL;
    struct snake_node   *node_snake = NULL;
    struct snake_node   *node_snake_last = NULL;

    node_food = (struct queue *)malloc(sizeof(struct queue));
    queue_snake = (struct queue *)malloc(sizeof(struct queue));
    node_snake_last = (struct queue *)malloc(sizeof(struct queue));
    queue_init(queue_snake);

    
    for(i=0;i<5;i++)
   {
        //新增节点
        node_snake = (struct snake_node *)malloc(sizeof(struct snake_node));
        node_snake->x = i;
        node_snake->y = 0;
        queue_in(queue_snake,(void *)node_snake);

        //保存原有节点
        node_snake_last->x = node_snake->x;
        node_snake_last->y = node_snake->y;
    }
    node_food->x = rand()%40;
    node_food->y = rand()%20;
    snake_put_node(node_food,'*');

    while(1)
    {
        game_loop_counter++;
        /*蛇的移动*/
        if(game_loop_counter > GAME_LOOP_COUNTER_MAX)
        {
            game_loop_counter = 0;

            node_snake = (struct snake_node *)malloc(sizeof(struct snake_node));

            node_snake->x = node_snake_last->x;
            node_snake->y = node_snake_last->y;

            if(game_snake_dir == 'W')
            {
                node_snake->y--;
                SATURATION(node_snake->y);
            }
            else if(game_snake_dir == 'S')
            {
                node_snake->y++;
                SATURATION(node_snake->y);
            }
            else if(game_snake_dir == 'A')
            {
                node_snake->x--;
                SATURATION(node_snake->x);
            }
            else if(game_snake_dir == 'D')
            {
                node_snake->x++;
                SATURATION(node_snake->x);
            }
            else
            {
                node_snake->x++;
                SATURATION(node_snake->x);
            }
            queue_in(queue_snake,(void *)node_snake);
            node_snake_last->x = node_snake->x;
            node_snake_last->y = node_snake->y;

            if( node_snake_last->x == node_food->x 
                && node_snake_last->y == node_food->y )
            {
                node_food->x = rand()%40;
                node_food->y = rand()%20;
                
            }
            else
            {
                node_snake = queue_out(queue_snake);
                snake_put_node(node_snake,' ');
                free(node_snake);
            }

            if(node_food_flash == 0)
            {
                node_food_flash = 1;
                snake_put_node(node_food,'*');
            }
            else
            {   node_food_flash = 0;
                snake_put_node(node_food,'O');
            }
            queue_foreach(queue_snake,queue_node_do);
        }

        /*键盘控制方向*/
        KEY_GET(game_snake_dir,'W');
        KEY_GET(game_snake_dir,'S');
        KEY_GET(game_snake_dir,'A');
        KEY_GET(game_snake_dir,'D');
    }

    return 0;
}

