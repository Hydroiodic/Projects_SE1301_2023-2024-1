#include "Qt_Exam.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Qt_Exam w;
    w.show();
    return a.exec();
}
