#include "surfacecommunication.h"
#include <QDebug>
#include <QThread>

SurfaceCommunication::SurfaceCommunication(QObject *parent) : QObject(parent),
    m_isOn(false),
    _doWorkCount(0)
{
     connect(&m_tcpSocket, SIGNAL(connected()), this, SLOT(slot_on_connected()));
     connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_on_readMessage()));
     connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_on_error(QAbstractSocket::SocketError)));
}

void SurfaceCommunication::init(const QString ip, const int port)
{
    m_hostIp = ip;
    m_port = port;
    m_tcpSocket.abort();
    m_tcpSocket.connectToHost(m_hostIp, m_port);
    if (m_tcpSocket.waitForConnected(CONNECTMAXDELAY))
    {
        emit sig_statusTable("Surface Tcp Init Succeeded");
        qDebug()<<"Surface Tcp Init Succeeded";
    }
}

int SurfaceCommunication::doWorkCount() const
{

}
//send data to surface
void SurfaceCommunication::slot_doWork()
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);

    for(int i = 0;i<NUMBERONEFRAME;i++)
    {
        if(i==0) out<<quint8(0xAA);
        if(i==1) out<<quint8(0x55);
        if(i==2)
        {
            int aa = 0;
            aa += m_cyjData_actual.forward;
            aa += m_cyjData_actual.backward*2;
            aa += m_cyjData_actual.neutral*4;
            aa += m_cyjData_actual.stop*8;
            aa += m_cyjData_actual.scram*16;
            aa += m_cyjData_actual.light*32;
            aa += m_cyjData_actual.horn*64;
            aa += m_cyjData_actual.zero*128;
            out<<quint8(aa);
        }
        if(i==3)
        {
            int aa = 0;
            aa += m_cyjData_actual.manualVisual;
            aa += m_cyjData_actual.localRemote*2;
            aa += m_cyjData_actual.start*4;
            aa += m_cyjData_actual.flameout*8;
            aa += m_cyjData_actual.middle*16;
            aa += m_cyjData_actual.warn1*32;
            aa += m_cyjData_actual.warn2*64;
            aa += m_cyjData_actual.warn3*128;
            out<<quint8(aa);
        }
        if(i==4) out<<quint8(m_cyjData_actual.rise);
        if(i==5) out<<quint8(m_cyjData_actual.fall);
        if(i==6) out<<quint8(m_cyjData_actual.turn);
        if(i==7) out<<quint8(m_cyjData_actual.back);
        if(i==8) out<<quint8(m_cyjData_actual.left);
        if(i==9) out<<quint8(m_cyjData_actual.right);
        if(i==10) out<<quint8(m_cyjData_actual.acc);
        if(i==11) out<<quint8(m_cyjData_actual.deacc);
        if(i==12) out<<quint8(m_cyjData_actual.speed);
        if(i==13) out<<quint8(m_cyjData_actual.engine);
        if(i==14) out<<quint8(m_cyjData_actual.spliceAngle);
        if(i==15) out<<quint8(m_cyjData_actual.oil);
        if(i==16) out<<quint8(m_cyjData_actual.temperature);
        if(i==17) out<<quint8(0xFF);
    }
//    qDebug()<<"=========================actual data are:";
//    qDebug()<<"neutral:"<<m_cyjData_actual.neutral;
//    qDebug()<<"stop:"<<m_cyjData_actual.stop;
//    qDebug()<<"scram:"<<m_cyjData_actual.scram;
//    qDebug()<<"light:"<<m_cyjData_actual.light;
//    qDebug()<<"horn:"<<m_cyjData_actual.horn;
//    qDebug()<<"horn:"<<m_cyjData_actual.horn;
//    qDebug()<<"autoManual:"<<m_cyjData_actual.manualVisual;
//    qDebug()<<"RemoteLocal:"<<m_cyjData_actual.localRemote;
//    qDebug()<<"flameout:"<<m_cyjData_actual.flameout;
//    qDebug()<<"middle:"<<m_cyjData_actual.middle;
//    qDebug()<<"rise:"<<m_cyjData_actual.rise;
//    qDebug()<<"fall:"<<m_cyjData_actual.fall;
//    qDebug()<<"turn:"<<m_cyjData_actual.turn;
//    qDebug()<<"back:"<<m_cyjData_actual.back;
//    qDebug()<<"left:"<<m_cyjData_actual.left;
//    qDebug()<<"right:"<<m_cyjData_actual.right;
//    qDebug()<<"acc:"<<m_cyjData_actual.acc;
//    qDebug()<<"deacc:"<<m_cyjData_actual.deacc;
//    qDebug()<<"speed:"<<m_cyjData_actual.speed;
//    qDebug()<<"engine:"<<m_cyjData_actual.engine;
//    qDebug()<<"splice:"<<m_cyjData_actual.spliceAngle;
    //AA 55 10 02 00 00 00 10 10 20 22 21 29 21 44 83 00 FF
//    out<<quint8(0xAA)<<quint8(0x55)<<quint8(0x10)<<
//         quint8(0x02)<<quint8(0x00)<<quint8(0x00)<<
//         quint8(0x00)<<quint8(0x10)<<quint8(0x10)<<
//         quint8(0x20)<<quint8(0x22)<<quint8(0x21)<<
//         quint8(0x29)<<quint8(0x21)<<quint8(0x44)<<
//         quint8(0x83)<<quint8(0x00)<<quint8(0xFF);
    m_tcpSocket.write(block);
}

void SurfaceCommunication::slot_on_mainwindowUpdate(CYJData cyj)
{
    m_cyjData_actual.startdata1 = cyj.startdata1;
    m_cyjData_actual.startdata2 = cyj.startdata2;
    m_cyjData_actual.forward = cyj.forward;
    m_cyjData_actual.backward = cyj.backward;
    m_cyjData_actual.neutral = cyj.neutral;
    m_cyjData_actual.stop = cyj.stop;
    m_cyjData_actual.scram = cyj.scram;
    m_cyjData_actual.light = cyj.light;
    m_cyjData_actual.horn = cyj.horn;
    m_cyjData_actual.zero = cyj.zero;

    m_cyjData_actual.manualVisual = cyj.manualVisual;
    m_cyjData_actual.localRemote = cyj.localRemote;
    m_cyjData_actual.start = cyj.start;
    m_cyjData_actual.flameout = cyj.flameout;
    m_cyjData_actual.middle = cyj.middle;
    m_cyjData_actual.warn1 = cyj.warn1;
    m_cyjData_actual.warn2 = cyj.warn2;
    m_cyjData_actual.warn3 = cyj.warn3;

    m_cyjData_actual.rise = cyj.rise;
    m_cyjData_actual.fall = cyj.fall;
    m_cyjData_actual.turn = cyj.turn;
    m_cyjData_actual.back = cyj.back;
    m_cyjData_actual.left = cyj.left;
    m_cyjData_actual.right = cyj.right;
    m_cyjData_actual.acc = cyj.acc;
    m_cyjData_actual.deacc = cyj.deacc;
    m_cyjData_actual.speed = cyj.speed;
    m_cyjData_actual.engine = cyj.engine;
    m_cyjData_actual.spliceAngle = cyj.spliceAngle;
    m_cyjData_actual.temperature = cyj.temperature;
    m_cyjData_actual.oil = 0;
    m_cyjData_actual.enddata = 0xFF;
}

void SurfaceCommunication::slot_on_addTickCount()
{
    if(_doWorkCount<32768)
        _doWorkCount++;
}

void SurfaceCommunication::slot_on_connected()
{
    emit sig_statusTable("keep alive option set!");
    m_tcpSocket.setSocketOption(QAbstractSocket::KeepAliveOption,1);
}
//receive data from surface
void SurfaceCommunication::slot_on_readMessage()
{
    //qDebug()<<"slot_on_readMessage";
    int bytesNum = m_tcpSocket.bytesAvailable();
    //qDebug()<<"bytes avalaible:"<<m_tcpSocket.bytesAvailable();
    QByteArray allBytes = m_tcpSocket.readAll();
    if(bytesNum<NUMBERONEFRAME)
        return;
    else
    {
//        char *data = new char[NUMBERONEFRAME + 1];
//        strcpy(data, allBytes.right(NUMBERONEFRAME).data());
//        strcpy((char *)&m_cyjData_surface, data);
          QByteArray ba = allBytes.right(NUMBERONEFRAME);
          m_cyjData_surface.startdata1 = ba.at(0);
          m_cyjData_surface.startdata2 = ba.at(1);
          if((ba.at(2))%2==1)
          {
              m_cyjData_surface.forward = 1;
          }
          else
          {
              m_cyjData_surface.forward = 0;
          }
          if((ba.at(2)/2)%2==1)
          {
              m_cyjData_surface.backward = 1;
          }
          else
          {
              m_cyjData_surface.backward = 0;
          }
          if((ba.at(2)/4)%2==1)
          {
              m_cyjData_surface.neutral = 1;
          }
          else
          {
              m_cyjData_surface.neutral = 0;
          }
          if((ba.at(2)/8)%2==1)
          {
              m_cyjData_surface.stop = 1;
          }
          else
          {
              m_cyjData_surface.stop = 0;
          }

          if((ba.at(2)/16)%2==1)
          {
              m_cyjData_surface.scram = 1;
          }
          else
          {
              m_cyjData_surface.scram = 0;
          }

          if((ba.at(2)/32)%2==1)
          {
              m_cyjData_surface.light = 1;
          }
          else
          {
              m_cyjData_surface.light = 0;
          }

          if((ba.at(2)/64)%2==1)
          {
              m_cyjData_surface.horn = 1;
          }
          else
          {
              m_cyjData_surface.horn = 0;
          }
        //for decrease the speed when switch auto to manual
          m_cyjData_surface.zero = (ba.at(2)/128)%2;
         // qDebug()<<"on or off"<<m_cyjData_surface.zero;

          //extract control mode
          //remote manual---2
          //local manual---0
          //local auto---1
          //remote auto---3
          //qDebug()<<"CYJ surface ba.at(3)%4:"<<ba.at(3)%4;
          switch(ba.at(3)%4)
          {
          case 0:
              m_cyjData_surface.localRemote = 0;
              m_cyjData_surface.manualVisual = 0;
              break;
          case 1:
              m_cyjData_surface.localRemote = 0;
              m_cyjData_surface.manualVisual = 1;
              break;
          case 2:
              m_cyjData_surface.localRemote = 1;
              m_cyjData_surface.manualVisual = 0;
              break;
          case 3:
              m_cyjData_surface.localRemote = 1;
              m_cyjData_surface.manualVisual = 1;
              break;
          default:
              break;
          }
          //extract engine start
          m_cyjData_surface.start = (ba.at(3)/4)%2;
          //extract engine stop
          m_cyjData_surface.flameout = (ba.at(3)/8)%2;
          //extract engine switch medium
          m_cyjData_surface.middle = (ba.at(3)/16)%2;
          //extract warn1
          m_cyjData_surface.warn1 = (ba.at(3)/32)%2;
          //extract warn2
          m_cyjData_surface.warn2 = (ba.at(3)/64)%2;
          //extract warn3
          m_cyjData_surface.warn3 = (ba.at(3)/128)%2;

          m_cyjData_surface.rise = ba.at(4);
          m_cyjData_surface.fall = ba.at(5);
          m_cyjData_surface.turn = ba.at(6);
          m_cyjData_surface.back = ba.at(7);
          m_cyjData_surface.left = ba.at(8);
          m_cyjData_surface.right = ba.at(9);
          m_cyjData_surface.acc = ba.at(10);
          m_cyjData_surface.deacc = ba.at(11);
          m_cyjData_surface.speed = 0;
          m_cyjData_surface.engine = 0;
          m_cyjData_surface.spliceAngle = 0;
          m_cyjData_surface.oil = 0;
          m_cyjData_surface.temperature = 0;
          m_cyjData_surface.enddata = ba.at(17);
//        qDebug()<<"===========================================>";
//        qDebug()<<"data from surface forward:"<<m_cyjData_surface.forward;
//        qDebug()<<"data from surface backward:"<<m_cyjData_surface.backward;
//        qDebug()<<"data from surface neutral:"<<m_cyjData_surface.neutral;
//        qDebug()<<"data from surface stop:"<<m_cyjData_surface.stop;
//        qDebug()<<"data from surface scram:"<<m_cyjData_surface.scram;
//        qDebug()<<"data from surface light:"<<m_cyjData_surface.light;
//        qDebug()<<"data from surface horn:"<<m_cyjData_surface.horn;
//        qDebug()<<"data from surface zero:"<<m_cyjData_surface.zero;
//        qDebug()<<"data from surface start:"<<m_cyjData_surface.start;
//        qDebug()<<"data from surface flameout:"<<m_cyjData_surface.flameout;
//        qDebug()<<"data from surface middle:"<<m_cyjData_surface.middle;
//        qDebug()<<"data from surface localRemote:"<<m_cyjData_surface.localRemote;
//        qDebug()<<"data from surface manualVisual:"<<m_cyjData_surface.manualVisual;
//        qDebug()<<"data from surface rise:"<<m_cyjData_surface.rise;
//        qDebug()<<"data from surface fall:"<<m_cyjData_surface.fall;
//        qDebug()<<"data from surface turn:"<<m_cyjData_surface.turn;
//        qDebug()<<"data from surface back:"<<m_cyjData_surface.back;
//        qDebug()<<"data from surface left:"<<m_cyjData_surface.left;
//        qDebug()<<"data from surface right:"<<m_cyjData_surface.right;
//        qDebug()<<"data from surface acc:"<<m_cyjData_surface.acc;
//        qDebug()<<"data from surface deacc:"<<m_cyjData_surface.deacc;
//        m_tcpSocket.read((char *)&m_cyjData_surface,sizeof(m_cyjData_surface));
        if(m_cyjData_surface.startdata1==0xAA&&m_cyjData_surface.startdata2==0x55&&m_cyjData_surface.enddata==0xFF)
        emit sig_informMainwindow(m_cyjData_surface);
    }
}

void SurfaceCommunication::slot_on_error(QAbstractSocket::SocketError socktError)
{
    switch(socktError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        emit sig_statusTable("surface RemoteHostClosedError:"+m_tcpSocket.errorString());
        break;
    case QAbstractSocket::DatagramTooLargeError:
        emit sig_statusTable("surface DatagramTooLargeError:"+m_tcpSocket.errorString());
        break;
    case QAbstractSocket::NetworkError:
        emit sig_statusTable("surface NetworkError:"+m_tcpSocket.errorString());
        break;
    default:
        break;
    }
}


