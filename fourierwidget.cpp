#include "fourierwidget.h"
#include "ui_fourierwidget.h"
#include"_ClassHpp/struct.hpp"
#include<queue>
#include<thread>

FourierWidget::FourierWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FourierWidget)
{
    ui->setupUi(this);
    hideControl();
    _img=new QImage;
    _photoLabel = new QLabel();
    getScreenRect();
    _m1 = tagMart(1, 0, 0, -1, 0, 0);
    _m2 = tagMart(1, 0, 0, -1, 0, 0);
}

FourierWidget::~FourierWidget()
{
    ReleaseVec(_vR);
    ReleaseVec(_vL);
    delete _img;
    delete _photoLabel;
    delete ui;
    delete _clock;
}

void FourierWidget::on_pbt1_clicked()
{
    QLabel* lb = _photoLabel;
    QString filename;
    lb->close();
    filename = QFileDialog::getOpenFileName(this,tr("选择图像"),"",tr("Images (*.bmp)"));//可打开的文件类型
    if(!filename.isEmpty())
    {
        if( 0 == _img->load(filename) ) {//加载图像
            QMessageBox::information(this,tr("打开图像失败"),tr("打开图像失败!"));
            return;
        }
        QPixmap p = QPixmap(filename);
        lb->setPixmap(p);
        lb->resize(p.width(),p.height());
        lb->setFixedSize(p.width(),p.height());
        lb->move(0,0);
        lb->show();
        _m1.e = 0 - _img->width() / 2;
        _m1.f = _m2.f = _img->height() / 2;
        _m2.e = abs(_m1.e);
        whenOpenFile();
    }
    else
        hideControl();
}

void FourierWidget::on_pbt2_clicked()
{
    double n = ui->ed1->text().toDouble();
    int k = ui->ed2->text().toInt();
    if (n && k)
    {
        //清理上一次执行残留
        ReleaseVec(_vR);
        ReleaseVec(_vL);
        //重新执行
        discernBmp(n, k);
    }
    else
        QMessageBox::warning(this, QStringLiteral("Error!"), QStringLiteral("输入不正确。"), QMessageBox::Ok);
}

void FourierWidget::on_pbt3_clicked()
{
    _photoLabel->close();
    _clock->close();
    close();
}

void FourierWidget::closeEvent(QCloseEvent* event)
{
    on_pbt3_clicked();
    event->accept();
}

int FourierWidget::getScreenRect()
{
    QScreen *scr = QGuiApplication::primaryScreen();
    QRect mm = scr->availableGeometry();
    setFixedSize(this->width(), this->height());
    move(mm.width() / 2, mm.height() / 2);
    return 0;
}

int FourierWidget::whenOpenFile()
{
    ui->ed1->show();
    ui->ed2->show();
    ui->ped1->show();
    ui->ped2->show();
    ui->pbt2->show();
    if (_clock)
    {
        _clock->close();
        delete _clock;
    }
    return 0;
}

void FourierWidget::upRec(myPoint& p)
{
    _x0 = min(p.x, _x0);
    _x1 = max(p.x, _x1);
    _y0 = min(p.y, _y0);
    _y1 = max(p.y, _y1);
}

int FourierWidget::checkP()
{
    for (int y = _y0; y < _y1+1; y++)
        for (int x = _x0; x < _x1+1; x++)
        {
            if (_img->pixel(x, y) != COL_POSABLE_LINE)
                continue;
            //图形点,背景点计数
            int countGraph = 0;
            int countBack = 0;
            //检查周围八个点
            for (int k = 0; k < 9; k++)
            {
                int x1 = x + (k % 3) - 1;
                int y1 = y + k / 3 - 1;
                if (_img->pixel(x1, y1) == COL_BACKGROUND)
                    countBack++;
                else if (_img->pixel(x1, y1) == COL_GRAPH)
                    countGraph++;
            }
            //如果图形色较多,则认为这个点是在图形内部,重置为图形色
            if(countGraph> countBack)
                setPixel(x, y, COL_GRAPH);
            else
                setPixel(x, y, COL_BACKGROUND);
        }
    //_img->save("d:\\CodeWork\\check.bmp");
    return 0;
}

Cpx FourierWidget::euler(double i)
{
    Cpx res(cos(i), sin(i));
    return res;
}

int FourierWidget::integralL(const int& k,Cpx* ftList)
{
    for (int k1 = -1; k1 >= -k; k1--)//从低频开始存
        _vL.push_back(integral(k1, ftList));
    return 0;
}

int FourierWidget::integralR(const int& k, Cpx* ftList)
{
    for (int k1 = 0; k1 <= k; k1++)
        _vR.push_back(integral(k1, ftList));
    return 0;
}

void FourierWidget::mulMart(tagMart& m, Cpx* c)
{
    double x = c->r;
    double y = c->i;
    c->r = x * m.a + y * m.b + m.e;
    c->i = x * m.c + y * m.d + m.f;
}

Cpx FourierWidget::mul2(Cpx& c, double i)
{
    Cpx res(c.r * i, c.i * i);
    return res;
}

void FourierWidget::hideControl()
{
    ui->pbt2->hide();
    ui->ed1->hide();
    ui->ed2->hide();
    ui->ped1->hide();
    ui->ped2->hide();
}

int FourierWidget::discernBmp(double& n, int& k)
{
    int bx = 0; int by = 0;
    QImage* temp = new QImage(*_img);
    binaryzation(COL_GRAPH,COL_BACKGROUND,n);
    setOutline(bx,by, COL_GRAPH, COL_BACKGROUND, COL_POSABLE_LINE);
    myPoint p(bx,by);
    Cpx* l = creatOutlineList(p);
    checkP();
    delete _img;
    _img = temp;
    animation(k,l);
    guiWindow(_vL, _vR);
    return 0;
}

Cpx* FourierWidget::integral(int k,Cpx* ftList)
{//链表的每个节点值相当于f(t)
    Cpx* l = ftList;
    Cpx* ck = new Cpx(0, 0);
    for (double i = 0.0; i <= 1; i += _dt)
    {
        Cpx add(l->r, l->i);//f(t)
        add = add * euler(k * -2 * PI * i);//e^-2Πkt
        add = mul2(add, _dt);//*dt
        *ck = *ck + add;//∫积分
        l = l->next;//指向下一个f(t)
        if (!l)
            break;
    }
    return ck;
}

int FourierWidget::animation(int k, Cpx* ftList)
{//周转圆系数
    clear();
    //这里可以线程
    std::thread ta(&FourierWidget::integralL, &*this,k, ftList);
    std::thread tb(&FourierWidget::integralR, &*this, k, ftList);
    ta.join();
    tb.join();
    return 0;
}

void FourierWidget::guiWindow(vector<Cpx*>& vL, vector<Cpx*>& vR)
{
    _clock = new AnalogClockWindow;
    _clock->setPara(&vL, &vR);
    QSize s(_img->width(), _img->height());
    _clock->setMinimumSize(s);
    _clock->setMaximumSize(s);
    _clock->show();
}

Cpx* FourierWidget::creatOutlineList(myPoint& p)
{
    if (p.x == -1 || p.y == -1)
        return NULL;
    /*这是上一个节点.在寻找下一个节点的时候, 从当前节点出发引出一条射线向上一个节点视为一个向量
    检查所有出度的向量与这个向量的夹角大小,然后选取最大的作为下一个节*/
    myPoint pre(-1,0);//因为我的默认条件就是从图形的顶部,往右和下走的,所以第一个向量是这样子设计
    //当前点
    myPoint cur(p.x, p.y);
    setPixel(cur.x, cur.y, COL_REAL_LINE);
    //出度存在多义性的节点
    stack<myPoint> sAmbiguity;
    //整个线路
    stack<myPoint> sPath;
    sPath.push(cur);
    //检查周围八个节点的所有出度
    map<double, myPoint> mOutdegree;
    while (1)
    {
        for (int k = 0; k < 9; k++)
        {
            int x1 = cur.x + (k % 3) - 1;
            int y1 = cur.y + k / 3 - 1;
            if (getPix(x1, y1) != COL_POSABLE_LINE)
                continue;
            myPoint p1(x1 - cur.x, cur.y - y1);
            //求两个向量的夹角
            double theta = (pre.x * p1.x + pre.y * p1.y) /
                (sqrt(pow(pre.x, 2) + pow(pre.y, 2)) * sqrt(pow(p1.x, 2) + pow(p1.y, 2)));
            p1.x = x1;
            p1.y = y1;
            if (mOutdegree.find(theta) == mOutdegree.end())
                mOutdegree[acos(theta)] = p1;
            else
                sAmbiguity.push(cur);
        }
        if (mOutdegree.size() == 0) {//如果没有出度并且临近最开始的点,则完成
            if (isClosePix(cur.x, cur.y, p.x, p.y))
                break;
            if (sAmbiguity.size() == 0)
                break;
            //维护双栈,并且涂白放弃这条线路,直到回到上一个多出度的点
            while (sPath.top().x != sAmbiguity.top().x
                || sPath.top().y != sAmbiguity.top().y)
            {
                myPoint& del = sPath.top();
                setPixel(del.x, del.y, COL_BACKGROUND);
                sPath.pop();
            }
            //裁剪完成之后以当前节点重新寻找,清理，不在认为他是多出度的点
            sAmbiguity.pop();
            //并且回到这个点重新选择另一条路
            cur = sPath.top();
        }
        else
        {//取夹角最大的一个
            myPoint next = mOutdegree.rbegin()->second;
            pre.x = cur.x - next.x;
            pre.y = next.y - cur.y;
            cur = next;
            setPixel(cur.x, cur.y, COL_REAL_LINE);
            sPath.push(cur);
            //清理容器
            mOutdegree.clear();
        }
    }
    //创建链表
    int listSize = 0;
    myPoint& pOut = sPath.top();
    Cpx* l = newCpx(pOut);
    sPath.pop();
    while (sPath.size())
    {
        pOut = sPath.top();
        Cpx* tt = newCpx(pOut);
        listSize++;
        tt->next = l;
        l = tt;
        sPath.pop();
    }
    _img->save("d:\\CodeWork\\realLine.bmp");
    listSize++;
    _dt = 1.0 / listSize;
    return l;
}

int FourierWidget::clear()
{
    ReleaseVec(_vL);
    ReleaseVec(_vR);
    return 0;
}

Cpx *FourierWidget::newCpx(pair<int, int> &p)
{
    double x = p.first * _m1.a + p.second * _m1.b + _m1.e;
    double y = p.first * _m1.c + p.second * _m1.d + _m1.f;
    return new Cpx(x,y);
}

Cpx* FourierWidget::newCpx(myPoint& p)
{
    double x = p.x * _m1.a + p.y * _m1.b + _m1.e;
    double y = p.x * _m1.c + p.y * _m1.d + _m1.f;
    return new Cpx(x, y);
}

bool FourierWidget::isClosePix(int& x1, int& y1, int& x2, int& y2)
{
    if (abs(x1 - x2) <=1 && abs(y1 - y2) <= 1)
        return true;
    return false;
}

void FourierWidget::setPixel(const int& x, const int& y,const QRgb& col)
{
    _img->setPixel(x,y,col);
}

int FourierWidget::colorSet(const QRgb& c1, const QRgb& c2)
{
    for (int y = 0; y < _img->height(); y++)
        for (int x = 0; x < _img->width(); x++)
            if (_img->pixel(x, y) == c1)
                setPixel(x, y, c2);
    return 0;
}

bool FourierWidget::binaryzation(const QRgb& l, const QRgb& r, const double& vHsv)
{
    for (int x = 0; x < _img->width(); x++)
        for (int y = 0; y < _img->height(); y++)
        {
            tagHsv t(_img->pixel(x, y));
            if (t.v < vHsv)
                setPixel(x,y,l);
            else
                setPixel(x,y,r);
        }
    _img->save("d:\\CodeWork\\Binarization.bmp");
    return true;
}

int FourierWidget::setOutline(int& bx, int& by, const QRgb& need, const QRgb& background, const QRgb& set)
{//这个算法好像想得太烂了,以后再优化
    vector<myPoint>v;
     for (int y = 0; y< _img->height(); y++)
        for (int x = 0; x < _img->width(); x++)
        {
            if (_img->pixel(x, y) != need)
                continue;
            //定死遇到的第一个为轮廓开始点,以免后续分析问题摇摆太多
            if (bx == 0)
            {
                bx = x;
                by = y;
            }
            //检查周围八个点有背景色就视为是轮廓点
            for (int k = 0; k < 9; k++)
            {
                int x1 = x + (k % 3) - 1;
                int y1 = y + k / 3 - 1;
                if (_img->pixel(x1, y1) == background) {
                    setPixel(x, y, set);
                    //压入当前轮廓点
                    v.push_back(myPoint(x,y));
                    break;
                }
            }
        }
     //第二遍,要求每个点的周围八个点都必须存在背景色和图形色至少一个,这里是为了减少杂质孤点
     /*所以只能支持填充图！！！*/
     for (auto it : v)
     {
         bool bFindBack = false;
         bool bFindNeed = false;
         for (int k = 0; k < 9 && !(bFindBack && bFindNeed); k++)
             if (_img->pixel(it.x + (k % 3) - 1, it.y + k / 3 - 1) == background)
                 bFindBack = true;
             else if (_img->pixel(it.x + (k % 3) - 1, it.y + k / 3 - 1) == need)
                 bFindNeed = true;
         if (!(bFindBack && bFindNeed))
             setPixel(it.x, it.y,COL_BACKGROUND);
     }
    _img->save("d:\\CodeWork\\maybeLine.bmp");
    return 0;
}


