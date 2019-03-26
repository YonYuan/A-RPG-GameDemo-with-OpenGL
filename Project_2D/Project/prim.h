#pragma once
#include <iostream>
#include <vector>
#include <cstdio>
#include <ctime>
#include <cstdlib>

using namespace std;
#define m 21//row
#define n 21//column
#define down 1
#define right 2
#define left 4
#define up 8
vector <int> block_row;
vector <int> block_column;
vector <int> block_direct;
vector <vector <int>> block_all(m+2);
typedef struct point
{
	int x;
	int y;
}Point;

Point Start, End;
int x_num = 1;
int y_num = 1;
int a[m + 2][n + 2];

 void init_Wall() {//����ͼȫ����1=wall
	for (int i = 0; i <= m + 1; i++) {
		for (int j = 0; j <= n + 1; j++) {
			a[i][j] = 1;//wall
		}
	}
}

void push(int x, int y, int direct) {//��һ�����ݴ�Ž�����vector������
	block_row.push_back(x);
	block_column.push_back(y);
	block_direct.push_back(direct);
}

int count() {//���㵱ǰλ����Χ ǽ�ĸ���
	int cnt = 0;
	if (x_num + 1 <= m) {
		push(x_num + 1, y_num, down);
		cnt++;
	} //down
	if (y_num + 1 <= n) {
		push(x_num, y_num + 1, right);
		cnt++;
	} //right
	if (x_num - 1 >= 1) {
		push(x_num - 1, y_num, up);
		cnt++;
	} //up
	if (y_num - 1 >= 1) {
		push(x_num, y_num - 1, left);
		cnt++;
	} //left
	return cnt;
}

vector<vector <int>> Prim_Maze(int startx,int starty,int endx,int endy) {
	Start.x = startx;//������ʼ��
	Start.y = starty;
	End.x = endx;
	End.y = endy;
	init_Wall();
	srand((unsigned)time(NULL));//���������
	count();
	a[1][1] = 2;
	while (block_row.size()) {//��һ��ѹ������ǽ������ұߺ�������棩����ѭ��
		int num = block_row.size();
		int randnum = rand() % num;//����0-num-1֮����������ͬʱҲ��vector����±�
		x_num = block_row[randnum];//���������п��Բ�д
		y_num = block_column[randnum];
		switch (block_direct[randnum]) {//ѡ��һ��������к�����������ʼ�� �ڿ� Ŀ��� ����������ͬһֱ���� ����ƶ���Ŀ����λ��
		case down: {
			x_num = block_row[randnum] + 1;
			y_num = block_column[randnum];
			break;
		}
		case right: {
			x_num = block_row[randnum];
			y_num = block_column[randnum] + 1;
			break;
		}
		case left: {
			x_num = block_row[randnum];
			y_num = block_column[randnum] - 1;
			break;
		}
		case up: {
			x_num = block_row[randnum] - 1;
			y_num = block_column[randnum];
			break;
		}
		}
		if (a[x_num][y_num] == 1) {//Ŀ��������ǽ
			a[block_row[randnum]][block_column[randnum]] = 2;//��ͨǽ
			a[x_num][y_num] = 2;//��ͨĿ���
			count();//�ٴμ��㵱ǰλ����Χ����ǽ�����������vector
		}
		block_row.erase(block_row.begin() + randnum);//ɾ�����ǽ(���ò��˵�ǽɾ�ˣ�������Щ�Ѿ�ʩ�����˲�����ʩ���ˣ�ͬʱҲ��ȷ������������ѭ��)
		block_column.erase(block_column.begin() + randnum);
		block_direct.erase(block_direct.begin() + randnum);
	}

	for (int i = 0; i <= m + 1; i++) {
		block_all[i].resize(n + 2);
	}

	for (int i = 0; i <= m + 1; i++) {
		for (int j = 0; j <= n + 1; j++) {
			block_all[i][j] = a[i][j];
		}
	}
	return block_all;
}