#pragma once
#include<vector>

typedef const int& CINT;

//������ɫ
#define RED qRgb(255,0,0)
//�ͷŶ�̬�ڴ溯��ģ��
template<class T>
void Release(T t)
{
	if (t != NULL) {
		delete t;
		t = NULL;
	}
}
//�ͷŶ�̬�ڴ溯��ģ��
template<class T>
void ReleaseVec(std::vector<T>& v)
{
	for (T& o : v)
		Release(o);
	v.clear();
}
//������д
#define VEC(x) vector<x>
#define PAIR_2INT(x,y) pair<int, int>(x, y)
//ݔ���ĈDƬ����ɫ
#define COL_BACKGROUND qRgb(165,165,165)
//���ͼ��ɫ
#define COL_GRAPH qRgb(255,255,255)
//���������ɫ
#define COL_POSABLE_LINE qRgb(0,0,255)
//ʵ�ʵ�����ɫ
#define COL_REAL_LINE qRgb(0,0,0)
//������������ɫ
#define COL_FILL qRgb(255,0,0)
