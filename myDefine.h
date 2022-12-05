#pragma once
#include<vector>

typedef const int& CINT;

//常用颜色
#define RED qRgb(255,0,0)
//释放动态内存函数模板
template<class T>
void Release(T t)
{
	if (t != NULL) {
		delete t;
		t = NULL;
	}
}
//释放动态内存函数模板
template<class T>
void ReleaseVec(std::vector<T>& v)
{
	for (T& o : v)
		Release(o);
	v.clear();
}
//快速缩写
#define VEC(x) vector<x>
#define PAIR_2INT(x,y) pair<int, int>(x, y)
//輸出的圖片背景色
#define COL_BACKGROUND qRgb(165,165,165)
//输出图形色
#define COL_GRAPH qRgb(255,255,255)
//假设的轮廓色
#define COL_POSABLE_LINE qRgb(0,0,255)
//实际的轮廓色
#define COL_REAL_LINE qRgb(0,0,0)
//多边形填充的填充色
#define COL_FILL qRgb(255,0,0)
