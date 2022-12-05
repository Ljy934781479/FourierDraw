#include "fourierwidget.h"
#include"_GuiClassHpp/clockDemo.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec* codec = QTextCodec::codecForName("GB2312");
    QTextCodec::setCodecForLocale(codec);
    FourierWidget w;
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
