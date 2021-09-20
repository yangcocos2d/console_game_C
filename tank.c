/*
    C����̹�˴�ս��Ϸ
    ������:VC6.0
    ���л���:Windows10������̨�ַ�����
    ʵ��ԭ��:
    1.ͼ����ײ���:��ά����Ԫ��֮����߼�����
    2.���̹�˹���:������������λ��ɾ��������
    3.��Ļˢ��:˫������ƣ��޿��١�
*/
#include<stdio.h>
#include<windows.h>
#include<conio.h>
#include<time.h>

#define cOk         1
#define cFail       0

#include<WinUser.h>     //Ϊ�˿���ʹ��GetAsyncKeyState����
#pragma comment(lib,"User32.lib")

//�û��������� ��������
char ui_key_is_pressed(char Key)
{
    if(GetAsyncKeyState(Key)&0x8000)
        return 1;//��������
    else 
        return 0;//û��
}

//�û��������� ��Ļ���
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

//��Ϸͼ��
char map_all_tank[SCREEN_MAX_X][SCREEN_MAX_Y]       = {0};
char map_player_bullet[SCREEN_MAX_X][SCREEN_MAX_Y]  = {0};
char map_enemy_bullet[SCREEN_MAX_X][SCREEN_MAX_Y]   = {0};
char map_background[SCREEN_MAX_X][SCREEN_MAX_Y]     = {0};
char map_boom_art[SCREEN_MAX_X][SCREEN_MAX_Y]       = {0};
char map_screen_display[SCREEN_MAX_X][SCREEN_MAX_Y] = {0};

//̹��
typedef struct
{
    int x;
    int y;
    int life;
    char (*Icon)[WIDTH_ICON];
}STTank;

//̹���ĸ������Сͼ��
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

//�ӵ��ĸ������ͼ��
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

//��ըͼ
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

//��һ����ά���� char map[SCREEN_MAX_X][SCREEN_MAX_Y] ÿ��Ԫ�ض����ַ���ʽ��ӡ����
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

//��һ��ͼ�긴�Ƶ�ͼ��
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

//��һ��ͼ���ͼ�����
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

//�ص����:ʵ���Ͼ�����PlaneIcon����x,y��λ�ú�GameMap���жԱȣ����ĳ�������£�PlaneIcon��GameMap���зǿ�Ԫ�أ������PlaneIcon��GameMap�������ص�
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

//�ҵ�����ӵ������ƶ�һ��
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

//�ҵ�����ӵ������ƶ�һ��
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
#define GAME_TIMR_LOOP               1000//�ɵ�����Ϸ��ִ���ٶ�

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

    //����ǽ��
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

    //������ѭ��
    while(1)
    {
        //�������ƽ�
        screen_refresh_cnt++;
        game_refresh_cnt++;
        enemy_tank_alive_cnt++;

        if(game_refresh_cnt>GAME_TIMR_LOOP)
        {
            game_refresh_cnt = 0;

            {//�ҷ�̹�˲���
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

            if(ui_key_is_pressed('J'))    //�����ӵ�
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

        {//�з�̹�˲��� ��������ENEMY_NUM��̹��
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

                    if(enemy_auto == 5)    //�����ӵ�
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

        {    //�ҷ��ӵ��ƶ�
            map_bullet_up(map_player_bullet);
            map_bullet_down(map_player_bullet);
            map_bullet_left(map_player_bullet);
            map_bullet_right(map_player_bullet);

            //�з��ӵ��ƶ�
            map_bullet_up(map_enemy_bullet);
            map_bullet_down(map_enemy_bullet);
            map_bullet_left(map_enemy_bullet); 
            map_bullet_right(map_enemy_bullet);

            //����˫���ӵ�����
            for(i=0;i<SCREEN_MAX_X;i++)
                for(j=0;j<SCREEN_MAX_Y;j++)
                {
                    if(map_enemy_bullet[i][j]!=' ' && map_player_bullet[i][j]!=' ')
                    {
                        map_enemy_bullet[i][j] = map_player_bullet[i][j] = ' ';
                    }
                }
        }

        {//����ש�����ӵ���ײ���
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

        {//��Ч����
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

        //��Ļˢ��
        if(screen_refresh_cnt > GAME_TIMR_LOOP)
        {
            screen_refresh_cnt = 0;
            //ͼ��ϳ�
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
            //��GameWorld������߿�
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

        //�з���������
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

        //��Ϸ�����ж�
        if(tank_player->life <= 0)
        {
            printf("Game Over!");
            break;
        }
    }

    return 0;
}