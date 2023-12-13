// main.cpp
// defines "main" function, providing the entry of the program

#include "QBasic.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QBasic w;
    w.show();
    return a.exec();
}
