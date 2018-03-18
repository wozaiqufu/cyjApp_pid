#ifndef AUTOALGORITHM_H
#define AUTOALGORITHM_H
#include <QVector>
#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QTime>
#include "pid.h"


class TrackMemory;
typedef struct PIDParams{
    double Kp;
    double Ki;
    double Kd;
}PIDP;

class autoAlgorithm : public QObject
{
    Q_OBJECT
public:
    explicit autoAlgorithm(QObject *parent = 0);
	void	update();
    void finish();
    int     left() const;
    int     right() const;
    int     accelerator() const;
    int     deaccelerator() const;
private:
    double angle2Radius(const double an);
signals:
    void sig_statusTable(QString);
public slots:
    void slot_on_updateSICK511InUse(QString str);
    void slot_on_updateBeaconLength(QVector<int> vec);
    void slot_on_updateCourseAngle(int angle);
    void slot_on_updateLateralOffset(int of,QString SICKName);
    void slot_on_updateMile(int mile);
	void slot_on_updateControlInfo(QVector<int> vec);
    void slot_on_updateSpliceAngle(int angle);
    void slot_on_updateSpeed(int speed);
    void slot_on_updateTrackAngle(int angle);
    //for test
    void slot_on_updateAngleCommand(int angle);
private:
    int                 m_courseAngle;
    int                 m_lateralOffset;
    QVector<int>        m_beaconLength;
    QVector<int>        m_mile_acc_deacc_left_right;//signal from mainwindow
    int                 m_mile_saved;//saved data into path.txt
    int                 m_mile_current;//current mile
    QTime               m_time;
    TrackMemory         *p_track;
    PID                 p_disntancePID;
    PID                 p_anglePID;
    PID                 p_TrackAnglePID;
    static const int    MILEDELTA = 20;//path.txt mile increment is 20cm
    static const double RATIO = 0.8;//k1*track + k2*pid k2=RATIO
    int                 m_left;
    int                 m_right;
    int                 m_acc;
    int                 m_spliceAnlge;
    int                 m_speed;
    int                 m_trackAngle;
    QString             _SICKName;
    //for test
    int                 m_angleCmm;
    static const int    DIS_SCRAPER2LASER = 350;//in cm
    static const double ANGLE_SCRAPTER2LASER = 14.5;
};
#endif // AUTOALGORITHM_H
