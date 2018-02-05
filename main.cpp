#include "mainwindow.h"
#include <QApplication>
#include <QMetaType>
#include <QAbstractSocket>
#include <QTcpSocket>
//#include <QVector>
#include "cyjdata.h"

//Q_DECLARE_METATYPE(QAbstractSocket::SocketError)
void myMessageOutput(QtMsgType type, const char *msg)
{
   QString txt;
   QString tim = QDate::currentDate().toString("yyyyMMdd") + QTime::currentTime().toString("hhmmss") + QString("-");
   switch (type) {
   case QtDebugMsg:  //调试信息提示
       txt = tim + QString("Debug: %1\n").arg(msg);
       break;
   case QtWarningMsg: //一般的warning提示
       txt = tim + QString("Warning: %1").arg(msg);
       break;
   case QtCriticalMsg: //严重错误提示
       txt = tim + QString("Critical: %1").arg(msg);
       break;
   case QtFatalMsg: //致命错误提示
       txt = tim + QString("Fatal: %1").arg(msg);
       //abort();
   }

   QFile outFile("logFile.txt"); //写入文件
   outFile.open(QIODevice::WriteOnly | QIODevice::Append);
   QTextStream ts(&outFile);
   ts << txt << "\r\n" << endl;
   if(QtFatalMsg == type){
       abort();
   }
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMsgHandler(myMessageOutput);
    qRegisterMetaType<QVector<int> >("QVector<int>");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<CYJData>("CYJData");
    MainWindow w;
    w.showMaximized();
    return a.exec();
}
