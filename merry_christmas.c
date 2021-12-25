#include "char_img.h"
#include <math.h>

struct st_spirit
{
	struct st_img *img_icon;
	int x;
	int y;
};


#define NUM_TARGET   (3*9 + 5+4 + 2)
#define NUM_SPIRIT	 2

void main()
{
	unsigned int counter = 0;
	int steps = 0;
	int diff = 0;
	int direction = 0;
	char screen_buffer[40][80];
	char icon_buffer_1[3][5] = {
		"  +  ",
		"+++++",
		"  +  ",
	};
	char icon_buffer_2[3][5] = {
		"  #  ",
		"#####",
		"  #  ",
	};
	char icon_buffer_3[3][5] = {
		"  *  ",
		"*****",
		"  *  ",
	};
	char icon_buffer_4[3][5] = {
		"  -  ",
		"-----",
		"  -  ",
	};
	char target_buffer[3][5] = {
		".....",
		".....",
		".....",
	};
	struct st_img img_icon = { 3, 5, &icon_buffer_1 };
	struct st_img img_target = { 3, 5, &target_buffer };
	struct st_img img_screen = { 40, 80, &screen_buffer };


	//spirit_icon[i] trace spirit_target[i]

	struct st_spirit spirit_icon[NUM_TARGET][NUM_SPIRIT] =
	{	
		{
			{ &img_icon, 40, 0 },
			{ &img_icon, 40, 0 },
		},
	};
	struct st_spirit spirit_target[NUM_TARGET] =
	{	
		{ &img_target, 40, 0 },
		{ &img_target, 40, 0 },
		{ &img_target, 40, 0 },
		
		{ &img_target, 35, 3 },
		{ &img_target, 35, 3 },
		{ &img_target, 45, 3 },

		{ &img_target, 30, 6 },
		{ &img_target, 40, 6 },
		{ &img_target, 50, 6 },
		
		{ &img_target, 40, 9 },
		{ &img_target, 40, 9 },
		{ &img_target, 40, 9 },

		{ &img_target, 30, 12 },
		{ &img_target, 30, 12 },
		{ &img_target, 50, 12 },

		{ &img_target, 20, 15 },
		{ &img_target, 40, 15 },
		{ &img_target, 60, 15 },

		{ &img_target, 40, 18 },
		{ &img_target, 40, 18 },
		{ &img_target, 40, 18 },

		{ &img_target, 30, 21},
		{ &img_target, 40, 21},
		{ &img_target, 50, 21},

		{ &img_target, 20, 24 },
		{ &img_target, 40, 24 },
		{ &img_target, 60, 24 },

		{ &img_target, 10, 27 },
		{ &img_target, 20, 27 },
		{ &img_target, 30, 27 },
		{ &img_target, 40, 27 },
		{ &img_target, 50, 27 },
		{ &img_target, 60, 27 },
		{ &img_target, 70, 27 },

		{ &img_target, 40, 30 },
		{ &img_target, 40, 33 },
		{ &img_target, 40, 36 },
		{ &img_target, 40, 39 },

	};

	struct st_spirit *spirit_move;
	struct st_spirit *spirit_des;


	srand(time(0));

	for (int i = 0; i < NUM_TARGET; i++)
	{
		for (int j = 0; j < NUM_SPIRIT; j++)
		{
			if (spirit_icon[i][j].img_icon == 0)
			{
				spirit_icon[i][j].img_icon = spirit_icon[0][j].img_icon;
				spirit_icon[i][j].x = rand() % 80;
				spirit_icon[i][j].y = rand() % 40;
			}
			
		}

	}

	
	ui_init();
	img_fill(&img_screen, ' ');
	
	while (1)
	{
		counter++;
		//if (counter > 1)
		{
			counter = 0;
			for (int j = 0; j < NUM_TARGET; j++)
			{
				spirit_des = &spirit_target[j];
				
				for (int i = 0; i < NUM_SPIRIT; i++)
				{
					int p_x = 0;
					int p_y = 0;
					int icon_change = rand() % 5;

					spirit_move = &spirit_icon[j][i];

					///ÇĞ»»Í¼±ê
					if (icon_change == 1)
					{
						spirit_move->img_icon->buffer = icon_buffer_1;
					}
					if (icon_change == 2)
					{
						spirit_move->img_icon->buffer = icon_buffer_2;
					}
					if (icon_change == 3)
					{
						spirit_move->img_icon->buffer = icon_buffer_3;
					}
					if (icon_change == 4)
					{
						spirit_move->img_icon->buffer = icon_buffer_4;
					}
					 

					steps = rand() % 2;
					diff = abs(spirit_des->x - spirit_move->x) + abs(spirit_des->y - spirit_move->y);
					if (diff != 0)
					{
						p_x = abs(spirit_des->x - spirit_move->x) * 100 / diff;
						p_y = abs(spirit_des->y - spirit_move->y) * 100 / diff;

						direction = rand() % 100; //0~99
						if (direction < p_x)
						{
							if (spirit_des->x - spirit_move->x < 0)
							{
								spirit_move->x -= steps;
							}
							if (spirit_des->x - spirit_move->x > 0)
							{
								spirit_move->x += steps;
							}

						}
						else
						{
							if (spirit_des->y - spirit_move->y < 0)
							{
								spirit_move->y -= steps;
							}
							if (spirit_des->y - spirit_move->y > 0)
							{
								spirit_move->y += steps;
							}
						}
					}

					img_write(&img_screen, spirit_move->img_icon, spirit_move->x, spirit_move->y);
					//img_write(&img_screen, spirit_des->img_icon, spirit_des->x, spirit_des->y);

				}

				

				
			}

			img_print(&img_screen);

			for (int j = 0; j < NUM_TARGET; j++)
			{
				for (int i = 0; i < NUM_SPIRIT; i++)
				{
					spirit_move = &spirit_icon[j][i];
					img_erase(&img_screen, spirit_move->img_icon, spirit_move->x, spirit_move->y);

				}
				
			}
		}
	}
}