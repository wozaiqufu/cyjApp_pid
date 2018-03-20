#include "autoalgorithm.h"

#include "trackmemory.h"
#include <math.h>
#include <QtCore>
#include <QVector>
#include <QDebug>

autoAlgorithm::autoAlgorithm(QObject *parent) : QObject(parent),
  m_mile_saved(0),
  m_mile_current(0),
  m_left(0),
  m_right(0),
  m_acc(0),
  m_spliceAnlge(0),
  m_trackAngle(0),
  m_angleCmm(0),
  m_lateralOffset(0),
  _SICKName(),
  p_disntancePID(0.005,20,-20,0.15,0.0005,0.8),
    p_anglePID    (0.005,85,-80,7,0.008,0.5),
    p_TrackAnglePID(0.005,85,-80,20,0.2,40)





//  p_speedPID    (0.005,105,0,01,0.001,0.01)
{
    //p_track = new TrackMemory;
    //p_disntancePID = new PID_Incremental(0.005,20,-20,0.06,0.0005,0.0001);
     //p_disntancePID = new PID(0.005,20,-20,0.08,0.0005,0.0001);


    //p_disntancePID = new PID(0.005,20,-20,0.15,0.0005,0.5);


//    p_anglePID = new PID(0.005,80,-80,6,0.005,0.5);//add 30 and 45
    // p_anglePID = new PID(0.005,80,-80,10,0.005,0.5);
    //p_anglePID = new PID(0.005,80,-80,15,0.005,0.5);


    //p_anglePID = new pid_angle(0.005,80,-80,6,0.008,0.5);
    p_disntancePID.setAllowError(15);
    p_anglePID.setAllowError(0.4);
    p_TrackAnglePID.setAllowError(0.4);
}

void autoAlgorithm::update()
{
    if(_SICKName=="forward")
        qDebug()<<"CLASS Alg. Direction: FORWARD,m_lateralOffset"<<m_lateralOffset;
    else
        qDebug()<<"CLASS Alg. Direction: BACKWARD,m_lateralOffset"<<m_lateralOffset;

    double deltaSpliceAngle = p_disntancePID.calculate(500,m_lateralOffset);
    //qDebug()<<"m_lateralOffset:"<<m_lateralOffset;
    double angleSet = 0-deltaSpliceAngle;
    //qDebug()<<"angleSet:"<<angleSet;
    double pidOut = p_anglePID.calculate(angleSet,m_spliceAnlge);
    //qDebug()<<"m_spliceAnlge:"<<m_spliceAnlge;
    //qDebug()<<"pidOut:"<<pidOut;
    //speed PID
    //double acc = p_speedPID.calculate(1380,m_speed);
    if(pidOut<0)
    {
        m_right = 0;
        m_left = -pidOut;
        m_acc = 0;
    }
    else
    {
        m_right = pidOut;
        m_left = 0;
        m_acc = 0;
    }

    //memory alg.
//    qDebug()<<"m_trackAngle:"<<m_trackAngle;
//    qDebug()<<"m_spliceAnlge:"<<m_spliceAnlge;
//    double pidOut = p_TrackAnglePID.calculate(m_trackAngle,m_spliceAnlge);
//    if(pidOut<0)
//    {
//        m_right = 0;
//        m_left = -pidOut;
//        m_acc = 0;
//    }
//    else
//    {
//        m_right = pidOut;
//        m_left = 0;
//        m_acc = 0;
//    }
}

void autoAlgorithm::finish()
{

}

int autoAlgorithm::left() const
{
    return m_left;
}

int autoAlgorithm::right() const
{
     return m_right;
}

int autoAlgorithm::accelerator() const
{
     return m_acc;
}

int autoAlgorithm::deaccelerator() const
{
    return 0;
}

double autoAlgorithm::angle2Radius(const double an)
{
    return an*0.0174532925;
}

void autoAlgorithm::slot_on_updateSICK511InUse(QString str)
{
    _SICKName = str;
}

void autoAlgorithm::slot_on_updateBeaconLength(QVector<int> vec)
{
    m_beaconLength = vec;
}

void autoAlgorithm::slot_on_updateCourseAngle(int angle)
{
    m_courseAngle = angle;
    //qDebug()<<"course angle is :"<<angle;
}

void autoAlgorithm::slot_on_updateLateralOffset(int of,QString SICKName)
{
    _SICKName = SICKName;
    qDebug()<<"=======================_SICKName:"<<_SICKName;
    if(_SICKName=="backward")
        m_lateralOffset = of/10;
    else
    {
        m_lateralOffset = of/10 - DIS_SCRAPER2LASER*cos(angle2Radius(90-m_courseAngle));
    }
    //qDebug()<<"m_lateralOffset is :"<<of;
}

void autoAlgorithm::slot_on_updateMile(int mile)
{
    m_mile_current = mile;
}

void autoAlgorithm::slot_on_updateControlInfo(QVector<int> vec)
{
	m_mile_acc_deacc_left_right = vec;
    m_mile_current = vec.at(0);
}

void autoAlgorithm::slot_on_updateSpliceAngle(int angle)
{
    m_spliceAnlge = angle;
}

void autoAlgorithm::slot_on_updateSpeed(int speed)
{
    m_speed = speed;
}

void autoAlgorithm::slot_on_updateTrackAngle(int angle)
{
    m_trackAngle = angle;
}

void autoAlgorithm::slot_on_updateAngleCommand(int angle)
{
    m_angleCmm = angle;
}
