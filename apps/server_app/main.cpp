#include "myservermain.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyServerMain w;
    w.show();
    return a.exec();
}
