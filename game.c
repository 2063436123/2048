#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

int a[4][4] = {
	0,
};
int empty; // 记录空格数量，满则结束游戏
int old_y, old_x;

void draw();				 // 用于绘制游戏界面
void play();				 // 游戏运行的逻辑主体
void init();				 // 初始化函数，用于完成一些必要的初始化操作
void draw_one(int y, int x); // 绘制单个数字
void cnt_value(int *new_y, int *new_x);
int game_over();		   // 结束游戏
int cnt_one(int y, int x); // 统计(y, x)对应的格子周围一圈的空格的个数

int main()
{
	init();
	play();
	endwin();
	return 0;
}

void init()
{
	int x, y;
	srand(time(0));
	x = rand() % 4;
	y = rand() % 4;
	// draw init gui
	initscr();
	cbreak();
	noecho();
	curs_set(0);

	empty = 15;
	a[y][x] = 2;
	draw();
}

void draw()
{
	int n, m, x, y;
	char c[4] = {'0', '0', '0', '0'};
	clear();

	for (n = 0; n < 9; n += 2)
		for (m = 0; m < 21; m++)
		{
			move(n, m);
			addch('-');
			refresh();
		}
	for (m = 0; m < 22; m += 5)
		for (n = 1; n < 8; n++)
		{
			move(n, m);
			addch('|');
			refresh();
		}
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
			draw_one(y, x);
}

void draw_one(int y, int x)
{
	int i, m, k, j;
	char c[5] = {0x00};
	i = a[y][x];
	m = 0;
	// c数组保存了i的数字倒序
	while (i > 0)
	{
		j = i % 10;
		c[m++] = j + '0';
		i = i / 10;
	}
	m = 0;
	k = (x + 1) * 5 - 1;
	// 书写i这个数字到屏幕上
	while (c[m] != 0)
	{
		move(2 * y + 1, k);
		addch(c[m++]);
		k--;
	}
}

void play()
{
	int x, y, i, new_x, new_y, temp;
	int old_empty, move;
	char ch;

	while (1)
	{
		move = 0;
		old_empty = empty;
		ch = getch();
		switch (ch)
		{
		case 97:  //左移  a
		case 104: // h
		case 68:  // 左移方向键
				  // 1. 合并相同且不被隔断的同行的数
			for (y = 0; y < 4; y++)
				for (x = 0; x < 4;)
				{
					if (a[y][x] == 0)
					{
						x++;
						continue;
					}
					else // find a number
					{
						for (i = x + 1; i < 4; i++)
						{
							if (a[y][i] == 0)
							{
								continue;
							}
							else // find second number
							{
								if (a[y][x] == a[y][i]) // equal -> combine
								{
									a[y][x] += a[y][i];
									a[y][i] = 0;
									empty++;
									break;
								}
								else // unequal -> break
								{
									break;
								}
							}
						}
						x = i; // continue find nubmers in the same row
					}
				}
			//  2. 将所有数向当前方向靠拢
			for (y = 0; y < 4; y++)
				for (x = 0; x < 4; x++)
				{
					if (a[y][x] == 0)
					{
						continue;
					}
					else // find a number
					{
						for (i = x; (i > 0) && (a[y][i - 1] == 0); i--)
						{
							a[y][i - 1] = a[y][i];
							a[y][i] = 0;
							move = 1;
						}
					}
				}
			break;
		case 100: //右移 d
		case 108: // l
		case 67:  //右移方向键
			for (y = 0; y < 4; y++)
				for (x = 3; x >= 0;)
				{
					if (a[y][x] == 0)
					{
						x--;
						continue;
					}
					else
					{
						for (i = x - 1; i >= 0; i--)
						{
							if (a[y][i] == 0)
							{
								continue;
							}
							else if (a[y][x] == a[y][i])
							{
								a[y][x] += a[y][i];
								a[y][i] = 0;
								empty++;
								break;
							}
							else
							{
								break;
							}
						}
						x = i;
					}
				}
			for (y = 0; y < 4; y++)
				for (x = 3; x >= 0; x--)
				{
					if (a[y][x] == 0)
					{
						continue;
					}
					else
					{
						for (i = x; (i < 3) && (a[y][i + 1] == 0); i++)
						{
							a[y][i + 1] = a[y][i];
							a[y][i] = 0;
							move = 1;
						}
					}
				}
			break;
		case 119: //上移 w
		case 107: //k
		case 65:  //上移方向键
			for (x = 0; x < 4; x++)
				for (y = 0; y < 4;)
				{
					if (a[y][x] == 0)
					{
						y++;
						continue;
					}
					else
					{
						for (i = y + 1; i < 4; i++)
						{
							if (a[i][x] == 0)
							{
								continue;
							}
							else if (a[y][x] == a[i][x])
							{
								a[y][x] += a[i][x];
								a[i][x] = 0;
								empty++;
								break;
							}
							else
							{
								break;
							}
						}
						y = i;
					}
				}
			for (x = 0; x < 4; x++)
				for (y = 0; y < 4; y++)
				{
					if (a[y][x] == 0)
					{
						continue;
					}
					else
					{
						for (i = y; (i > 0) && (a[i - 1][x] == 0); i--)
						{
							a[i - 1][x] = a[i][x];
							a[i][x] = 0;
							move = 1;
						}
					}
				}
			break;
		case 115: //下移 s
		case 106: //j
		case 66:  //下移方向键
			for (x = 0; x < 4; x++)
				for (y = 3; y >= 0;)
				{
					if (a[y][x] == 0)
					{
						y--;
						continue;
					}
					else
					{
						for (i = y - 1; i >= 0; i--)
						{
							if (a[i][x] == 0)
							{
								continue;
							}
							else if (a[y][x] == a[i][x])
							{
								a[y][x] += a[i][x];
								a[i][x] = 0;
								empty++;
								break;
							}
							else
							{
								break;
							}
						}
						y = i;
					}
				}
			for (x = 0; x < 4; x++)
				for (y = 3; y >= 0; y--)
				{
					if (a[y][x] == 0)
					{
						continue;
					}
					else
					{
						for (i = y; (i < 3) && (a[i + 1][x] == 0); i++)
						{
							a[i + 1][x] = a[i][x];
							a[i][x] = 0;
							move = 1;
						}
					}
				}
			break;
		case 'Q':
		case 'q':
			game_over();
			break;
		default:
			continue;
			break;
		}
		if (empty <= 0) // 全部填满->over
			game_over();
		if ((empty != old_empty) || (move == 1)) // 有合并产生
		{
			do
			{
				new_x = rand() % 4;
				new_y = rand() % 4;
			} while (a[new_y][new_x] != 0);

			cnt_value(&new_y, &new_x);

			do
			{
				temp = rand() % 4;
			} while (temp == 0 || temp == 2);
			a[new_y][new_x] = temp + 1; // 生成2或4（概率均等)
			empty--;
		}
		draw();
	}
}

int cnt_one(int y, int x)
{
	int value = 0;
	if (y - 1 >= 0 && a[y - 1][x] == 0)
		value++;
	if (y + 1 < 4 && a[y + 1][x] == 0)
		value++;
	if (x - 1 >= 0 && a[y][x - 1] == 0)
		value++;
	if (x + 1 < 4 && a[y][x + 1] == 0)
		value++;

	if (y - 1 >= 0 && x - 1 >= 0)
		a[y - 1][x - 1] ? 0 : value++;
	if (y - 1 >= 0 && x + 1 < 4)
		a[y - 1][x + 1] ? 0 : value++;
	if (y + 1 < 4 && x - 1 >= 0)
		a[y + 1][x - 1] ? 0 : value++;
	if (y + 1 < 4 && x + 1 < 4)
		a[y + 1][x + 1] ? 0 : value++;

	return value;
}

void cnt_value(int *new_y, int *new_x)
{
	int max_x, max_y, x, y, value;
	int max = 0;

	max = cnt_one(*new_y, *new_x);
	for (y = 0; y < 4; y++)
		for (x = 0; x < 4; x++)
		{
			// 如果(y, x)对应的空格为空
			if (!a[y][x])
			{
				// 优先选取周围空格最多的空格展示新数字
				value = cnt_one(y, x);
				if (value > max && old_y != y && old_x != x)// 如果没有value > max发生，那么将保持在第一次生成的位置产生新的数
				{
					// 避免在同一位置反复出现新数字
					*new_y = y;
					*new_x = x;
					old_x = x;
					old_y = y;
					break;
				}
			}
		}
}

int game_over()
{
    sleep(1);
    endwin();
    exit(0);
}