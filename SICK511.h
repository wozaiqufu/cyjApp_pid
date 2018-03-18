#ifndef _NETACCESS_H_
#define _NETACCESS_H_

#include <QDir>
#include <QStringList>
#include <QByteArray>
#include <QTcpSocket>
#include <math.h>
#include <QtNetwork/QHostAddress>

class SICK511 : public QObject
{
    Q_OBJECT
    public:
		SICK511(QObject *parent = 0);
        bool	init(const QString name,const QString ip,const int port);
		void	continuousStart();
		void	continuousStop();
		bool	isOn();
        void    useData(const bool b);
		QString name()	const;
		QString ip()	const;
		int		port()	const;
        int     doWorkCount() const;
		~SICK511();
private:
        void	extractDISTData();
        void	extractRSSIData();
        int		courseAngle();
        int		lateralOffset();
        int     frontDistance();
		bool    isDataLoaded();
		int		angle2index(const int angle);
        void    splitByte(QString str, QList <QString> &data);
signals:
    void	sigUpdateCourseAngle(int);
    void	sigUpdateLateralOffset(int,QString);
    void	sig_statusTable(QString);
public slots:
    void slot_on_addTickCount();
private slots:
    void slot_on_readMessage();
    void slot_on_error(QAbstractSocket::SocketError socktError);
private:
	QString m_name;
	QString m_ip;
	int		m_port;
    bool	m_isOn;//is false when data is not fresh
    bool    m_dataInUse;
    static const double	m_PI = 3.141592653;
    static const int	MILSECONDSWAIT = 2000;//wait maximum seconds when establish the TCP connection
    QByteArray          m_dataRecieved;
    QTcpSocket          m_tcpSocket;
    QVector<int>        m_DISTdata;//final distance data is here!
    QVector<int>        m_RSSIdata;//final RSSI data is here!
    int                 m_numberDataOneSocket;
    double              m_angleResolution;
    int                 m_courseAngle;//for lateral offset calculation!
    int                 m_lateralOffset;
    int                 m_c_STARTANGLE;
    int                 m_c_L1L2ANGLE;
    int                 m_c_ANGLESACLE;
    int                 _doWorkCount;
    static const double	PI = 3.141592653;
    static const double	ANGLEDEGREE2RADIUS = 0.0174532925;
    static const int	FRONT_ANGLE_START = 75;
    static const int	FRONT_ANGLE_END = 105;
    static const int    SIZEONEFRAME = 1670;
    static const int    DATANUMBER = 361;
    static const double ANGLERESOLUTION = 0.5;
};
#endif
