/*
    C语言坦克大战游戏
    编译器:VC6.0
    运行环境:Windows10，控制台字符界面
    实现原理:
    1.图像碰撞检测:二维数组元素之间的逻辑运算
    2.多个坦克管理:带新增和任意位置删除的数组
    3.屏幕刷新:双缓冲设计，无卡顿。
*/
#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>

#define cOk         1
#define cFail       0

#include<WinUser.h>     //为了可以使用GetAsyncKeyState函数
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
#define WIDTH_ICON      12
#define HIGH_ICON       6

//游戏图层
char map_all_tank[SCREEN_MAX_X][SCREEN_MAX_Y]       = {0};
char map_player_bullet[SCREEN_MAX_X][SCREEN_MAX_Y]  = {0};
char map_enemy_bullet[SCREEN_MAX_X][SCREEN_MAX_Y]   = {0};
char map_background[SCREEN_MAX_X][SCREEN_MAX_Y]     = {0};
char map_boom_art[SCREEN_MAX_X][SCREEN_MAX_Y]       = {0};
char map_screen_display[SCREEN_MAX_X][SCREEN_MAX_Y] = {0};

//坦克
typedef struct
{
    int x;
    int y;
    int life;
    char (*Icon)[WIDTH_ICON];
}STTank;

//坦克四个方向的小图标
char icon_tank_dir_up[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"    |      ",
"  :-+-:    ",
"  : 0 :    ",
"  :---:    ",
"           ",
};

char icon_tank_dir_down[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"           ",
"  :---:    ",
"  : 0 :    ",
"  :-+-:    ",
"    |      ",
};

char icon_tank_dir_left[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"           ",
"  '''''    ",
"-+| 0 |    ",
"  .....    ",
"           ",
};

char icon_tank_dir_right[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"           ",
"  '''''    ",
"  | 0 |+-  ",
"  .....    ",
"           ",
};

//子弹四个方向的图标
char icon_bullet_dir_up[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"    A      ",
"           ",
"           ",
"           ",
"           ",
};

char icon_bullet_dir_down[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"           ",
"           ",
"           ",
"           ",
"    V      ",
};

char icon_bullet_dir_left[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"           ",
"           ",
" <         ",
"           ",
"           ",
};

char icon_bullet_dir_right[HIGH_ICON][WIDTH_ICON] = 
{
"           ",
"           ",
"           ",
"        >  ",
"           ",
"           ",
};

//爆炸图
char icon_boom[HIGH_ICON][WIDTH_ICON] = 
{
"           ",/*#->*->O->o->.-> */
"           ",
"    #      ",
"  #####    ",
"    #      ",
"           ",
};

void map_fill_map(char map[SCREEN_MAX_X][SCREEN_MAX_Y])
{    int i,j;
    for(i=0;i<SCREEN_MAX_X;i++)
    {
        for(j=0;j<SCREEN_MAX_Y;j++)
            map[i][j]= ' ';
    }
}

//把一个二维数组 char map[SCREEN_MAX_X][SCREEN_MAX_Y] 每个元素都以字符形式打印出来
void map_print_map(char map[SCREEN_MAX_X][SCREEN_MAX_Y]) 
{
    int i,j;
    for(i=0;i<SCREEN_MAX_X;i++)
    {
        for(j=0;j<SCREEN_MAX_Y;j++)
        {
            ui_putchar(map[i][j]);
        }
        ui_putchar('\n');
    }
}

//把一个图标复制到图层
void map_write_icon(char map[SCREEN_MAX_X][SCREEN_MAX_Y],char Icon[HIGH_ICON][WIDTH_ICON],int x,int y)
{
    int i,j;
    for(i=0;i<HIGH_ICON;i++)
        for(j=0;j<WIDTH_ICON-1;j++)
        {
            if(Icon[i][j]!=' ' && x+i<SCREEN_MAX_X  &&  y+j<SCREEN_MAX_Y)                                              
            {
                map[x+i][y+j] =  Icon[i][j];
            }
            
        }
}

//把一个图标从图层擦除
void map_erase_icon(char map[SCREEN_MAX_X][SCREEN_MAX_Y],char Icon[HIGH_ICON][WIDTH_ICON],int x,int y)
{
    int i,j;
    for(i=0;i<HIGH_ICON;i++)
        for(j=0;j<WIDTH_ICON-1;j++)
        {
            if(Icon[i][j]!=' ' && x+i<SCREEN_MAX_X  &&  y+j<SCREEN_MAX_Y)                                          
            {
                map[x+i][y+j] =  ' ';
            }
            
        }
}

//重叠检测:实际上就是拿PlaneIcon，在x,y的位置和GameMap进行对比；如果某个坐标下，PlaneIcon和GameMap都有非空元素，则代表PlaneIcon与GameMap发送了重叠
char map_over_check(char map[SCREEN_MAX_X][SCREEN_MAX_Y],char Icon[HIGH_ICON][WIDTH_ICON],int x,int y)
{
    int i,j;
    for(i=0;i<HIGH_ICON;i++)
        for(j=0;j<WIDTH_ICON-1;j++)
        {
            if(Icon[i][j]!=' ' && x+i<SCREEN_MAX_X  &&  y+j<SCREEN_MAX_Y)                        
            {
                if( map[x+i][y+j] !=' ')
                {
                     
                    return cOk;
                    
                }
            }
            
        }
        return cFail;
}

//找到玩家子弹，并移动一格
void  map_bullet_up(char map[SCREEN_MAX_X][SCREEN_MAX_Y])
{
    int i,j;
    for(i=0;i<SCREEN_MAX_X;i++)
        for(j=0;j<SCREEN_MAX_Y;j++)
        {
            if(map[i][j] == 'A')
            {
                map[i][j] = ' ';
                if( i-1 >=0)
                    {
                        map[i -1][j] = 'A';   
                    }
                        
            }
        }
}

//找到玩家子弹，并移动一格
void  map_bullet_down(char map[SCREEN_MAX_X][SCREEN_MAX_Y])
{
    int i,j;
    for(i=SCREEN_MAX_X-1;i>=0;i--)
        for(j=0;j<SCREEN_MAX_Y;j++)
        {
            if(map[i][j] == 'V')
            {
                map[i][j] = ' ';
                if( i+1 <SCREEN_MAX_X)
                    {
                        map[i +1][j] = 'V';
                    }
                        
            }
        }
}

void  map_bullet_left(char map[SCREEN_MAX_X][SCREEN_MAX_Y])
{
    int i,j;
    for(i=0;i<SCREEN_MAX_X;i++)
        for(j=0;j<SCREEN_MAX_Y;j++)
        {
            if(map[i][j] == '<')
            {
                map[i][j] = ' ';
                if(j-1 >=0)
                    {
                        map[i ][j-1] = '<';
                    }
            }
        }
}

void  map_bullet_right(char map[SCREEN_MAX_X][SCREEN_MAX_Y])
{
    int i,j;
    for(i=0;i<SCREEN_MAX_X;i++)
        for(j=SCREEN_MAX_Y-1;j>=0;j--)
        {
            if(map[i][j] == '>')
            {
                map[i][j] = ' ';
                if( j+1 <SCREEN_MAX_Y)
                    {
                        map[i][j+1] = '>';
                    }
            }
        }
}

#define GAME_ENEMY_NUM               5
#define GAME_TIMR_LOOP               1000//可调节游戏的执行速度

STTank tank_player_object = {SCREEN_MAX_X-HIGH_ICON,(SCREEN_MAX_Y-WIDTH_ICON)/2,10,icon_tank_dir_up};
STTank *tank_player = &tank_player_object;
STTank tank_list_enemy[GAME_ENEMY_NUM] = 
{   {0,0,1,icon_tank_dir_down},
    {0,15,1,icon_tank_dir_down},
    {0,30,1,icon_tank_dir_down},
    {0,45,1,icon_tank_dir_down},
    {0,60,1,icon_tank_dir_down}
};

STTank *tank_enemy = &tank_list_enemy[0];

int main()
{
    int i,j; 
    int screen_refresh_cnt = 0;
    int game_refresh_cnt   = 0;
    int enemy_tank_alive_cnt= 0;
    int enemy_auto = 0;

    srand(time(NULL));
    map_fill_map(map_all_tank);
    map_fill_map(map_player_bullet);
    map_fill_map(map_enemy_bullet);
    map_fill_map(map_background);
    map_fill_map(map_boom_art);
    map_fill_map(map_all_tank);

    //建设墙壁
    for(i =10 ;i <20;i++)
        for(j = 0;j<5;j++)
        {
            map_background[i][j] = 'Z';
        }

    for(i =8 ;i <13;i++)
        for(j = 40;j<62;j++)
        {
            map_background[i][j] = '#';
        }

    //主任务循环
    while(1)
    {
        //计数器推进
        screen_refresh_cnt++;
        game_refresh_cnt++;
        enemy_tank_alive_cnt++;

        if(game_refresh_cnt>GAME_TIMR_LOOP)
        {
            game_refresh_cnt = 0;

            {//我方坦克操作
            STTank TankSave = *tank_player;

            if(tank_player->life != 0)
            {
            if(ui_key_is_pressed('W'))
            {
                if(tank_player->x>0) tank_player->x--;
                tank_player->Icon = icon_tank_dir_up;
            }
            if(ui_key_is_pressed('S'))
            {
                if(tank_player->x<SCREEN_MAX_X -HIGH_ICON) tank_player->x++;
                tank_player->Icon = icon_tank_dir_down;
            }
            if(ui_key_is_pressed('A'))
            {
                if(tank_player->y>0) tank_player->y--;
                if(tank_player->y>0) tank_player->y--;
                tank_player->Icon = icon_tank_dir_left;
            }
            if(ui_key_is_pressed('D'))
            {
                if(tank_player->y<SCREEN_MAX_Y -WIDTH_ICON+2) tank_player->y++;
                if(tank_player->y<SCREEN_MAX_Y -WIDTH_ICON+2) tank_player->y++;
                tank_player->Icon = icon_tank_dir_right;
            }

            map_erase_icon(map_all_tank,TankSave.Icon,TankSave.x,TankSave.y);
            if( map_over_check(map_all_tank,tank_player->Icon,    tank_player->x,tank_player->y)  ||
                map_over_check(map_background,tank_player->Icon,    tank_player->x,tank_player->y)
                )
            {
                
                *tank_player = TankSave;
                 
            }
            else
            {
                if(cOk == map_over_check(map_enemy_bullet,tank_player->Icon,    tank_player->x,tank_player->y))
                {
                    tank_player->life = tank_player->life - 1;
                    map_erase_icon(map_enemy_bullet,tank_player->Icon,tank_player->x,tank_player->y);
                }
            }

            if(tank_player->life != 0)
            {
                map_write_icon(map_all_tank,tank_player->Icon,tank_player->x,tank_player->y);
            }
            else
            {
                map_erase_icon(map_all_tank,tank_player->Icon,tank_player->x,tank_player->y);
                map_write_icon(map_boom_art,icon_boom,tank_player->x,tank_player->y);    
            }

            if(ui_key_is_pressed('J'))    //发射子弹
            {
                char (*Bullet)[WIDTH_ICON];
                if(tank_player->Icon == icon_tank_dir_up)       Bullet = icon_bullet_dir_up;
                if(tank_player->Icon == icon_tank_dir_down)     Bullet = icon_bullet_dir_down;
                if(tank_player->Icon == icon_tank_dir_left)     Bullet = icon_bullet_dir_left;
                if(tank_player->Icon == icon_tank_dir_right)    Bullet = icon_bullet_dir_right;

                map_write_icon( map_player_bullet, Bullet,tank_player->x,tank_player->y);
            }
         }
        }

        {//敌方坦克操作 连续操作ENEMY_NUM个坦克
            for(i=0;i<GAME_ENEMY_NUM;i++)
            {    STTank TankSave;
                tank_enemy = &tank_list_enemy[i];

                TankSave = *tank_enemy;
                game_refresh_cnt = 0;
                if(tank_enemy->life != 0)
                {
                    enemy_auto = 0;

                    enemy_auto = rand()%10;
                    if(enemy_auto == 1)
                    {
                        if(tank_enemy->x>0) tank_enemy->x--;
                        tank_enemy->Icon = icon_tank_dir_up;
                    }
                    if(enemy_auto == 2)
                    {
                        if(tank_enemy->x<SCREEN_MAX_X -HIGH_ICON) tank_enemy->x++;
                        tank_enemy->Icon = icon_tank_dir_down;
                    }
                    if(enemy_auto == 3)
                    {
                        if(tank_enemy->y>0) tank_enemy->y--;
                        if(tank_enemy->y>0) tank_enemy->y--;
                        tank_enemy->Icon = icon_tank_dir_left;
                    }
                    if(enemy_auto == 4)
                    {
                        if(tank_enemy->y<SCREEN_MAX_Y -WIDTH_ICON+2) tank_enemy->y++;
                        if(tank_enemy->y<SCREEN_MAX_Y -WIDTH_ICON+2) tank_enemy->y++;
                        tank_enemy->Icon = icon_tank_dir_right;
                    }


                    map_erase_icon(map_all_tank,TankSave.Icon,TankSave.x,TankSave.y);
                    if( map_over_check(map_all_tank,tank_enemy->Icon,    tank_enemy->x,tank_enemy->y)  ||
                        map_over_check(map_background,tank_enemy->Icon,    tank_enemy->x,tank_enemy->y)
                        )
                    {
                    
                        *tank_enemy = TankSave;
                     
                    }
                    else
                    {
                        if(cOk == map_over_check(map_player_bullet,tank_enemy->Icon,    tank_enemy->x,tank_enemy->y))
                        {
                            tank_enemy->life = tank_enemy->life - 1;
                            map_erase_icon(map_player_bullet,tank_enemy->Icon,tank_enemy->x,tank_enemy->y);
                        }
                    }

                    if(tank_enemy->life != 0)
                    {
                        map_write_icon(map_all_tank,tank_enemy->Icon,tank_enemy->x,tank_enemy->y);
                    }
                    else
                    {
                        map_erase_icon(map_all_tank,tank_enemy->Icon,tank_enemy->x,tank_enemy->y);
                        map_write_icon(map_boom_art,icon_boom,tank_enemy->x,tank_enemy->y);    
                    }

                    if(enemy_auto == 5)    //发射子弹
                    {      
                        char (*Bullet)[WIDTH_ICON];
                 
                        if(tank_enemy->Icon == icon_tank_dir_up)    Bullet = icon_bullet_dir_up;
                        if(tank_enemy->Icon == icon_tank_dir_down) Bullet = icon_bullet_dir_down;
                        if(tank_enemy->Icon == icon_tank_dir_left) Bullet = icon_bullet_dir_left;
                        if(tank_enemy->Icon == icon_tank_dir_right)Bullet = icon_bullet_dir_right;

                        map_write_icon( map_enemy_bullet, Bullet,tank_enemy->x,tank_enemy->y);
                    }
                }
            }
        }

        {    //我方子弹移动
            map_bullet_up(map_player_bullet);
            map_bullet_down(map_player_bullet);
            map_bullet_left(map_player_bullet);
            map_bullet_right(map_player_bullet);

            //敌方子弹移动
            map_bullet_up(map_enemy_bullet);
            map_bullet_down(map_enemy_bullet);
            map_bullet_left(map_enemy_bullet); 
            map_bullet_right(map_enemy_bullet);

            //敌我双方子弹抵消
            for(i=0;i<SCREEN_MAX_X;i++)
                for(j=0;j<SCREEN_MAX_Y;j++)
                {
                    if(map_enemy_bullet[i][j]!=' ' && map_player_bullet[i][j]!=' ')
                    {
                        map_enemy_bullet[i][j] = map_player_bullet[i][j] = ' ';
                    }
                }
        }

        {//背景砖块与子弹碰撞检测
            for(i=0;i<SCREEN_MAX_X;i++)
                for(j=0;j<SCREEN_MAX_Y;j++)
                {
                    if(map_background[i][j]!=' ')
                    {
                        if(map_player_bullet[i][j]!=' ')
                        {        
                            map_player_bullet[i][j] = ' ';
                            map_background[i][j]= ' ';
                            map_boom_art[i][j]= '#';
                        }
                        if(map_enemy_bullet[i][j]!=' ')
                        {        
                            map_enemy_bullet[i][j] = ' ';
                            map_background[i][j]= ' ';
                            map_boom_art[i][j]= '#';
                        }
                        
                    }
                }
        }

        {//特效处理
        for(i=0;i<SCREEN_MAX_X;i++)
            for(j=0;j<SCREEN_MAX_Y;j++)
            {
                if(map_boom_art[i][j] == '#')
                        map_boom_art[i][j] = '*';
                else if(map_boom_art[i][j] == '*')
                        map_boom_art[i][j] = 'O';
                else if(map_boom_art[i][j] == 'O')
                        map_boom_art[i][j] = 'o';
                else if(map_boom_art[i][j] == 'o')
                        map_boom_art[i][j] = '.';
                else if(map_boom_art[i][j] == '.')
                        map_boom_art[i][j] = ' ';
            }
        }
        }

        //屏幕刷新
        if(screen_refresh_cnt > GAME_TIMR_LOOP)
        {
            screen_refresh_cnt = 0;
            //图层合成
            {
                for(i=0;i<SCREEN_MAX_X;i++)
                {
                    for(j=0;j<SCREEN_MAX_Y;j++)
                    {
                        map_screen_display[i][j] = ' ';
                        if(map_player_bullet[i][j] != ' ')  map_screen_display[i][j] =  map_player_bullet[i][j];
                        if(map_enemy_bullet[i][j] != ' ')   map_screen_display[i][j] =  map_enemy_bullet[i][j];
                        if(map_all_tank[i][j] != ' ')       map_screen_display[i][j] =  map_all_tank[i][j];
                        if(map_background[i][j] != ' ')     map_screen_display[i][j] =  map_background[i][j];
                        if(map_boom_art[i][j] != ' ')       map_screen_display[i][j] =  map_boom_art[i][j];
                    }
                }
            }
            //给GameWorld加上外边框
            for(i=0;i<SCREEN_MAX_X;i++)
            {
                map_screen_display[i][0] = '.';
                map_screen_display[i][SCREEN_MAX_Y-1] = '.';
            }
            for(i=0;i<SCREEN_MAX_Y;i++)
            {
                map_screen_display[0][i] = '-';
                map_screen_display[SCREEN_MAX_X-1][i] = '-';
            }
            ui_set_xy(0,0);
            map_print_map(map_screen_display);
        }

        //敌方死而复生
        if(enemy_tank_alive_cnt > GAME_TIMR_LOOP*10)
        {
            enemy_tank_alive_cnt = 0;
            for(i=0;i<GAME_ENEMY_NUM;i++)
            {
                if(tank_list_enemy[i].life <= 0)
                {
                    tank_list_enemy[i].life = 1;
                    tank_list_enemy[i].x=0;
                    tank_list_enemy[i].y=15*i;
                }
            }
        }

        //游戏结束判断
        if(tank_player->life <= 0)
        {
            printf("Game Over!");
            break;
        }
    }

    return 0;
}