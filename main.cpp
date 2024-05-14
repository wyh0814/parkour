
#include<stdio.h>
#include<graphics.h>
#include<conio.h>
#include<vector>//���ȿɱ����飬��ʡ�ڴ�
#include"tools.h"

//����tools.cpp�����ֵ�ͷ�ļ�
#include<mmsystem.h>
#pragma comment(lib,"winmm.lib")

using namespace std;

#define WIN_SCORE 100

#define WIN_WIDTH 1012
#define WIN_HEIGHT 396
#define OBSTACLE_COUNT 10

IMAGE imgBgs[3];//����ͼƬ
int bgX[3];//����ͼƬ��x����
int bgSpeed[3] = { 1,2,4 };

IMAGE imgHeros[12];
int heroX;//��ҵ�x����
int heroY;//��ҵ�y����
int heroIndex;//��ұ���ʱ��ͼƬ֡���

bool heroJump;//��ʾ���������Ծ

int jumpHeightMax;
int heroJumpOff;
int update;//�Ƿ���Ҫ����ˢ�»���

int heroblood; //Ѫ��
int score; //����

typedef enum {
	TORTOISE,//�ڹ� 0
	LION,//ʨ�� 1
	HOOK1,
	HOOK2,
	HOOK3,
	HOOK4,
	OBSTACLE_TYPE_COUNT //6
} obstacle_type;

vector<vector<IMAGE>>obstacleImgs;//��������ϰ���ĸ���ͼƬ

typedef struct obstracle {
	int type;//�ϰ�������
	int imgIndex;//��ǰ��ʾ��ͼƬ���
	int x, y;//�ϰ��������
	int speed;
	int power;//ɱ����
	bool exist;
	bool hited;//��ʾ�Ƿ��Ѿ�������ײ
	bool passed; //��ʾ�Ƿ��Ѿ���ͨ��
}obstacle_t;

obstacle_t obstacles[OBSTACLE_COUNT];
int lastObsIndex; //��ʾ��һ��ͼƬ���

IMAGE imgHeroDown[2];
bool heroDown;//��ʾ����Ƿ����¶�״̬

IMAGE imgSZ[10];

//��Ϸ�ĳ�ʼ��
void init() {
	//������Ϸ����
	initgraph(WIN_WIDTH, WIN_HEIGHT);

	//���ر�����Դ
	char name[64];
	for (int i = 0;i < 3;i++) {
		//"res/bg001.png"   "res/bg002.png"   "res/bg003.png"
		sprintf(name, "res/bg%03d.png", i + 1);
		loadimage(&imgBgs[i], name);

		bgX[i] = 0;
	}
	
	//����Hero���ܵ�ͼƬ֡�ز�
	for (int i = 0; i < 12; i++) {
		//"res/hero1.png"..."res/hero12.png"
		sprintf(name, "res/hero%d.png", i + 1);
		loadimage(&imgHeros[i], name);
	}

	//������ҵĳ�ʼλ��
	heroX = WIN_WIDTH * 0.5 - imgHeros[0].getwidth() * 0.5;
	heroY = 345 - imgHeros[0].getheight();
	heroIndex = 0;

	heroJump = false;
	jumpHeightMax= 345 - imgHeros[0].getheight()-120;
	heroJumpOff = -4;

	update = true;

	//����С�ڹ��ز�
	IMAGE imgTort;
	loadimage(&imgTort, "res/t1.png");
	vector<IMAGE>imgTortArray;
	imgTortArray.push_back(imgTort);
	obstacleImgs.push_back(imgTortArray);

	IMAGE imgLion;
	vector<IMAGE>imgLionArray;
	for (int i = 0;i < 6;i++) {
		sprintf(name, "res/p%d.png", i + 1);
		loadimage(&imgLion, name);
		imgLionArray.push_back(imgLion);
	}
	obstacleImgs.push_back(imgLionArray);

	//��ʼ���ϰ����
	for (int i = 0;i < OBSTACLE_COUNT;i++) {
		obstacles[i].exist = false;
	}

	//�����¶��ز�
	loadimage(&imgHeroDown[0], "res/d1.png");
	loadimage(&imgHeroDown[1], "res/d2.png");
	heroDown = false;

	//�����ϰ����ز�
	IMAGE imgH;
	
	for (int i = 0;i < 4;i++) {
		vector<IMAGE>imgHookArray;
		sprintf(name, "res/b%d.png", i + 1);
		loadimage(&imgH, name,63,260,true);
		imgHookArray.push_back(imgH);
		obstacleImgs.push_back(imgHookArray);
	}

	heroblood = 100;

	//Ԥ������Ч
	preLoadSound("res/hit.mp3");

	//���ر�������
	mciSendString("play res/bg.mp3 repeat",0,0,0);

	lastObsIndex = -1;
	score = 0;

	//��������ͼƬ
	for (int i = 0;i < 10;i++) {
		sprintf(name, "res/sz/%d.png", i);
		loadimage(&imgSZ[i], name);
	}
}

//��Ⱦ��Ϸ����
void updateBg() {

	putimagePNG2(bgX[0], 0, &imgBgs[0]);
	putimagePNG2(bgX[1], 119, &imgBgs[1]);
	putimagePNG2(bgX[2], 330, &imgBgs[2]);
}

void createObstacle() {
	int i;
	for (i = 0;i < OBSTACLE_COUNT;i++) {
		if (obstacles[i].exist == false) {
			break;
		}
	}
	if (i > OBSTACLE_COUNT) {
			return;
	}

	obstacles[i].exist = true;
	obstacles[i].hited = false;
	obstacles[i].imgIndex = 0;
	//obstacles[i].type = (obstacle_type)(rand() % OBSTACLE_TYPE_COUNT);
	obstacles[i].type = (obstacle_type)(rand() % 3);

	if (lastObsIndex >= 0 &&                                 /*��������ϰ�*/ 
		obstacles[lastObsIndex].type >= HOOK1 &&              
		obstacles[lastObsIndex].type <= HOOK4 &&
		obstacles[i].type == LION &&
		obstacles[lastObsIndex].x >= (WIN_WIDTH - 500)) {
		obstacles[i].type = TORTOISE;
	}

	lastObsIndex = i;                                       /*��������ϰ�*/

	if (obstacles[i].type == HOOK1) {
		obstacles[i].type += rand() % 4;//0..3
	}
	obstacles[i].x = WIN_WIDTH;
	obstacles[i].y = 345 + 5 - obstacleImgs[obstacles[i].type][0].getheight();
	if (obstacles[i].type == TORTOISE) {
		obstacles[i].speed = 0;
		obstacles[i].power = 5;
	}
	else if (obstacles[i].type == LION) {
		obstacles[i].speed = 4;
		obstacles[i].power = 20;
	}
	else if (obstacles[i].type >=HOOK1&& obstacles[i].type<=HOOK4) {
		obstacles[i].speed = 0;
		obstacles[i].power = 20;
		obstacles[i].y = 0;
	}

	obstacles[i].passed = false;
}

void checkHit() {
	for (int i = 0;i < OBSTACLE_COUNT;i++) {
		if (obstacles[i].exist && obstacles[i].hited == false) {
			int a1x, a1y, a2x, a2y;
			int off = 30;
			if (!heroDown) {//���¶ף������ܻ���Ծ��
				a1x = heroX + off;
				a1y = heroY + off;
				a2x = heroX + imgHeros[heroIndex].getwidth() - off;
				a2y = heroY + imgHeros[heroIndex].getheight();

			}
			else {
				a1x = heroX + off;
				a1y = 345 - imgHeroDown[heroIndex].getheight();
				a2x = heroX + imgHeroDown[heroIndex].getwidth() - off;
				a2y = 345;
			}

			IMAGE img = obstacleImgs[obstacles[i].type][obstacles[i].imgIndex];
			int blx = obstacles[i].x + off;
			int b1y = obstacles[i].y + off;
			int b2x = obstacles[i].x + img.getwidth() - off;
			int b2y = obstacles[i].y + img.getheight() - 10;

			if (rectIntersect(a1x, a1y, a2x, a2y, blx, b1y, b2x, b2y)) {
				heroblood -= obstacles[i].power;
				printf("Ѫ��ʣ��%d\n", heroblood);
				playSound("res/hit.mp3");
				obstacles[i].hited = true;
			}
		}
	}
}

void fly() {
	for (int i = 0;i < 3;i++) {
		bgX[i] -= bgSpeed[i];
		if (bgX[i] < -WIN_WIDTH) {
			bgX[i] = 0;
		}
	}

	//ʵ����Ծ
	if (heroJump) {
		if (heroY < jumpHeightMax) {
			heroJumpOff = 4;
		}

		heroY += heroJumpOff;

		if (heroY > 345 - imgHeros[0].getheight()) {
			heroJump = false;
			heroJumpOff = -4;
		}
	}
	else if (heroDown) {
		static int count = 0;
		int delays[2] = { 4,40 };
		count++;
		if (count >= delays[heroIndex]) {
			count = 0;
			heroIndex++;
			if (heroIndex >= 2) {
				heroIndex = 0;
				heroDown = false;
		    }
		}
	}
	else{ //����Ծ
		heroIndex = (heroIndex + 1) % 12;
	}

	//�����ϰ���
	static int frameCount = 0;
	static int enemyFre = 50;
	frameCount++;
	if (frameCount > enemyFre) {
		frameCount = 0;		
		enemyFre = 50+rand()%50;//50...99 
	    createObstacle();
	}

	//�����ϰ�������
	for (int i = 0;i < OBSTACLE_COUNT;i++) {
		if (obstacles[i].exist) {
			obstacles[i].x -= obstacles[i].speed + bgSpeed[2];
			if (obstacles[i].x < -obstacleImgs[obstacles[i].type][0].getwidth() * 2) {
				obstacles[i].exist = false;
			}

			int len = obstacleImgs[obstacles[i].type].size();
			obstacles[i].imgIndex = (obstacles[i].imgIndex + 1) % len;
		}
	}

	//��Һ��ϰ���ġ���ײ��⡱����
	checkHit();
}

void jump() {
	heroJump = true;
	update = true;
}

void down() {
	heroDown = true;
	update = true;
	heroIndex = 0;
}

//�����û�����������
void keyEvent() {
	char ch;

	if (_kbhit()) { //����а������£�_kbhit()���� true
		ch=getch();    //getch()����Ҫ���»س�����ֱ�Ӷ�ȡ
		if (ch == ' ') {
			jump();
		}
		else if (ch == 'a') {
			down();
		}
	}
}

void updateEnemy() {
	for (int i = 0;i < OBSTACLE_COUNT;i++) {
		if (obstacles[i].exist) {
			putimagePNG2(obstacles[i].x,obstacles[i].y,WIN_WIDTH,
				&obstacleImgs[obstacles[i].type][obstacles[i].imgIndex]);
		}
	}
}

void updateHero() {
	if (!heroDown) {
        putimagePNG2(heroX, heroY, &imgHeros[heroIndex]);
	}
	else {
		int y = 345 - imgHeroDown[heroIndex].getheight();
	    putimagePNG2(heroX, y, &imgHeroDown[heroIndex]);
	}
}

void updateBloodBar() {
	drawBloodBar(10, 10, 200, 10, 2, BLUE, DARKGRAY, RED, heroblood / 100.0);
}

void checkScore() {
	for (int i = 0;i < OBSTACLE_COUNT;i++) {
		if (obstacles[i].exist&&
			 obstacles[i].passed == false &&  
			  obstacles[i].hited == false &&
			   obstacles[i].x + obstacleImgs[obstacles[i].type][0].getwidth() < heroX) {
			if (obstacles[i].type == TORTOISE)score++;
			else if (obstacles[i].type == LION)score = score + 3;
			else score = score + 2;
			obstacles[i].passed = true;
			printf("score: %d\n", score);
		}
	}
}

void updateScore() {
	char str[8];
	sprintf(str, "%d", score);

	int x = 20;
	int y = 25;

	for(int i = 0; str[i]; i++) {
		int sz = str[i] - '0';
		putimagePNG(x, y, &imgSZ[sz]);
		x += imgSZ[sz].getwidth() + 5;
	}
}

void checkWin() {
	if (score >= WIN_SCORE) {
		FlushBatchDraw();
		mciSendString("play res/win.mp3", 0, 0, 0);
		Sleep(2000);
		loadimage(0, "res/win.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3", 0, 0, 0);
		system("pause");

		heroblood = 100;
		score = 0;
		mciSendString("play res/bg.mp3 repeat", 0, 0, 0);
	}

}

void checkOver() {
	if (heroblood <= 0) {
		Sleep(1000);
		loadimage(0, "res/over.png");
		FlushBatchDraw();
		mciSendString("stop res/bg.mp3 repeat", 0, 0, 0);
		system("pause");

		//��֮ͣ�������ֱ�ӿ�ʼ��һ��
		heroblood = 100;
		score = 0;
		mciSendString("play res/bg.mp3", 0, 0, 0);
	}
}

int main(void) {
	init();
	
	//��ʾ��ʼ����
	loadimage(0, "res/begin.png");
	system("pause");

	int timer = 0;
	while (1) {
		keyEvent();
		timer+=getDelay();
		if (timer > 30) {
			timer = 0;
;			update = true;
		}
		
		if (update) {
			update = false;
            BeginBatchDraw();
		    updateBg();
			updateHero();
			updateEnemy();
			updateBloodBar();
			updateScore();
			checkWin();
		    EndBatchDraw();

			checkOver();
			checkScore();
			fly();
		}
		//Sleep(30);����
	}

	updateBg();

	system("pause");
	return 0;
}
