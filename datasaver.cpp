#include "datasaver.h"
#include <cmath>
#include <QDebug>
#include <QStringList>
using namespace std;

DataSaver::DataSaver(QObject *parent) : QObject(parent)
  ,m_lastDis(0)
{
    m_param.dis = 0;
    m_param.velocity = 0;
    m_param.angle = 0;
    m_param.courseAngle = 0;
    m_param.acc = 0;
    m_param.offset = 0;
    m_param.angleOrg = 0;
}

void DataSaver::openFiles()
{
      file.setFileName("path.txt");
      if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
          return;
      stream.setDevice(&file);

      file1.setFileName("beacon.txt");
      if (!file1.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
          return;
      stream1.setDevice(&file1);
}

void DataSaver::slot_doWork()
{
   // qDebug()<<"m_param.dis"<<m_param.dis;
//    if(abs(m_param.dis-m_lastDis-disDelta)<50)
    if(m_param.dis!=m_lastDis)
    {
        qDebug()<<m_param.dis<<","<<
                  m_param.velocity<<","<<
                  m_param.angle<<","<<
                  m_param.courseAngle<<","<<
                  m_param.acc<<","<<
                  m_param.offset<<","<<
                  m_param.angleOrg<<","<<
                  m_param.hydraulic;
        m_lastDis = m_param.dis;
    }
}

void DataSaver::slot_update_dis(int dis)
{
    //qDebug()<<"distance:"<<m_param.dis;
    m_param.dis = dis;
}

void DataSaver::slot_update_angle(int an)
{
    m_param.angle = an - ANGLEMIDDLE;
}

void DataSaver::slot_update_velocity(int ve)
{
    m_param.velocity = ve;
}

void DataSaver::slot_update_courseAngle(int courseAngle)
{
    m_param.courseAngle = courseAngle;
}

void DataSaver::slot_update_lateralOffset(int offset)
{
    m_param.offset = offset;
}

void DataSaver::slot_update_acc(int acc)
{
    m_param.acc = acc;
}

void DataSaver::slot_on_update_angleOrg(int anOrg)
{
    m_param.angleOrg = anOrg;
}

void DataSaver::slot_on_update_hydraulic(int hyd)
{
    m_param.hydraulic = hyd;
}



