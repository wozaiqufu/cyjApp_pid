#include "autoalgorithm.h"
#include "pid.h"
#include "trackmemory.h"
#include <QtCore>
#include <QVector>
#include <QDebug>

autoAlgorithm::autoAlgorithm(QObject *parent) : QObject(parent),
  m_isAuto(true),
  m_stage(Auto),
  m_type(PIDType),
  m_mile_saved(0),
  m_mile_current(0),
  m_left(0),
  m_right(0),
  m_acc(0),
  m_spliceAnlge(0),
  m_angleCmm(0)
{
    //p_track = new TrackMemory;
    p_disntancePID = new PID(0.1,20,-20,0.08,0.05,0.00001);
    p_anglePID = new PID(0.005,80,-80,6,0.08,0.14);
    p_disntancePID->setAllowError(15);
    p_anglePID->setAllowError(0.4);
}

void autoAlgorithm::update()
{
    double spliceAngle = p_disntancePID->calculate(400,m_lateralOffset);
     qDebug()<<"m_lateralOffset:"<<m_lateralOffset;
    qDebug()<<"distance PID output:"<<0-spliceAngle;
    double pidOut = p_anglePID->calculate(0-spliceAngle,m_spliceAnlge);
//    double pidOut = p_anglePID->calculate(m_angleCmm,m_spliceAnlge);
    //qDebug()<<"m_angleCmm:"<<m_angleCmm;
    qDebug()<<"spliceAnlge:"<<m_spliceAnlge;
    if(pidOut<0)
    {
        m_right = 0;
        m_left = -pidOut+30;
        m_acc = 45;
    }
    else
    {
        m_right = pidOut+35;
        m_left = 0;
        m_acc = 45;
    }

}

void autoAlgorithm::finish()
{
    if(m_stage==Teach)
        p_track->saveAll();
}
void autoAlgorithm::setStageType(const int type)
{
	switch (type)
	{
	case 0:
		m_stage = Teach;
		break;
	case 1:
		m_stage = Auto;
		break;
	default:
		break;
	}
}

void autoAlgorithm::setAlgorithmType(const int type)
{
    switch(type)
    {
    case 0:
        m_type = PIDType;
        break;
    case 1:
        m_type = TrackMemoryType;
        break;
    default:
        break;
    }
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

void autoAlgorithm::testTrackMemory()
{
    qDebug()<<"autoAlgorithm::testTrackMemory";
    //p_track->loadData("beaconRaw.txt");
    p_track->saveBeacon(200,1000);
    p_track->savePath(1000,43,0,10,0);
}

void autoAlgorithm::testTrackMemory2()
{
    qDebug()<<"autoAlgorithm::testTrackMemory2";
    p_track->saveAll();
}

void autoAlgorithm::loadData()
{
    qDebug()<<"autoAlgorithm::loadData";
    p_track->loadData("beaconRaw.dat");
    p_track->loadData("path.dat");
    p_track->loadData("beacon.dat");
}

void autoAlgorithm::slot_on_updateControlMode(bool isAuto)
{
    m_isAuto = isAuto;
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

void autoAlgorithm::slot_on_updateLateralOffset(int of)
{
    m_lateralOffset = of/10;
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

void autoAlgorithm::slot_on_updateAngleCommand(int angle)
{
    m_angleCmm = angle;
}

bool autoAlgorithm::isCertainMileIncrement(const int mile)
{
	//10% of m_mileDelta
	if ((mile - m_mile_saved) <= 0) return false;
    return (abs(mile - m_mile_saved - MILEDELTA) < 0.1*MILEDELTA);
}
