#include "searchWindow.h"
#include <QApplication>
#include <QProcessEnvironment>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString qtDir =QProcessEnvironment::systemEnvironment().value("QTDIR");
    qDebug() << "QTDIR: " << qtDir;

    SearchWindow w;
    QObject::connect(&a,&QApplication::focusChanged,&w,&SearchWindow::recvFocusChanged,Qt::QueuedConnection);

    w.show();

    return a.exec();
}
