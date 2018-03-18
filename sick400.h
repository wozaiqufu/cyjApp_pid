#ifndef SICK400_H
#define SICK400_H
#include <QDir>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <math.h>
#include <QtNetwork/QHostAddress>
#include <QObject>

class SICK400 : public QObject
{
    Q_OBJECT
public:
    explicit SICK400(QObject *parent = 0);
    bool	init(const QString name,const QString ip,const int port);
    void	continuousStart();
    void	continuousStop();
    bool	isOn();//is false when data is not fresh
    QString name()	const;
    QString ip()	const;
    int		port()	const;
    int     doWorkCount() const;
signals:
    void	sig_statusTable(QString);
    void    sigUpdateBeaconLength(QVector<int> vec);
private:
    void    extractData();
    void    extractData_new();
    void    splitByte(QString str, QList <QString> &data);
    QVector<int>  beaconLength(const int threshold);
    QVector<int>  Pro_binary(QVector<int> vec) const;
public slots:
    void slot_on_addTickCount();
private slots:
    void        slot_on_readMessage();
    void        slot_on_disconnected();
    void        slot_on_error(QAbstractSocket::SocketError socktError);
private:
    QString             m_name;
    QString             m_ip;
    int                 m_port;
    bool                m_isOn;
    QByteArray          m_dataRecieved;
    QTcpSocket          m_tcpSocket;
    QList<int>          m_distance;
    QList<int>          m_ris;
    int                 _doWorkCount;
    static const int	MILSECONDSWAIT = 2000;
    static const double	ANGLEDEGREE2RADIUS = 0.0174532925;
    static const int	STARTANGLE = 75;
    static const int	ENDANGLE = 105;
    static const double	ANGLERESOLUTION = 0.2;
    static const int    SIZEONEFRAME = 1097;
    static const int    DATASIZE = 350;
};

#endif // SICK400_H
