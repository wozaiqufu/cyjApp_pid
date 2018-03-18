#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QThread>
#include <QList>
#include "SICK511.h"
#include "sick400.h"
#include "CAN.h"
#include "surfacecommunication.h"
#include "autoalgorithm.h"
#include "cyjdata.h"
#include "datasaver.h"
//test only
#include "trackmemory.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum Direction{Forward = 0,Backward};//default Forward==0
    enum ControlMode{Local,Visible,Remote,Auto};//default Local==0
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
     void updateControlMode();
     void initCYJActualData();
     void initCYJSurfaceData();
     void updateAcc();
     void updateDirection();
     void initDataSaver();
     void updateData2DataSaver();
     void updateData2Surface();
     void updateData2Algorithm();
     void loadData();
     void QString_to_Int(QString str, QList <int> &data);
     bool checkSensorsAndSurface();
signals:
    void sig_stopPermanentReq();
    void sig_informInfo2surface(CYJData);
    void sig_informAlgrithmMile(int);
    void sig_speed2Algorithm(int);
    void finished();
    void sig_statusTable(QString);
    void sig_spliceAngle2Algorithm(int);
    void sig_mile(int);
    void sig_velocity(int);
    void sig_angle(int);
    void sig_acc(int);
    void sig_angleOrg(int);//angle read in path.txt
    void sig_hydraulic(int);
    void sig_diretion2Alg(bool);
    void sig_addTickCount();
    //for test
    void sig_angleCmm2Algorithm(int);
private slots:
    void slot_on_initSICK511();
    void slot_on_stopSICK511();
    bool slot_on_initCAN();
    void slot_on_initSICK400();
    void slot_on_stopSICK400();
    void slot_on_readFrame();
    void slot_on_initSurface();
    void slot_on_mainTimer_timeout();
    void slot_on_mileAccumulator_timeout();
    void slot_on_startAccumMile();
    void slot_on_stopAccumMile();
    void slot_on_acc50();
    void slot_on_acc85();
    void slot_on_acc90();
    void slot_on_acc95();
    void slot_on_acc100();

public slots:
    void slot_on_updateStatusTable(QString qstr);
    void slot_on_updateCAN304(QVector<int> vec);
    void slot_on_updateCAN305(QVector<int> vec);
    void slot_on_surfaceUpdate(CYJData cyj);
    void slot_on_updateBeaconLength(QVector<int> vec);
private:
    Ui::MainWindow *ui;
    CYJData m_cyjData_surface;
    CYJData m_cyjData_actual;
	SICK511 m_sick511_f;//Forward SICK511
	SICK511 m_sick511_b;//Backward SICK511
    SICK400 m_sick400;
    CAN m_can;
    DataSaver               m_dataSaver;
    SurfaceCommunication	m_surfaceComm;
    autoAlgorithm			m_algorithm;
    QTimer					m_timer_Teach;
    QTimer					m_timer_CAN;
    QTimer					m_timer_DataSaver;
    QThread					m_thread_CAN;
    QThread                 m_thread_dataSaver;
    QTimer					m_timer_main;
    QTimer					m_timer_surface;
    QTimer                  m_timer_mileAccumulator;
    QMap<int, int>          m_map;
	ControlMode m_controlMode;
    int m_mileInstant;//in cm
    int m_mileMeter;
    int m_speed;
    int _acc;
    int _deacc;
    bool _sensorLost;
    bool _forward;
    bool _backward;
    bool _neutral;
    bool _stop;
    int _maintimerCount;
    bool _isBeaconFound;
    int  _hydraulic;
    static const int LEFTLIMIT = 4;
    static const int RIGHTLIMNIT = 83;
};

#endif // MAINWINDOW_H
