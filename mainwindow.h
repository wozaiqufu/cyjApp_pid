#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTableWidget>
#include <QThread>
#include "SICK511.h"
#include "sick400.h"
#include "CAN.h"
#include "surfacecommunication.h"
#include "autoalgorithm.h"
#include "cyjdata.h"
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
     void initStatusTable();
     void checkControlMode();
     void initCYJActualData();
signals:
    void sig_stopPermanentReq();
    void sig_informDirection(int);
    void sig_informInfo2surface(CYJData);
    void sig_autoInfo2Algorithm(bool);
    void sig_informAlgrithmMile(int);
    void finished();
    void sig_statusTable(QString);
    void sig_spliceAngle2Algorithm(int);
    //for test
    void sig_angleCmm2Algorithm(int);
private slots:
    void slot_on_initSICK511();
    void slot_on_stopSICK511();
    void slot_on_initCAN();
    void slot_on_initSICK400();
    void slot_on_stopSICK400();
    void slot_on_readFrame();
    void slot_on_initSurface();
    void slot_on_testAlgorithm();
    void slot_on_testAlgorithm2();
    void slot_on_testAlgorithmLoadData();
    void slot_on_startTeach();
    void slot_on_mainTimer_timeout();
    void slot_on_teachTimer_timeout();
    void slot_on_mileAccumulator_timeout();
    void slot_on_setAlgorithm();
	void slot_on_setMode();
    void slot_on_startAccumMile();
    void slot_on_stopAccumMile();
    void slot_on_stretch();
    void slot_on_retract();
public slots:
    void slot_on_updateStatusTable(QString qstr);
    void slot_on_updateCAN304(QVector<int> vec);
    void slot_on_updateCAN305(QVector<int> vec);
    void slot_on_surfaceUpdate(CYJData cyj);
private:
    Ui::MainWindow *ui;
    CYJData m_cyjData_surface;
    CYJData m_cyjData_actual;
	SICK511 m_sick511_f;//Forward SICK511
	SICK511 m_sick511_b;//Backward SICK511
    SICK400 m_sick400;
    CAN m_can;
    SurfaceCommunication	m_surfaceComm;
    autoAlgorithm			m_algorithm;
    QTimer					m_timer_Teach;
    QTimer					m_timer_CAN;
    QThread					m_thread_CAN;
    QTimer					m_timer_main;
    QTimer					m_timer_surface;
    QTimer                  m_timer_mileAccumulator;
    /*************************************************************************
     * vehicle states to surface
	 *
	/*************************************************************************/
    Direction m_direction;
	ControlMode m_controlMode;
    int m_mileInstant;//in cm
    int m_mileMeter;
    //test
    int m_hydraulic1;
    int m_hydraulic2;
    static const int LEFTLIMIT = 4;
    static const int RIGHTLIMNIT = 83;
};

#endif // MAINWINDOW_H
