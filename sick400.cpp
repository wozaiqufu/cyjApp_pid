#include "sick400.h"
#include <QtCore>
#include <QDebug>

SICK400::SICK400(QObject *parent) : QObject(parent)
{
    //typedef void (QAbstractSocket::*QAbstractSocketErrorSignal)(QAbstractSocket::SocketError);
    connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_on_readMessage()));
//   QObject::connect(&m_tcpSocket, &QTcpSocket::disconnected, this, &SICK400::slot_on_disconnected);
    connect(&m_tcpSocket,SIGNAL(disconnected()),this,SLOT(slot_on_disconnected()));
    //connect(&m_tcpSocket, static_cast<QAbstractSocketErrorSignal>(&QAbstractSocket::error), this, SLOT(slot_on_error(QAbstractSocket::SocketError)));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_on_error(QAbstractSocket::SocketError))/*,Qt::DirectConnection*/);
}

bool SICK400::init(const QString name, const QString ip, const int port)
{
    m_name = name;
    m_ip = ip;
    m_port = port;
    m_tcpSocket.abort();
    m_tcpSocket.connectToHost(m_ip, m_port);
    if (m_tcpSocket.waitForConnected(MILSECONDSWAIT))
    {
        emit sig_statusTable(m_name + " init succeeded");
        m_isOn = true;
        return true;
    }
    else return false;
}

void SICK400::continuousStart()
{
    //QString qstr("\x2sMN mLRreqdata 0020 1\x3");
    //QString qstr("sMN mLRreqdata 0020");
    //0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x11, 0x73,
    //0x4d, 0x4e, 0x20, 0x6d, 0x4c, 0x52, 0x72, 0x65, 0x71,
    //0x64, 0x61, 0x74, 0x61, 0x20, 0x32, 0x30, 0x77

    //send sMN mLRreqdata 0020
    QByteArray block;
    QDataStream dataS(&block,QIODevice::WriteOnly);
    dataS<<quint8(0x02)<<quint8(0x02)<<quint8(0x02)<<quint8(0x02)<<quint8(0x00)
        <<quint8(0x00)<<quint8(0x00)<<quint8(0x11)
        <<quint8(0x73)<<quint8(0x4d)<<quint8(0x4e)<<quint8(0x20)
        <<quint8(0x6d)<<quint8(0x4c)<<quint8(0x52)<<quint8(0x72)<<quint8(0x65)
       <<quint8(0x71)<<quint8(0x64)<<quint8(0x61)
      <<quint8(0x74)<<quint8(0x61)<<quint8(0x20)<<quint8(0x30)<<quint8(0x30)
     <<quint8(0x32)<<quint8(0x30)<<quint8(0x77);
//    emit sig_statusTable("SICK400 continuousStart block:"+block);
//    qDebug() << "request at:" << QTime::currentTime();
//    qDebug()<<"SICK400 continuousStart block:"<<block;
    m_tcpSocket.write(block);
}

void SICK400::continuousStop()
{
    //0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x0F, 0x73,
    //0x4D, 0x4E, 0x20, 0x6D, 0x4C, 0x52, 0x73, 0x74, 0x6F,
    //0x70, 0x64, 0x61, 0x74, 0x61, 0x2B
    QByteArray block;
    QDataStream dataS(&block,QIODevice::WriteOnly);
    dataS<<quint8(0x02)<<quint8(0x02)<<quint8(0x02)<<quint8(0x02)<<quint8(0x00)
        <<quint8(0x00)<<quint8(0x00)<<quint8(0x0F)
        <<quint8(0x73)<<quint8(0x4d)<<quint8(0x4e)<<quint8(0x20)
        <<quint8(0x6d)<<quint8(0x4c)<<quint8(0x52)<<quint8(0x73)<<quint8(0x74)
       <<quint8(0x6F)<<quint8(0x70)<<quint8(0x64)
      <<quint8(0x61)<<quint8(0x74)<<quint8(0x61)<<quint8(0x2B);
    emit sig_statusTable("SICK400 continuousStart block:"+block);
    qDebug() << "request at:" << QTime::currentTime();
    qDebug()<<"SICK400 continuousStart block:"<<block;
    m_tcpSocket.write(block);
}

bool SICK400::isOn()
{
    return m_isOn==true;
}

QString SICK400::name() const
{
    return m_name;
}

QString SICK400::ip() const
{
    return m_ip;
}

int SICK400::port() const
{
    return m_port;
}

void SICK400::extractData()
{
     if(m_dataRecieved.size()!=1097) return;
    m_distance.clear();
    m_ris.clear();
    char ba1,ba2,ba3;
    ba1 = m_dataRecieved.at(18);
    ba2 = m_dataRecieved.at(19);
    int dataNum = ba1 + 256*ba2;
    //qDebug()<<"SICK400 recv data number is:"<<dataNum;
    for(int i=0;i<dataNum;i++)
    {
        ba1 = m_dataRecieved.at(28 + 3*i);
        ba2 = m_dataRecieved.at(29 + 3*i);
        ba3 = m_dataRecieved.at(30 + 3*i);
        int dis = ba1 + 256*ba2;
        m_distance.append(dis);
        m_ris.append(ba3);
    }

    emit sigUpdateBeaconLength(beaconLength(250));
    //qDebug()<<"SICK400 size of distance:"<<m_distance.size();
    //qDebug()<<"SICK400 size of ris are:"<<m_ris.size();
    //qDebug()<<"SICK400 distance are:"<<m_distance;
    //qDebug()<<"SICK400 ris are:"<<m_ris;
    //qDebug()<<"SICK400 beacon length are:"<<beaconLength(250);
}

QVector<int> SICK400::beaconLength(const int threshold)
{
    QVector<int> binary_vec;
    QVector<int> beaconLength;
    //qDebug()<<"RIS are:"<<m_ris;
    //qDebug()<<"the dist are :"<<m_distance;
    for(int i = 0;i < m_ris.size();i++)
    {
        if(m_ris.at(i) > threshold)
            binary_vec.push_back(1);
        else
            binary_vec.push_back(0);
    }
    //qDebug()<<"before Pro binary_vec is :"<<binary_vec;
    binary_vec = Pro_binary(binary_vec);
    //qDebug()<<"after Pro binary_vec is :"<<binary_vec;
    //qDebug()<<"the dist is :"<<m_SICKdata;
    QVector<int> pos01_binary_vec ;//index of "01"
    QVector<int> pos10_binary_vec ;//index of "10"
    if(binary_vec.at(0) == 1)
    {
        pos01_binary_vec.push_back(0);
    }
    for(int i = 0;i < binary_vec.size() -1; i++)
    {
        int RSSIX = binary_vec.at(i+1)- binary_vec.at(i);
        if(RSSIX == 1)
        {
            pos01_binary_vec.push_back(i + 1) ;
        }
        else if(RSSIX == -1)
        {
            pos10_binary_vec.push_back(i) ;
        }
    }

    if(binary_vec.last() == 1)
    {
        pos10_binary_vec.push_back(binary_vec.size()-1) ;
    }
    //qDebug()<<"the pos of 01 in RSSI is :"<<pos01_binary_vec;
    //qDebug()<<"the pos of 10 in RSSI is :"<<pos10_binary_vec;
    //angle = pos10_binary_vec - pos10_binary_vec ;
    beaconLength.clear();
    if(pos01_binary_vec.size() == pos10_binary_vec.size())
    {
        int dist1_beacon = 0;
        int dist2_beacon = 0;
        for(int ix = 0;ix < pos10_binary_vec.size(); ++ix)
        {
            if(binary_vec.first()==1||binary_vec.last()==1) continue;
            double angle_num = pos10_binary_vec.at(ix) - pos01_binary_vec.at(ix);
            if(angle_num < 2) continue;
            double angle_beacon = angle_num * ANGLEDEGREE2RADIUS * ANGLERESOLUTION;
            dist1_beacon = m_distance.at(pos01_binary_vec.at(ix)-1);
            dist2_beacon = m_distance.at(pos10_binary_vec.at(ix)+1);
            int temp3 = sqrt(pow(dist1_beacon,2) + pow(dist2_beacon,2)- 2*dist1_beacon*dist2_beacon*cos(angle_beacon));
            temp3 = temp3 - (dist1_beacon+dist2_beacon)*ANGLEDEGREE2RADIUS * ANGLERESOLUTION;
//            qDebug()<<"dist1_beacon "<<dist1_beacon;
//            qDebug()<<"dist2_beacon "<<dist2_beacon;
//            qDebug()<<"angle_beacon "<<angle_beacon;
            beaconLength.push_back(temp3);
        }
    }
    return beaconLength;
}

QVector<int> SICK400::Pro_binary(QVector<int> vec) const
{
    QVector<int> Pro_vec = vec;
    QVector<int> expansion_vec ;
    QVector<int> corrosion_vec ;
    //expansion
    if(Pro_vec.at(0)+Pro_vec.at(1))
    {
        expansion_vec.push_back(1);
    }
    else
    {
        expansion_vec.push_back(0);
    }
    for(int ix = 1; ix < Pro_vec.size()-1; ++ix)
    {
        if(Pro_vec.at(ix-1) + Pro_vec.at(ix) + Pro_vec.at(ix+1))
        {
            expansion_vec.push_back(1);
        }
        else
        {
            expansion_vec.push_back(0);
        }
    }
    if(Pro_vec.last() + Pro_vec.at(Pro_vec.size()-2))
        {
            expansion_vec.push_back(1);
        }
        else
        {
            expansion_vec.push_back(0);
        }
        //corrosion
        corrosion_vec.push_back(expansion_vec.at(0) * expansion_vec.at(1));
        for(int ix = 1; ix < expansion_vec.size()-1; ++ix)
        {
            corrosion_vec.push_back(expansion_vec.at(ix-1) * expansion_vec.at(ix) * expansion_vec.at(ix+1));
        }
        corrosion_vec.push_back(expansion_vec.last() * expansion_vec.at(expansion_vec.size()-2));

        return corrosion_vec;
}

void SICK400::slot_on_readMessage()
{
    m_dataRecieved.clear();
    m_dataRecieved = m_tcpSocket.readAll();
    if(m_dataRecieved.size()==0)
    {
        emit sig_statusTable("SICK400 error:no data");
        return;
    }
    //qDebug()<<"SICK 400 recv frame length is:"<<m_dataRecieved.size()<<endl;
    extractData();
}

void SICK400::slot_on_disconnected()
{
    qDebug()<<"slot_on_disconnected()";
}

void SICK400::slot_on_error(QAbstractSocket::SocketError socktError)
{
    qDebug()<<"SICK 400 error";
    switch(socktError)
    {
    case QAbstractSocket::RemoteHostClosedError:
        emit sig_statusTable("SICK400 RemoteHostClosedError:"+m_tcpSocket.errorString());
        break;
    case QAbstractSocket::DatagramTooLargeError:
        emit sig_statusTable("SICK400 DatagramTooLargeError:"+m_tcpSocket.errorString());
        break;
    case QAbstractSocket::NetworkError:
        emit sig_statusTable("SICK400 NetworkError:"+m_tcpSocket.errorString());
        break;
    default:
        break;
    }
    m_isOn = false;
}

