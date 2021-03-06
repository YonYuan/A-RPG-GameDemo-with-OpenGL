// Project.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "prim.h"
#include "loadTex.h"
#include <iostream>
#include <vector>
#include <Gl/glut.h>
#include <ctime>
#include "Draw.h"
#include "mmsystem.h"

using namespace std;

#define KEY_DOWN(vk_code)((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
//#define m 21//row
//#define n 21//column

int locm;
int locn;
int locGhostx, locGhosty;

bool Ghostkill = FALSE;
bool GhostFind = FALSE;
bool AliceFind = FALSE;
bool BeginGame = FALSE;
bool KillorNot = FALSE;

bool moveStye = FALSE;
bool moveup = FALSE;
bool moveleft = FALSE;

int SCREEN_WIDTH = 800;//屏幕宽       
int SCREEN_HEIGHT = 600;//屏幕高
GLfloat movesight = 0;
GLfloat knumber = 3;//系数
int Block_size = 14*knumber;
int Hero_size = 6*knumber;
GLfloat key_timex = 0;
GLfloat key_timey = 0;
clock_t start, stop;//程序时间
GLfloat dur_time;
GLfloat MaxSpeed = 3*knumber;
GLfloat MinSpeed = 1;
GLfloat moveSpeed = 0.5f;
GLfloat movex = 0;
GLfloat movey = 0;


GLuint TexShadow;
GLuint TexShadow2;
GLuint TexOld;
GLuint TexAlice;
GLuint Tex1;
GLuint Tex2;
GLuint TexStone;
GLuint FirstPage;

vector<vector <int>> Maze(m+2);


void init_Maze() {
	for (int i = 0; i <= m + 1; i++) {
		block_all[i].resize(n + 1);
	}
}

void init_gl() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void init_tex() {
	TexShadow = load_texture("Shadow.bmp");
	TexShadow2 = load_texture("Shadow2.bmp");
	TexOld = load_texture("old.bmp");
	TexAlice = load_texture("Alice.bmp");
	Tex1 = load_texture("1.bmp");
	Tex2 = load_texture("2.bmp");
	TexStone = load_texture("Stone.bmp");
	FirstPage = load_texture("FirstPage.bmp");
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
}
//音乐导入
void music()
{
	MCI_OPEN_PARMS op;                                     //播放背景音乐
	op.dwCallback = NULL;
	op.lpstrAlias = NULL;
	op.lpstrDeviceType = "MPEGAudio";
	op.lpstrElementName = "背景音乐.mp3";
	op.wDeviceID = NULL;
	UINT rs;
	rs = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&op);
	if (rs == 0)
	{
		MCI_PLAY_PARMS pp;

		pp.dwCallback = NULL;

		pp.dwFrom = 0;

		mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&pp);
	}
}
//边缘判断
void Edge() {
	bool leftdown,leftup,rightup,rightdown;
	bool upblock, downblock, rightblock, leftblock;
	//左下
	if (readPixel(SCREEN_WIDTH / 2 - Block_size / 2 -1 , SCREEN_HEIGHT / 2 - Block_size / 2-1)) {
		leftdown = TRUE;
	}
	else leftdown = FALSE;
	//左上
	if (readPixel(SCREEN_WIDTH / 2 - Block_size / 2-1, SCREEN_HEIGHT / 2 + Hero_size - Block_size / 2+1)) {
		leftup = TRUE;
	}
	else leftup = FALSE;
	//右上
	if (readPixel(SCREEN_WIDTH / 2+Hero_size - Block_size / 2+1, SCREEN_HEIGHT / 2+Hero_size - Block_size / 2+1)) {
		rightup = TRUE;
	}
	else rightup = FALSE;
	//右下
	if (readPixel(SCREEN_WIDTH / 2+Hero_size - Block_size / 2+1, SCREEN_HEIGHT / 2 - Block_size / 2-1)) {
		rightdown = TRUE;
	}
	else rightdown = FALSE;
	//上
	if (readPixel(SCREEN_WIDTH / 2 - Block_size / 2+Hero_size/2, SCREEN_HEIGHT / 2 + Hero_size - Block_size / 2+1)) {
		upblock = TRUE;
	}
	else upblock = FALSE;
	//左
	if (readPixel(SCREEN_WIDTH / 2- Block_size / 2-1, SCREEN_HEIGHT / 2+Hero_size/2-Block_size/2 )) {
		leftblock = TRUE;
	}
	else leftblock = FALSE;
	//下
	if (readPixel(SCREEN_WIDTH / 2+Hero_size/2-Block_size/2, SCREEN_HEIGHT / 2 - Block_size / 2-1)) {
		downblock = TRUE;
	}
	else downblock = FALSE;
	//右
	if (readPixel(SCREEN_WIDTH / 2 + Hero_size - Block_size / 2 +1, SCREEN_HEIGHT / 2 - Block_size / 2+Hero_size/2)) {
		rightblock = TRUE;
	}
	else rightblock = FALSE;

	if ((leftup&&rightup)||upblock) {
		movey += moveSpeed*MaxSpeed*1.5;
		if (moveStye&&moveup) {
			key_timey = 0;
		}
	}
	if ((leftup&&leftdown)||leftblock) {
		movex -= moveSpeed*MaxSpeed*1.5;
		if (moveStye&&moveleft) {
			key_timex = 0;
		}
	}
	if ((leftdown&&rightdown)||downblock) {
		movey -= moveSpeed*MaxSpeed*1.5;
		if (moveStye && !moveup) {
			key_timey = 0;
		}
	}
	if ((rightdown&&rightup)||rightblock) {
		movex += moveSpeed*MaxSpeed*1.5;
		if (moveStye && !moveleft) {
			key_timex = 0;
		}
	}
}
//交互判断
void Meet() {
	if (!BeginGame) {
		POINT mousePos;
		GetCursorPos(&mousePos);
		if (mousePos.x >= 400 && KEY_DOWN(VK_LBUTTON)) {
			BeginGame = TRUE;
		}
	}
	else {
		//绿色判定计算
		{
			//上
			if (readPixelRGB(SCREEN_WIDTH / 2 - Block_size / 2 + Hero_size / 2, SCREEN_HEIGHT / 2 + Hero_size - Block_size / 2 + MaxSpeed,0,255,0)) {
				AliceFind = TRUE;
			}
			//左
			if (readPixelRGB(SCREEN_WIDTH / 2 - Block_size / 2 - MaxSpeed, SCREEN_HEIGHT / 2 + Hero_size / 2 - Block_size / 2,0,255,0)) {
				AliceFind = TRUE;
			}
			//下
			if (readPixelRGB(SCREEN_WIDTH / 2 + Hero_size / 2 - Block_size / 2, SCREEN_HEIGHT / 2 - Block_size / 2 - MaxSpeed, 0, 255, 0)) {
				AliceFind = TRUE;
			}
			//右
			if (readPixelRGB(SCREEN_WIDTH / 2 + Hero_size - Block_size / 2 + MaxSpeed, SCREEN_HEIGHT / 2 - Block_size / 2 + Hero_size / 2, 0, 255, 0)) {
				AliceFind = TRUE;
			}
		}
		{
			//上
			if (readPixelRGB(SCREEN_WIDTH / 2 - Block_size / 2 + Hero_size / 2, SCREEN_HEIGHT / 2 + Hero_size - Block_size / 2 + MaxSpeed, 0, 0, 255)) {
				GhostFind = TRUE;
			}
			else
			//左
			if (readPixelRGB(SCREEN_WIDTH / 2 - Block_size / 2 - MaxSpeed, SCREEN_HEIGHT / 2 + Hero_size / 2 - Block_size / 2, 0, 0, 255)) {
				GhostFind = TRUE;
			}
			else
			//下
			if (readPixelRGB(SCREEN_WIDTH / 2 + Hero_size / 2 - Block_size / 2, SCREEN_HEIGHT / 2 - Block_size / 2 - MaxSpeed, 0, 0, 255)) {
				GhostFind = TRUE;
			}
			else
			//右
			if (readPixelRGB(SCREEN_WIDTH / 2 + Hero_size - Block_size / 2 + MaxSpeed, SCREEN_HEIGHT / 2 - Block_size / 2 + Hero_size / 2, 0, 0, 255)) {
				GhostFind = TRUE;
			}
			else GhostFind = FALSE;
		}
		if (AliceFind) {
			cout << "Game over";
			//Sleep(10000);
		}
		if (GhostFind) {
			cout << "Ghost Find";
			//Sleep(10000);
		}

	}
}
//移动
void Move() {
	if (moveStye) {
		if (KEY_DOWN(VK_LEFT) || KEY_DOWN('A') || KEY_DOWN(VK_RIGHT) || KEY_DOWN('D')) {

			if(key_timex<=1.5*MaxSpeed) key_timex += 0.1;
			cout << key_timex << endl;
		}
		else if (key_timex >= 0)key_timex -= 0.1; else key_timex = 0;

		if (KEY_DOWN(VK_UP) || KEY_DOWN('W') || KEY_DOWN(VK_DOWN) || KEY_DOWN('S')) {

			if (key_timey <= 1.5*MaxSpeed) key_timey += 0.1;
			cout << key_timey << endl;
		}
		else if (key_timey >= 0)key_timey -= 0.1; else key_timey = 0;
	}
	else {
		if (KEY_DOWN(VK_LEFT) || KEY_DOWN('A') || KEY_DOWN(VK_RIGHT) || KEY_DOWN('D')||
			KEY_DOWN(VK_UP) || KEY_DOWN('W') || KEY_DOWN(VK_DOWN) || KEY_DOWN('S')) {

			if (key_timex <= 1.5*MaxSpeed) key_timex += 0.1;
			cout << key_timex << endl;
		}
		else if (key_timex >= 0)key_timex -= 0.1; else key_timex = 0;
	}


	if (moveStye) {
		if (KEY_DOWN(VK_LEFT) || KEY_DOWN('A')) {
			movex += moveSpeed*key_timex;
			moveleft = TRUE;
		}
		else if (moveleft) {
			movex += 0.1*moveSpeed * key_timex* key_timex;
		}
		else {
			movex -= 0.1*moveSpeed * key_timex* key_timex;
		} // 向左
		if (KEY_DOWN(VK_RIGHT) || KEY_DOWN('D')) {
			movex -= moveSpeed * key_timex;
			moveleft = FALSE;
		}
		else if (moveleft) {
			movex += 0.1*moveSpeed * key_timex* key_timex;
		}
		else {
			movex -= 0.1*moveSpeed * key_timex* key_timex;
		}//向右
		if (KEY_DOWN(VK_UP) || KEY_DOWN('W')) {
			movey -= moveSpeed * key_timey;
			moveup = TRUE;
		}
		else if (!moveup) {
			movey += 0.1*moveSpeed * key_timey* key_timey;
		}
		else {
			movey -= 0.1*moveSpeed * key_timey* key_timey;
		}//前进
		if (KEY_DOWN(VK_DOWN) || KEY_DOWN('S')) {
			movey += moveSpeed * key_timey;
			moveup = FALSE;
		}//后退
		else if (!moveup) {
			movey += 0.1*moveSpeed * key_timey* key_timey;
		}
		else {
			movey -= 0.1*moveSpeed * key_timey* key_timey;
		}
	}
	else {
		if (KEY_DOWN(VK_LEFT) || KEY_DOWN('A')) {
			movex += moveSpeed * key_timex;
			moveleft = TRUE;
		} // 向左
		if (KEY_DOWN(VK_RIGHT) || KEY_DOWN('D')) {
			movex -= moveSpeed * key_timex;
			moveleft = FALSE;
		}//向右
		if (KEY_DOWN(VK_UP) || KEY_DOWN('W')) {
			movey -= moveSpeed * key_timex;
			moveup = TRUE;
		}//前进
		if (KEY_DOWN(VK_DOWN) || KEY_DOWN('S')) {
			movey += moveSpeed * key_timex;
			moveup = FALSE;
		}//后退
	}

	Edge();
	if (KEY_DOWN('L')) {
		movesight =0;
	}else movesight =1;
}
//画鬼
void Draw_Ghost(int x, int y, int size) {
	glColor4f(0.0f, 0.0f, 1.0f, 1.0);
	//glPushMatrix();
	//glTranslated(-m / 2 * Block_size + SCREEN_WIDTH / 2, -n / 2 * Block_size + SCREEN_HEIGHT / 2, 0);
	glPushMatrix();
	glTranslated(x*size, y*size, 0);
	glBegin(GL_QUADS);
	glVertex2f(x , y + size);
	glVertex2f(x + size, y + size);
	glVertex2f(x + size, y );
	glVertex2f(x, y);

	glEnd();
	glFlush();
	//glPopMatrix();
	glPopMatrix();
}
//画鬼Tex
void Draw_GhostTex(int x, int y, int size) {
	glColor4f(0.0f, 0.0f, 1.0f, 1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexOld);
	glPushMatrix();
	glTranslated(-m / 2 * Block_size + SCREEN_WIDTH / 2, -n / 2 * Block_size + SCREEN_HEIGHT / 2, 0);
	glPushMatrix();
	glTranslated(x*size, y*size, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + size);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x + size, y + size);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + size, y);
							  
	glEnd();
	glFlush();
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}
//画人
void Draw_Hero(int size) {
	glPushMatrix();
	Move();
	glTranslated(-movex, -movey, 0);
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f,0.5);

	glBegin(GL_QUADS);
	glVertex2f(SCREEN_WIDTH / 2-Block_size/2, SCREEN_HEIGHT / 2 + size-Block_size/2);
	glVertex2f(SCREEN_WIDTH / 2-Block_size/2 + size, SCREEN_HEIGHT / 2 + size - Block_size / 2);
	glVertex2f(SCREEN_WIDTH / 2-Block_size/2 + size, SCREEN_HEIGHT / 2 - Block_size / 2);
	glVertex2f(SCREEN_WIDTH / 2-Block_size/2, SCREEN_HEIGHT / 2 - Block_size / 2);

	glEnd();
	glFlush();
	glPopMatrix();
	glPopMatrix();
}
//画人tex
void Draw_HeroTex(int size) {
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, Tex2);
	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	//Move();
	glTranslated(-movex, -movey, 0);
	glPushMatrix();
	glColor4f(1.0f, 1.0f, 1.0f, 1);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(SCREEN_WIDTH / 2 - Block_size / 2-2, SCREEN_HEIGHT / 2 - Block_size / 2-2);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(SCREEN_WIDTH / 2 - Block_size / 2-2, SCREEN_HEIGHT / 2 + 2*size - Block_size / 2+2);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(SCREEN_WIDTH / 2 - Block_size / 2 + size+2, SCREEN_HEIGHT / 2 + 2*size - Block_size / 2+2);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(SCREEN_WIDTH / 2 - Block_size / 2 + size+2, SCREEN_HEIGHT / 2 - Block_size / 2-2);
	glEnd();
	glFlush();
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
//画女猪脚Tex
void Draw_AliceTex(int x, int y) {
	glColor4f(1.0f, 1.0f, 1.0f,1.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, TexAlice);
	glPushMatrix();
	glTranslated(-m / 2 * Block_size + SCREEN_WIDTH / 2, -n / 2 * Block_size + SCREEN_HEIGHT / 2, 0);
	glPushMatrix();
	glTranslated(x*Block_size, y*Block_size, 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x, y);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(x, y + 1.5*Block_size);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x + Block_size, y + 1.5*Block_size);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x + Block_size, y);
							 
	glEnd();
	glFlush();
	glPopMatrix();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

}
//画女猪脚
void Draw_Alice(int x, int y) {
	glColor4f(0.0f, 1.0f, 0.0f, 1.0);
	glPushMatrix();
	glTranslated(x*Block_size, y*Block_size, 0);

	glBegin(GL_QUADS);
	glVertex2f(x, y + Block_size);
	glVertex2f(x + Block_size, y + Block_size);
	glVertex2f(x + Block_size, y);
	glVertex2f(x, y);

	glEnd();
	glFlush();
	glPopMatrix();
}
//画地图
void Draw_Map() {
	glPushMatrix();
	glTranslated(-m / 2 * Block_size+SCREEN_WIDTH/2, -n / 2 * Block_size+SCREEN_HEIGHT/2, 0);
	for (int i = 0; i <= m + 1; i++) {
		for (int j = 0; j <= n + 1; j++) {
			if (i == locGhostx && j == locGhosty) {
				if (!Ghostkill) {
					Draw_Ghost(locGhostx,locGhosty,Block_size);
				}
			}else
			if (Maze[i][j] == 1)
				Draw_Block(i, j, Block_size);
		}
	}
	Draw_Alice(locm, locn);
	glPopMatrix();
};
//画对话
void DrawWord(const char *std, int sleeptime, int posy) {
	glPushMatrix();
	glColor4f(1.0, 1.0, 1.0,1);

	glRasterPos2f(SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 * 7+posy);
	//glScaled(2, 4, 0);
	drawString(std);
	if (sleeptime > 100) {
		Sleep(sleeptime);
	}
	glPopMatrix();
}

void DrawWordBlack(const char *std, int sleeptime, int posy) {
	glPushMatrix();
	glColor4f(0.0, 0.0, 0.0, 1);

	glRasterPos2f(SCREEN_WIDTH / 8, SCREEN_HEIGHT / 8 * 7 + posy);
	//glScaled(2, 4, 0);
	drawString(std);
	if (sleeptime > 100) {
		Sleep(sleeptime);
	}
	glPopMatrix();
}
//画灯光
void Shadow() {
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.99);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, TexShadow);
	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	glPushMatrix();
	glTranslated(-SCREEN_WIDTH*1.5-Block_size*1.3, -SCREEN_HEIGHT*1.5-Block_size*0.3, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f);glVertex3f(0, 0,0);
	glTexCoord2f(0.0f, 1.0f);glVertex3f(0, 4*SCREEN_HEIGHT,0);
	glTexCoord2f(1.0f, 1.0f);glVertex3f(4*SCREEN_WIDTH, 4*SCREEN_HEIGHT,0);
	glTexCoord2f(1.0f, 0.0f);glVertex3f(4*SCREEN_WIDTH, 0,0);
	glEnd();
	glFlush();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
//画灯光
void Shadow2() {
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glColor4f(1.0f, 1.0f, 1.0f, 0.99);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, TexShadow2);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//设置滤波模式
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glPushMatrix();
	//glTranslated(-SCREEN_WIDTH * 1.5 - Block_size * 1.3, -SCREEN_HEIGHT * 1.5 - Block_size * 0.3, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,  SCREEN_HEIGHT, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f( SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f( SCREEN_WIDTH, 0, 0);
	glEnd();
	glFlush();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

}
//首页
void Draw_Firstpage() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, FirstPage);
	glBegin(GL_QUADS);

	glTexCoord2f(0.0f, 0.0f); glVertex3f(0, 0, 0);
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0,SCREEN_HEIGHT, 0);
	glTexCoord2f(1.0f, 1.0f); glVertex3f(SCREEN_WIDTH,SCREEN_HEIGHT, 0);
	glTexCoord2f(1.0f, 0.0f); glVertex3f(SCREEN_WIDTH, 0, 0);
	glEnd();
	glFlush();
	glDisable(GL_TEXTURE_2D);
}

void midBlack(){
	stop = clock();
	dur_time =stop - start;

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(0.0, 0.0f, 0.0f, sin(dur_time/1000));
	glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_QUADS);
	glVertex3f(0, 0, 0);
	glVertex3f(0, SCREEN_HEIGHT, 0);
	glVertex3f(SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	glVertex3f(SCREEN_WIDTH, 0, 0);
	glEnd();
	glFlush();

}

void myDisplay() {

	init_gl();
	//判定区域绘制
	if (BeginGame) {
		glPushMatrix();

		glTranslated(movex, movey, 0);//view position
		Draw_Map();
		Draw_Hero(Hero_size);

		glPopMatrix();
	}
	else {
		Draw_Firstpage();
	}
	Meet();
	//贴图绘制
	if (BeginGame) {
		glPushMatrix();
		glTranslated(movex, movey, 0);//view position

		//Draw_Map();
		if (!Ghostkill) {
			Draw_GhostTex(locGhostx, locGhosty, Block_size);
		}

		Draw_AliceTex(locm, locn);
		Draw_HeroTex(Hero_size);
		glPopMatrix();

		if (movesight||!Ghostkill) {
			Shadow();

		}
		Shadow2();
		DrawWord("I have to find Alice......",0,0);
		if (GhostFind&&!Ghostkill) {
			DrawWord("???: Go away! Evil!", 0,-30);
			DrawWord("???: Do not catch my MAP!",0, -60);
			if (KEY_DOWN('K'))Ghostkill = TRUE;
			KillorNot = TRUE;
		}
		if (!GhostFind && !Ghostkill&&KillorNot) {
			DrawWord("Map?", 0, -120);
			DrawWord("...Maybe I could kill him and .....", 0, -150);
			DrawWord("(Close and down K for killing)", 0, -180);
		}
		if (Ghostkill) {
			DrawWordBlack(".............", 0, -30);
			if (!KEY_DOWN('L')) {
				DrawWord("(down L for Map)", 0, -60);
				DrawWord("It's danger everywhere.....", 0, -90);
			}
		}
	}
	if (Ghostkill&&AliceFind) {
		//Shadow();
		DrawWord("It's too late........", 0, -330);
		DrawWordBlack("It's too late.......", 0, -360);
		for (int i = 1; i <= 10; i++) {
			DrawWord("Alice is Dead!!!!!!!!!!!!!!!!!", 0, -30*i);
			Sleep(100);
			DrawWordBlack("Alice is Dead!!!!!!!!!!!!!!!!!", 0, -15*i);
			glFlush();
		}
		//Sleep(10000);
	}else
	if (!Ghostkill&&AliceFind) {
		for (int i = 0; i <= 3; i++) {
			DrawWord("Alice!!!!!!!!!!!!!!!!!", 0, -30);
			//DrawWordBlack("Alice!!!!!!!!!!!!!!!!!", 0, -15);
			DrawWord("But I don't have map......", 0, -60);
			DrawWord("There is someone who has the map", 0, -90);
		}
		AliceFind = FALSE;
	}
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (double)w, 0, (double)h);
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Demo");
	music();
	srand((unsigned)time(NULL));//随机数种子
	locm = (rand() % 2 >= 1 ? m*1 : 1);
	locn = (rand() % 2 >= 1 ? n*1 : 1);
	locGhostx = rand() % (m-1)+1;
	locGhosty = rand() % (n-1)+1;

	init_gl();
	init_Maze();
	init_tex();
	Maze = Prim_Maze(m/2, n/2, locm, locn);

	start = clock();
	cout << locm << " " << locn << endl;
	cout << locGhostx << " " << locGhosty << endl;

	glutDisplayFunc(&myDisplay);
	glutReshapeFunc(Reshape);
	glutIdleFunc(&myDisplay);
	glutMainLoop();
    return 0;
}

