#ifndef _H
#define _H

#include<QWidget>
#include<QFileDialog>
#include<QMessageBox>
#include<QLabel>
#include<QPixmap>
#include<QScreen>
#include<QLineEdit>
#include<stack>
#include<set>
#include"myDefine.h"
#include"_ClassHpp/struct.hpp"
#include"_GuiClassHpp/clockDemo.hpp"

#define PI 3.14159265359

using namespace std;
QT_BEGIN_NAMESPACE
namespace Ui { class FourierWidget; }
QT_END_NAMESPACE

class  FourierWidget : public QWidget
{
    Q_OBJECT

        friend myPoint;
public:
    FourierWidget(QWidget *parent = nullptr);
    ~FourierWidget();

private slots:
    void on_pbt1_clicked();
    void on_pbt2_clicked();
    void on_pbt3_clicked();

private:
    void mulMart(tagMart& m, Cpx* c);
    Cpx mul2(Cpx& c, double i);
    Cpx euler(double i);
    int integralL(const int& k, Cpx* ftList);
    int integralR(const int& k, Cpx* ftList);
    Cpx* integral(int k,Cpx* ftList);

    void hideControl();
    int discernBmp(double& n, int& k);
    int animation(int k, Cpx* ftList);
    void guiWindow(vector<Cpx*>& vL,vector<Cpx*>& vR);
    void closeEvent(QCloseEvent* event);
private:
    int clear();
    Cpx* newCpx(pair<int, int>& p);
    Cpx* newCpx(myPoint& p);
    bool isClosePix(int& x1, int& y1, int& x2, int& y2);
    void setPixel(const int& x,const int& y,const QRgb& col);
    QRgb getPix(int x,int y){return _img->pixel(x,y);};
    //将图片中 c1 颜色的像素点设置为 c2 颜色.
    int colorSet(const QRgb& c1, const QRgb& c2);
    bool binaryzation(const QRgb& l, const QRgb& r, const double& vHsv);
    int setOutline(int& bx,int& by,const QRgb& need, const QRgb& background, const QRgb& set);
    //确定好轮廓点之后创建对应的链表,视为函数f(t),rl为用在原图bmp的坐标链表
    Cpx* creatOutlineList(myPoint& p);
    int getScreenRect();
    int whenOpenFile();//打开文件后做一些自适应调整.
    void upRec(myPoint& p);
    //检查所有被遗弃的轮廓点，重新定性他们.
    int checkP();
private:
    //时间微分
    double _dt = 0.0;
    //记录图形的最小外接矩形
    int _x0 = 99999;
    int _x1 = -9999;
    int _y0 = 99999;
    int _y1 = -9999;
    tagMart _m1;//bmp坐标到动画坐标的矩阵
    tagMart _m2;//动画坐标到bmp坐标的矩阵
    QLabel* _photoLabel = NULL;
    QImage* _img = NULL;//图像
    Ui::FourierWidget *ui;
    vector<Cpx*> _vL;//负频率
    vector<Cpx*> _vR;//正频率
    AnalogClockWindow* _clock = NULL;//动画类
};
#endif // _H
