#ifndef CANOBJ_H
#define CANOBJ_H
#include <stdio.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/socket.h>
#include <linux/can.h>
#include <linux/can/error.h>
#include <linux/can/raw.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <error.h>
#include <time.h>
#include <QObject>
#include <QMutex>
#include <QVector>

#ifndef AF_CAN
#define AF_CAN 29
#endif

#ifndef PF_CAN
#define AF_CAN AF_CAN
#endif

#define CAN_SFF_MASK 0x000007FFU
#define CAN_EFF_MASK 0x1FFFFFFFU
#define CAN_ERR_MASK 0x1FFFFFFFU

class CAN : public QObject
{
    Q_OBJECT
public:
    explicit CAN(QObject *parent = 0);
     bool init(const int portIndex);
signals:
    void sigUpdateCAN304(QVector<int>);
    void sigUpdateCAN305(QVector<int>);
    void sig_statusTable(QString);
public slots:
    void slot_on_sendFrame(ulong id, uchar length, uchar *data);
    void  slot_dowork();//timer to refresh data
private:
    void printFrame(struct can_frame* frame);
    void handle_err_frame(const can_frame *fr);
private:
    int m_s;
    struct ifreq m_ifr;
    struct sockaddr_can m_addr;
    struct can_frame m_frameSend;
    struct can_frame m_frameRecv;
    struct can_filter m_filter[2];
    struct timeval m_tv;
    fd_set m_rset;
    QVector<int> m_CAN304;
    QVector<int> m_CAN305;
};

#endif // CANOBJ_H
