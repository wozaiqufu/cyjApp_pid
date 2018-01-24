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
		~SICK511();
private:
        void	extractDISTData();
        void	extractRSSIData();
        int		courseAngle();
        int		lateralOffset();
        int     frontDistance();
		bool    isDataLoaded();
		int		angle2index(const int angle);
signals:
    void	sigUpdateCourseAngle(int);
    void	sigUpdateLateralOffset(int);
    void	sig_statusTable(QString);
private slots:
    void slot_on_readMessage();
    void slot_on_error(QAbstractSocket::SocketError socktError);
private:
	QString m_name;
	QString m_ip;
	int		m_port;
	bool	m_isOn;
    bool    m_isInForward;
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
//    static const double m_pi = 3.141592653;
//    static const double m_Angle_degree2Radian = 0.0174532925;
//    static const int STARTANGLE = 0;
//    static const int L1L2ANGLE = 30;
//    static const int m_angleDeltaMax = 30;
//    static const int m_angleResolution = 1;
    static const double	PI = 3.141592653;
    static const double	ANGLEDEGREE2RADIUS = 0.0174532925;
    static const int	STARTANGLE = 0;
    static const int	L1L2ANGLE = 30;
    static const int	ANGLESACLE = 30;
    static const int	FRONT_ANGLE_START = 75;
    static const int	FRONT_ANGLE_END = 105;
};
#endif
