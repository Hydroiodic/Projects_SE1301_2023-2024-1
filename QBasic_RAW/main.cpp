#include "QBasic.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QBasic w;
    w.show();
    return a.exec();
}
