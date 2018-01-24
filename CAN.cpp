#include "CAN.h"
#include <QDebug>

CAN::CAN(QObject *parent) : QObject(parent)
  ,m_s(0) {
}

bool CAN::init(const int portIndex)
{
    int ret;
    if(0 == portIndex)
    {//CAN0 is used!
   /*********************step 1*******************************************************************/
        //create the CAN Socket
        srand(time(NULL));
        m_s = socket(PF_CAN,SOCK_RAW,CAN_RAW);
        if(m_s<0)
        {
            emit sig_statusTable("CAN initialization failed!");
            return false;
        }

    /*********************step 2*******************************************************************/
    //bind socket to specific CAN port
    strcpy(m_ifr.ifr_name,"can0");
    //strcpy(m_ifr.ifr_name,"can1");
    //qDebug()<<"m_ifr.ifr_name:"<<m_ifr.ifr_name;
    ret = ioctl(m_s,SIOCGIFINDEX,&m_ifr);
    if(ret<0)
    {
        qDebug()<<"ioctl failed!";
        return false;
    }
    m_addr.can_family = PF_CAN;
    m_addr.can_ifindex = m_ifr.ifr_ifindex;
    ret = bind(m_s,(struct sockaddr*)&m_addr,sizeof(m_addr));
    if(ret<0)
    {
        qDebug()<<"bind failed!";
        emit sig_statusTable("bind failed!");
        return false;
    }
    emit sig_statusTable("init CAN succeed!");
    /*******************setup CAN filters***********************************************************************************/
//    m_filter[0].can_id = 0x304|CAN_EFF_FLAG;
//    m_filter[0].can_mask = 0xFFF;
//    m_filter[1].can_id = 0x305|CAN_EFF_FLAG;
//    m_filter[1].can_mask = 0xFFF;
//    ret = setsockopt(m_s,SOL_CAN_RAW,CAN_RAW_FILTER,&m_filter,sizeof(m_filter));
//    if(ret<0) {
//        qDebug()<<"filter setup failed!";
//    }
    }
}

void CAN::slot_on_sendFrame(ulong id, uchar length, uchar *data)
{
    m_frameSend.can_id   =   id;
    m_frameSend.can_dlc =   length;
    for(uchar i=0; i < length; i++)
    m_frameSend.data[i] = data[i];
    //printFrame(&m_frameSend);
    int nbytes=write(m_s,&m_frameSend,sizeof(m_frameSend));
    if (nbytes < 0) {
        qDebug()<<"======================";
        handle_err_frame(&m_frameSend);
        emit sig_statusTable("Send message error senddata\n");
    }
    else
    {
        emit sig_statusTable("CAN Send message succeed");
    }
}

void CAN::slot_dowork()
{
    m_tv.tv_sec = 1;
    m_tv.tv_usec = 0;
    FD_ZERO(&m_rset);
    FD_SET(m_s,&m_rset);
    int ret = select(m_s+1,&m_rset,NULL,NULL,NULL);
    if(0 == ret) {
        qDebug()<<"select timeout!";
    }
    ret = read(m_s,&m_frameRecv,sizeof(m_frameRecv));
    if(0 == ret) {
        qDebug()<<"sead failed!";
    }
    m_CAN304.clear();
    m_CAN305.clear();
    for(int i=0;i<m_frameRecv.can_dlc;i++)
    {
        if(m_frameRecv.can_id == 0x304)
        {
            m_CAN304.push_back(m_frameRecv.data[i]);
        }
        else if(m_frameRecv.can_id == 0x305)
        {
            m_CAN305.push_back(m_frameRecv.data[i]);
        }
        else
        {
            return;
        }
    }
    emit sigUpdateCAN304(m_CAN304);
    emit sigUpdateCAN305(m_CAN305);
    //printFrame(&m_frameRecv);
}

void CAN::printFrame(can_frame *frame)
{
    qDebug()<<"can_id:"<<frame->can_id;
    qDebug()<<"dlc:"<<frame->can_dlc;
    //printf("%08x\n",frame->can_id & CAN_EFF_MASK);
    //printf("dlc=%d\n",frame->can_dlc);
    printf("data = ");
    for(int i=0;i<frame->can_dlc;i++) {
        //printf("%02x",frame->data[i]);
        qDebug()<<frame->data[i];
    }
}

void CAN::handle_err_frame(const can_frame *fr)
{
    if(fr->can_id & CAN_ERR_TX_TIMEOUT)
    {
        qDebug()<<"CAN_ERR_TX_TIMEOUT";
    }
    if(fr->can_id & CAN_ERR_LOSTARB)
    {
        qDebug()<<"CAN_ERR_LOSTARB";
        qDebug()<<fr->data[1];
    }

    if(fr->can_id & CAN_ERR_CRTL)
    {
        qDebug()<<"CAN_ERR_CRTL";
        qDebug()<<fr->data[1];
    }

    if(fr->can_id & CAN_ERR_PROT)
    {
        qDebug()<<"CAN_ERR_PROT";
        qDebug()<<fr->data[2];
        qDebug()<<fr->data[3];
    }

    if(fr->can_id & CAN_ERR_TRX)
    {
        qDebug()<<"CAN_ERR_TRX";
        qDebug()<<fr->data[4];
    }

    if(fr->can_id & CAN_ERR_BUSOFF)
    {
        qDebug()<<"CAN_ERR_BUSOFF";
    }

    if(fr->can_id & CAN_ERR_BUSERROR)
    {
        qDebug()<<"CAN_ERR_BUSERROR";
    }

    if(fr->can_id & CAN_ERR_RESTARTED)
    {
        qDebug()<<"CAN_ERR_RESTARTED";
    }
}

