#include "searchWindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SearchWindow w;
    qDebug() << QCoreApplication::applicationFilePath();

    QObject::connect(&a,&QApplication::focusChanged,&w,&SearchWindow::recvFocusChanged,Qt::QueuedConnection);
    w.show();

    return a.exec();
}
