// main.cpp
// include the "main" function and QTest for the program

#include "QLink.h"
#include <QtWidgets/QApplication>

#ifdef QLINK_TEST

#include "QLinkTest.h"
QTEST_MAIN(QLinkTest)

#else

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLink w;
    w.show();
    return a.exec();
}

#endif
