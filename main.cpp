#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>
#include <QAbstractSocket>
#include <QTcpSocket>
//#include <QVector>
#include "cyjdata.h"

//Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<CYJData>("CYJData");
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
