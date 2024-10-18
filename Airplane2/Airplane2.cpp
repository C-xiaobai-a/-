#include <easyx.h>
#include<stdio.h>
#include <conio.h>
#include<cstdlib>
#include <time.h> 
#include <windows.h>
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define VK_A 0x41
#define VK_D 0x44
#define VK_S 0x53
#define VK_W 0x57
#define VK_SPACE 0x20
//#include "AirPlay.h"

#define BGWEIGHT 400  //宽
#define BGHEIGHT 600  //高
//我方飞机的大小
#define myAirWEIGHT  25
#define myAirHEIGHT  40
//敌机的大小
#define enemyPlaneWEIGHT  25
#define enemyPlaneHEIGHT  25
//子弹的大小
#define BulletWEIGHT  8
#define BulletHEIGHT  8
//子弹速度
#define BulletSpeed  10
IMAGE img[5],img1[5];//图片数组

ExMessage msg;

//玩家状态
bool is_move_up = 0;
bool is_move_down = 0;
bool is_move_left = 0;
bool is_move_right = 0;
bool is_attack = 0;

struct Plance//定义飞机
{
	int x;
	int y;
	int live;//是否存活
}player;

struct Node
{
	int size;
	int x;
	int y;
	int speed;
	Node* next;
};

Node *PBulletHead, *EPlane, *EBullet;


void gameInit()//初始化数据
{
	player.x = BGWEIGHT / 2;
	player.y = BGHEIGHT - myAirHEIGHT;
	player.live = 1;
}

//输出透明背景贴图
void transparentimage(int x, int y, IMAGE img,IMAGE img1) {
	putimage(x, y, &img1, SRCAND);
	putimage(x, y, &img, SRCPAINT);
}


//初始化函数
void init()
{
	//加载图片
	loadimage(&img[0], "res\\背景.jpg", BGWEIGHT, BGHEIGHT);
	loadimage(&img[1], "res\\飞机.png", myAirWEIGHT, myAirHEIGHT);
	loadimage(&img[2], "res\\敌机.png", enemyPlaneWEIGHT, enemyPlaneHEIGHT);
	loadimage(&img[3], "res\\子弹.png", BulletWEIGHT, BulletHEIGHT);
	loadimage(&img[4], "res\\gameover.jpg", BGWEIGHT, BGHEIGHT);
	putimage(0, 0, &img[0]);
	putimage(BGWEIGHT / 2, BGHEIGHT - myAirHEIGHT, &img[1]);
	mciSendString("open res\\JustBlue.mp3 alias music", 0, 0, 0);
	mciSendString("play music repeat", 0, 0, 0);
	PBulletHead = NULL;
	EPlane = NULL;
	EBullet = NULL;
	srand(time(0));
	//进行三元光栅操作
	for (int i = 0; i <= 4; i++)
	{
		DWORD* d1;
		img1[i] = img[i];
		d1 = GetImageBuffer(&img1[i]);
		float h, s, l;
		for (int i = 0; i < img1[i].getheight() * img1[i].getwidth(); i++) {
			RGBtoHSL(BGR(d1[i]), &h, &s, &l);
			if (l < 0.03) {
				d1[i] = BGR(WHITE);
			}
			if (d1[i] != BGR(WHITE)) {
				d1[i] = 0;
			}
		}
	}
	
}

bool crash(int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2)
{
	if (abs( (x1 + w1 / 2)-(x2 + w2 / 2) ) < abs( (w1 + w2) / 2 ) && abs( (y1 + h1 / 2) -(y2 + h2 / 2) ) < abs( (h1 + h2) / 2 ) )
		return true;
	return false;
}

//生成对象
Node* createObject(int x, int y, int speed)
{
	Node* New = (Node*)malloc(sizeof(Node));
	New->x = x;
	New->y = y;
	New->speed = speed;
	New->next = NULL;
	return New;
}

void gameDraw()
{
	BeginBatchDraw();
	//飞机移动
	if (player.y >= BGHEIGHT - myAirHEIGHT)
		player.y = BGHEIGHT - myAirHEIGHT;
	if (player.y <= 0)
		player.y = 0;
	if (player.x >= BGWEIGHT - myAirWEIGHT)
		player.x = BGWEIGHT - myAirWEIGHT;
	if (player.x <= 0)
		player.x = 0;
	putimage(0, 0, &img[0]);
	transparentimage(player.x, player.y,img[1], img1[1]);
	//子弹移动
	for (Node *i = PBulletHead;i != NULL; i = i->next)
	{
		i->y -= i->speed;
		transparentimage(i->x, i->y, img[3], img1[3]);
	}
	//敌方飞机生成
	if(rand() % 30 <= 1)
	{
		Node* New = createObject(rand() % BGWEIGHT, 0, rand() % 10 + 5);
		if (EPlane == NULL)
			EPlane = New;
		else
		{
			New->next = EPlane;
			EPlane = New;
		}
	}

	//敌方飞机移动
	for (Node* i = EPlane; i != NULL; i = i->next)
	{
		i->y += i->speed;
		transparentimage(i->x, i->y, img[2], img1[2]);
	}

	//敌机与子弹的碰撞检测
	for (Node* i = PBulletHead; i != NULL; i = i->next)
	{
		for (Node* j = EPlane; j != NULL; j = j->next)
		{
			//if (crash(i->x,i->y, BulletWEIGHT, BulletHEIGHT,j->x,j->y, enemyPlaneWEIGHT, enemyPlaneHEIGHT))
			if(j->x - i->x >=0 && j->x - i->x <= enemyPlaneWEIGHT && i->y - j->y >= 0 && i->y - j->y <= enemyPlaneHEIGHT)
			{
				if (i->next == NULL)
					i = NULL;
				else
					i->next = i->next->next;
				if (j->next == NULL)
					j = NULL;
				else
					j->next = j->next->next;

				break;
			}
		}
		if (i == NULL) break;
	}
	//敌机我机碰撞检测
	for (Node* i = EPlane; i != NULL; i = i->next)
	{
		if (crash(player.x,player.y,myAirWEIGHT,myAirHEIGHT,i->x,i->y,enemyPlaneWEIGHT,enemyPlaneHEIGHT))
		{
			player.live = 0;
			//printf("GAME OVER\n");
			putimage(0, 0, &img[4]);
			break;
		}
	}
	EndBatchDraw();
}



void start()
{
	//打开窗口
    initgraph(BGWEIGHT, BGHEIGHT);
	//初始化
	init();
	gameInit();

	while (1)
	{
		gameDraw();
		if (player.live == 0)
		{
			Sleep(1000000);
		}
		while (peekmessage(&msg))
		{
			if (msg.message == WM_KEYDOWN)
			{
				switch (msg.vkcode)
				{
				case VK_W:
					is_move_up = true; 
					//is_move_down = false;
					break;
				case VK_S:
					is_move_down = true;
					//is_move_up = false;
					break;
				case VK_A:
					is_move_left = true;
					//is_move_right = false;
					break;
				case VK_D:
					is_move_right = true;
					//is_move_left = false;
					break;
				case VK_SPACE:
					is_attack = true;
					break;
				}
			}
			else if (msg.message == WM_KEYUP)
			{
				
				switch (msg.vkcode)
				{
				case VK_W:
					is_move_up = false;
					break;
				case VK_S:
					is_move_down = false;
					break;
				case VK_A:
					is_move_left = false;
					break;
				case VK_D:
					is_move_right = false;
					break;
				case VK_SPACE:
					is_attack = false;
					break;
				}
			}
		}

			if (is_move_up)			player.y -= 10;
			if (is_move_down)		player.y += 10;
			if (is_move_left)		player.x -= 10;
			if (is_move_right)		player.x += 10;
			if (is_attack)
			{
				Node * New = createObject(player.x+ myAirWEIGHT/2,player.y,BulletSpeed);
				if (PBulletHead == NULL)
					PBulletHead = New;
				else
				{
					New->next = PBulletHead;
					PBulletHead = New;
				}
			}

		Sleep(50);
	};
}

int main()
{
	
    start();
    return 0;
}

