#define  _CRT_SECURE_NO_WARINGS 1
#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<time.h>
#pragma comment( lib, "MSIMG32.LIB")

#define npc_num1 3
// 半透明贴图函数
// 参数：
//		dstimg: 目标 IMAGE 对象指针。NULL 表示默认窗体
//		x, y:	目标贴图位置
//		srcimg: 源 IMAGE 对象指针。NULL 表示默认窗体
//透明贴图函数
void transparentimage(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	HDC dstDC = GetImageHDC(dstimg);
	HDC srcDC = GetImageHDC(srcimg);
	int w = srcimg->getwidth();
	int h = srcimg->getheight();


	// 结构体的第三个成员表示额外的透明度，0 表示全透明，255 表示不透明。
	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	// 使用 Windows GDI 函数实现半透明位图
	AlphaBlend(dstDC, x, y, w, h, srcDC, 0, 0, w, h, bf);
}

IMAGE imgbk;
IMAGE imgplayer[2];
IMAGE imgbullet;
IMAGE imgnpc;


int width = 564;
int height = 750;
int bullet_num = 25;
int m = 0;

//暂停背景函数
void drawPauseScreen() {
	setfillcolor(LIGHTGRAY);
	solidrectangle(200, 200, 600, 400);

	// 设置文字样式
	settextstyle(24, 0, "Arial"); 
	settextcolor(WHITE);

	// 绘制暂停文字
	outtextxy(300, 280, "游戏暂停");
	outtextxy(300, 320, "按任意键继续...");
	FlushBatchDraw();
}

//绘制结束文字
void drawoverscreren() {
	setfillcolor(LIGHTGRAY);
	solidrectangle(200, 200, 600, 400);

	// 设置文字样式
	settextstyle(24, 0, "Arial");
	settextcolor(WHITE);

	outtextxy(300, 280, "游戏结束");
	FlushBatchDraw();
}
//定时器
bool timer(int ms, int id)
{
	static int start[5];
	int end = clock();
	if (end - start[id] >= ms) {
		start[id] = end;
		return true;
	}
	return false;
}

//飞机结构
struct plane 
{
	int x;
	int y;
	bool alive;
	int score;
	int frame;
}player;

plane npc[npc_num1];/*敌机结构*/

//子弹结构
struct bullet
{
	int x;
	int y;
	bool died;
}bullet[25] = { 0 };

// 碰撞检测函数
bool check(plane& player, plane npc[], int npc_size) {
	for (int i = 0; i < npc_size; i++) {
		if (npc[i].alive) {
			// 假设我们知道敌机和玩家飞机的宽度和高度
			int npcWidth = 69; // 敌机的宽度
			int npcHeight = 99; // 敌机的高度
			int playerWidth = 128; // 玩家飞机的宽度
			int playerHeight = 128; // 玩家飞机的高度

			// 玩家飞机和敌机的矩形边界的相交检测
			if (player.x < npc[i].x + npcWidth &&
				player.x + playerWidth > npc[i].x &&
				player.y < npc[i].y + npcHeight &&
				player.y + playerHeight > npc[i].y) {
				// 发生碰撞
				return true;
			}
		}
	}
	// 没有发生碰撞
	return false;
}

//初始化游戏数据
void gamedata() {
	player.x = width / 2.35;
	player.y = height - 120;
	player.alive = true;
	player.score = 0;
	player.frame = 0;
	for (int i = 0;i < bullet_num;i++) /*初始化子弹*/
	 {
		bullet[i].x = 0;
		bullet[i].y = 0;
		bullet[i].died = true;
	}
	/*初始化敌机*/
	 for(int i = 0;i < npc_num1;i++) 
	{
		npc[i].alive = false;
	}
}

//自动创建敌机
void creatnpc()
{
	for (int i = 0;i < npc_num1;i++)
	{
		if (!npc[i].alive)
		{
			/*随机坐标*/
			npc[i].x = rand() % (width-32);
			npc[i].y = -99;
			npc[i].alive = true;
			break;
		
		}
	}
}


//加载图片
void loadImage()
{
	loadimage(&imgbk, "./images/background1.png");
	loadimage(&imgplayer[0], "./images/player.png");
	loadimage(&imgplayer[1], "./images/player1.png");
	loadimage(&imgbullet, "./images/bullet.png");
	loadimage(&imgnpc, "./images/npc.png");
}


// 游戏图片绘制
void gameDraw() {
	loadImage();
	putimage(0, 0, &imgbk);
	transparentimage(NULL, player.x, player.y, imgplayer + player.frame);
	player.frame = (player.frame + 1) % 2;

	for (int i = 0;i < bullet_num;i++)
	{
		if (!bullet[i].died)
		{
			transparentimage(NULL, bullet[i].x, bullet[i].y, &imgbullet);
		}
	}
	for (int i = 0;i < npc_num1;i++)
	{
		if(npc[i].alive)
			transparentimage(NULL, npc[i].x, npc[i].y, &imgnpc);
	}
}

//创建子弹
void bulletcreat()
{
	for (int i = 0;i < bullet_num;i++)
	{
		if (bullet[i].died)
		{
			bullet[i].x = player.x + 64;
			bullet[i].y = player.y;
			bullet[i].died = false;
			break;
		}
	}
}

//子弹移动
void bulletmove()
{
	for (int i = 0;i < bullet_num;i++)
	{
		if (!bullet[i].died) {
			bullet[i].y -= 4;
			if (bullet[i].y < 0)
			{
				bullet[i].died = true;
			}
		}
	}
}

//玩家移动
void playermove(int speed) {
	if (_kbhit()) {
		int key = _getch(); // 仅需一次_getch()调用
		switch (key) {
		case 27: // ESC键
			// 显示暂停画面
			drawPauseScreen();
			_getch(); // 等待玩家按下任意键继续
			cleardevice(); // 清屏
			gameDraw(); // 重新绘制游戏画面以恢复游戏状态
			break;
		case 'w':
		case 'W':
			if (player.y > 0) {
				player.y -= speed;
			}
			break;
		case 's':
		case 'S':
			if (player.y < height - 128) {
				player.y += speed;
			}
			break;
		case 'a':
		case 'A':
			if (player.x + 60 > 0) {
				player.x -= speed;
			}
			break;
		case 'd':
		case 'D':
			if (player.x - 60 < width - 128) {
				player.x += speed;
			}
			break;
		}
		// 检测空格键来发射子弹，注意此处不再是用_getch()判断
		if (GetAsyncKeyState(VK_SPACE) && timer(200, 0)) {
			bulletcreat(); // 创建子弹
		}
	}
}

//敌机移动
	void npcmove(int speed)
	{
		for (int i = 0;i < npc_num1;i++)
		{
			if (npc[i].alive)
				npc[i].y += speed;
			if (npc[i].y > height)
			{
				npc[i].alive = false;
			}
		}
	}

//打飞机函数
void playplane()
{
	for (int i = 0;i < npc_num1;i++)
	{
		if (!npc[i].alive)
			continue;
		for (int j = 0;j < bullet_num;j++)
		{
			if (bullet[j].died)
				continue;
			if (bullet[j].x > npc[i].x && bullet[j].x<npc[i].x + 69
				&& bullet[j].y>npc[i].y && bullet[j].y < npc[i].y + 99)
			{
				bullet[j].died = true;
				npc[i].alive = false;
				player.score++;
			}
		}
    }
}
// 定义 gameLoop 函数
void gameLoop() {
	gamedata(); // 初始化游戏数据
	while (player.alive) { // 使用玩家存活状态作为循环条件
		int starttime = clock();
		gameDraw();
		bulletmove();
		creatnpc();
		npcmove(1);
		playplane();
		playermove(10);

		// 检测玩家与敌机的碰撞
		if (check(player, npc, npc_num1)) {
			MessageBox(NULL, TEXT("游戏结束"), TEXT("游戏结束"), MB_OK);
			player.alive = false; // 更新玩家状态为不存活，退出游戏循环
		}

		int frametime = clock() - starttime;
		if (1000 / 80 - frametime > 0) {
			Sleep(1000 / 80 - frametime);
		}
	}
}

// 主函数
int main() {
	initgraph(width, height, SHOWCONSOLE); // 初始化图形窗口
	gameLoop(); // 调用 gameLoop 函数来运行游戏
	getchar(); // 等待用户输入，防止窗口立即关闭
	return 0;
}