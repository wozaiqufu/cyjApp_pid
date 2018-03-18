#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QTime>
#include <fstream>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
m_controlMode(Local),
m_mileInstant(0),
m_mileMeter(0),
m_speed(0),
  _sensorLost(false),
  _acc(0),
  _deacc(0),
  _forward(true),
  _backward(false),
  _stop(false),
  _neutral(false),
  _maintimerCount(0),
  _isBeaconFound(false),
  _hydraulic(0)
{
    ui->setupUi(this);
    initCYJActualData();
    initCYJSurfaceData();
    //init CAN
    if( slot_on_initCAN())
        slot_on_readFrame();
    //init SICK511
    slot_on_initSICK511();
    //init SICK400
    slot_on_initSICK400();
    //init DataSaver
//    initDataSaver();
    //loadData();
    //init surface
    slot_on_initSurface();
    //connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(slot_on_initSICK511()));
	connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(slot_on_stopSICK511()));
//    connect(ui->pushButton_5,SIGNAL(clicked()),this,SLOT(slot_on_initCAN()));
//    connect(ui->pushButton_6,SIGNAL(clicked()),this,SLOT(slot_on_readFrame()));
    connect(ui->pushButton_initSurface,SIGNAL(clicked()),this,SLOT(slot_on_initSurface()));
    connect(ui->pushButton_connect400,SIGNAL(clicked()),this,SLOT(slot_on_initSICK400()));
    connect(ui->pushButton_stop400,SIGNAL(clicked()),this,SLOT(slot_on_stopSICK400()));
    connect(ui->pushButton_accuMile,SIGNAL(clicked()),this,SLOT(slot_on_startAccumMile()));
    connect(&m_timer_main,SIGNAL(timeout()),this,SLOT(slot_on_mainTimer_timeout()));
    connect(&m_timer_mileAccumulator,SIGNAL(timeout()),this,SLOT(slot_on_mileAccumulator_timeout()));
    connect(ui->pushButton_acc50,SIGNAL(clicked()),this,SLOT(slot_on_acc50()));
    connect(ui->pushButton_acc85,SIGNAL(clicked()),this,SLOT(slot_on_acc85()));
    connect(ui->pushButton_acc90,SIGNAL(clicked()),this,SLOT(slot_on_acc90()));
    connect(ui->pushButton_acc95,SIGNAL(clicked()),this,SLOT(slot_on_acc95()));
    connect(ui->pushButton_acc100,SIGNAL(clicked()),this,SLOT(slot_on_acc100()));
    m_timer_mileAccumulator.start(200);
    m_timer_main.start(5);
    //signals:SICK400,slots:algorithm
    connect(&m_sick400,SIGNAL(sigUpdateBeaconLength(QVector<int>)),&m_algorithm,SLOT(slot_on_updateBeaconLength(QVector<int>)));
    //signals:SICK400,slots:mainwindow
    connect(&m_sick400,SIGNAL(sigUpdateBeaconLength(QVector<int>)),this,SLOT(slot_on_updateBeaconLength(QVector<int>)));
    //signals:SICK511,slots:algorithm
    connect(&m_sick511_f, SIGNAL(sigUpdateCourseAngle(int)), &m_algorithm, SLOT(slot_on_updateCourseAngle(int)));
    connect(&m_sick511_f, SIGNAL(sigUpdateLateralOffset(int,QString)), &m_algorithm, SLOT(slot_on_updateLateralOffset(int,QString)));
    connect(&m_sick511_b, SIGNAL(sigUpdateCourseAngle(int)), &m_algorithm, SLOT(slot_on_updateCourseAngle(int)));
    connect(&m_sick511_b, SIGNAL(sigUpdateLateralOffset(int,QString)), &m_algorithm, SLOT(slot_on_updateLateralOffset(int,QString)));
    //signals:MainWindow,slots:Algorithm

    connect(this, SIGNAL(sig_informAlgrithmMile(int)), &m_algorithm, SLOT(slot_on_updateMile(int)));
    connect(this,SIGNAL(sig_spliceAngle2Algorithm(int)),&m_algorithm,SLOT(slot_on_updateSpliceAngle(int)));
    connect(this,SIGNAL(sig_angleCmm2Algorithm(int)),&m_algorithm,SLOT(slot_on_updateAngleCommand(int)));
    connect(this,SIGNAL(sig_speed2Algorithm(int)),&m_algorithm,SLOT(slot_on_updateSpeed(int)));
    connect(this,SIGNAL(sig_angleOrg(int)),&m_algorithm,SLOT(slot_on_updateTrackAngle(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

//control mode from surface is:
//remote manual---2
//local manual---0
//local auto---1
//remote auto---3
void MainWindow::updateControlMode()
{
    if(m_cyjData_actual.localRemote==0&&m_cyjData_actual.manualVisual==0)
    {
        m_controlMode = Local;
        //qDebug()<<"m_controlMode:Local";
    }
    else if(m_cyjData_actual.localRemote==0&&m_cyjData_actual.manualVisual==1)
    {
        m_controlMode = Visible;
        //qDebug()<<"m_controlMode:Visible";
    }
    else if(m_cyjData_actual.localRemote==1&&m_cyjData_actual.manualVisual==1)
    {
        if(m_cyjData_surface.localRemote==1&&m_cyjData_surface.manualVisual==0)
        {
            m_controlMode = Remote;
            //qDebug()<<"m_controlMode:Remote";
        }
        else if(m_cyjData_surface.localRemote==1&&m_cyjData_surface.manualVisual==1)
        {
            m_controlMode = Auto;
            //qDebug()<<"m_controlMode:Auto";
        }
    }
}

void MainWindow::initCYJActualData()
{
    m_cyjData_actual.startdata1 = 0xAA;
    m_cyjData_actual.startdata2 = 0x55;
    m_cyjData_actual.forward = 0;
    m_cyjData_actual.backward = 0;
    m_cyjData_actual.neutral = 1;
    m_cyjData_actual.stop = 1;
    m_cyjData_actual.scram = 0;
    m_cyjData_actual.light = 0;
    m_cyjData_actual.horn = 0;
    m_cyjData_actual.zero = 0;

    m_cyjData_actual.manualVisual = 0;
    m_cyjData_actual.localRemote = 0;
    m_cyjData_actual.start = 0;
    m_cyjData_actual.flameout = 0;
    m_cyjData_actual.middle = 1;
    m_cyjData_actual.warn1 = 0;
    m_cyjData_actual.warn2 = 0;
    m_cyjData_actual.warn3 = 0;

    m_cyjData_actual.rise = 0;
    m_cyjData_actual.fall = 0;
    m_cyjData_actual.turn = 0;
    m_cyjData_actual.back = 0;
    m_cyjData_actual.left = 0;
    m_cyjData_actual.right = 0;
    m_cyjData_actual.acc = 0;
    m_cyjData_actual.deacc = 0;
    m_cyjData_actual.speed = 0;
    m_cyjData_actual.engine = 0;
    m_cyjData_actual.spliceAngle = 0;
    m_cyjData_actual.oil = 0;
    m_cyjData_actual.temperature = 0;
    m_cyjData_actual.enddata = 0xFF;
}

void MainWindow::initCYJSurfaceData()
{
    m_cyjData_surface.startdata1 = 0xAA;
    m_cyjData_surface.startdata2 = 0x55;
    m_cyjData_surface.forward = 0;
    m_cyjData_surface.backward = 0;
    m_cyjData_surface.neutral = 1;
    m_cyjData_surface.stop = 1;
    m_cyjData_surface.scram = 0;
    m_cyjData_surface.light = 0;
    m_cyjData_surface.horn = 0;
    m_cyjData_surface.zero = 1;
//remember modify to 0 0 when in remote control
    m_cyjData_surface.manualVisual = 1;
    m_cyjData_surface.localRemote = 1;
    m_cyjData_surface.start = 0;
    m_cyjData_surface.flameout = 0;
    m_cyjData_surface.middle = 1;
    m_cyjData_surface.warn1 = 0;
    m_cyjData_surface.warn2 = 0;
    m_cyjData_surface.warn3 = 0;

    m_cyjData_surface.rise = 0;
    m_cyjData_surface.fall = 0;
    m_cyjData_surface.turn = 0;
    m_cyjData_surface.back = 0;
    m_cyjData_surface.left = 0;
    m_cyjData_surface.right = 0;
    m_cyjData_surface.acc = 0;
    m_cyjData_surface.deacc = 0;
    m_cyjData_surface.speed = 0;
    m_cyjData_surface.engine = 0;
    m_cyjData_surface.spliceAngle = 0;
    m_cyjData_surface.oil = 0;
    m_cyjData_surface.temperature = 0;
    m_cyjData_surface.enddata = 0xFF;
}

void MainWindow::updateAcc()
{
    if(_maintimerCount==20)
    {
         //qDebug()<<"_maintimerCount==20";
         qDebug()<<"_acc:"<<_acc;
        if((!_isBeaconFound)&&(m_cyjData_surface.zero))
        {
            if(m_speed>1390)
            {
                _acc = 85;
                return;
            }
           if(_acc<50)
           {
               _acc += 10;
               return;
               //qDebug()<<"acc is"<<_acc;
           }
        }
        else if(_isBeaconFound||!m_cyjData_surface.zero)
        {
            if(_acc>=10)
            {
                _acc = _acc - 10;
            }
        }
    }
}

void MainWindow::updateDirection()
{
    if(_maintimerCount==20)
    {
        qDebug()<<"deacc:"<<_deacc;
        qDebug()<<"_stop:"<<_stop;
        qDebug()<<"_forward:"<<_forward;
        qDebug()<<"_backward:"<<_backward;
        qDebug()<<"_neutral:"<<_neutral;
        if(m_cyjData_surface.zero==0||_isBeaconFound)
        {
            if(_acc==0)
            {
                _deacc = 127;
                if(m_speed<40)
                {
                    _stop = true;
                    _forward = false;
                    _backward = false;
                    _neutral = true;
                }
            }
        }
        else
        {
            if(_acc>0)
            {
                _deacc = 0;
                _stop = m_cyjData_surface.stop;
                _forward = m_cyjData_surface.forward;
                _backward = m_cyjData_surface.backward;
                _neutral = m_cyjData_surface.neutral;
                qDebug()<<"_forward:"<<_forward;
                qDebug()<<"_backward:"<<_backward;
            }
        }
    }
}

void MainWindow::initDataSaver()
{
    m_dataSaver.moveToThread(&m_thread_dataSaver);
    m_timer_DataSaver.setInterval(100);
    m_timer_DataSaver.moveToThread(&m_thread_dataSaver);
    connect(&m_thread_dataSaver,SIGNAL(started()),&m_timer_DataSaver,SLOT(start()));
    connect(&m_timer_DataSaver,SIGNAL(timeout()),&m_dataSaver,SLOT(slot_doWork()));


    connect(this,SIGNAL(sig_mile(int)),&m_dataSaver,SLOT(slot_update_dis(int)));
    connect(this,SIGNAL(sig_velocity(int)),&m_dataSaver,SLOT(slot_update_velocity(int)));
    connect(this,SIGNAL(sig_angle(int)),&m_dataSaver,SLOT(slot_update_angle(int)));
    connect(this,SIGNAL(sig_acc(int)),&m_dataSaver,SLOT(slot_update_acc(int)));
    connect(this,SIGNAL(sig_angleOrg(int)),&m_dataSaver,SLOT(slot_on_update_angleOrg(int)));
    connect(this,SIGNAL(sig_hydraulic(int)),&m_dataSaver,SLOT(slot_on_update_hydraulic(int)));
    connect(&m_sick511_b,SIGNAL(sigUpdateCourseAngle(int)),&m_dataSaver,SLOT(slot_update_courseAngle(int)));
    connect(&m_sick511_f,SIGNAL(sigUpdateCourseAngle(int)),&m_dataSaver,SLOT(slot_update_courseAngle(int)));
    connect(&m_sick511_b,SIGNAL(sigUpdateLateralOffset(int)),&m_dataSaver,SLOT(slot_update_lateralOffset(int)));
    connect(&m_sick511_f,SIGNAL(sigUpdateLateralOffset(int)),&m_dataSaver,SLOT(slot_update_lateralOffset(int)));
    connect(this, SIGNAL(sig_addTickCount()), &m_sick511_f, SLOT(slot_on_addTickCount()));
    connect(this, SIGNAL(sig_addTickCount()), &m_sick511_b, SLOT(slot_on_addTickCount()));
    connect(&m_dataSaver,SIGNAL(finished()),&m_dataSaver,SLOT(deleteLater()));
    m_thread_dataSaver.start();
}

void MainWindow::updateData2DataSaver()
{
    emit sig_mile(m_mileMeter);
    emit sig_velocity(m_speed);
    emit sig_angle(m_cyjData_actual.spliceAngle);
    emit sig_acc(m_cyjData_actual.acc);
    emit sig_hydraulic(_hydraulic);
    if(m_map.size()>0)
    {
        QMap<int, int>::const_iterator i = m_map.lowerBound(m_mileMeter);
        int angleSet = i.value();
        emit sig_angleOrg(angleSet);
    }
}

void MainWindow::updateData2Surface()
{
    m_cyjData_actual.startdata1 = 0xAA;
    m_cyjData_actual.startdata2 = 0x55;
    m_cyjData_actual.enddata = 0xFF;
    emit sig_informInfo2surface(m_cyjData_actual);
}

void MainWindow::updateData2Algorithm()
{
    if(m_map.size()>0)
    {
        QMap<int, int>::const_iterator i = m_map.lowerBound(m_mileMeter);
        int angleSet = i.value();
        emit sig_angleOrg(angleSet);
    }
}

void MainWindow::loadData()
{
    QFile file("path.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
             return;
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QList <int> dataRaw;
        QString_to_Int(line,dataRaw);
        //qDebug()<<"dataRow is:"<<dataRow;
        if(dataRaw.size()==8)
        {
            m_map.insert(dataRaw.at(0),dataRaw.at(2));
        }
    }
    qDebug()<<"map is:"<<m_map;
}

void MainWindow::QString_to_Int(QString str, QList<int> &data)
{
    QStringList list;
       list=str.split(QRegExp(","));
       //qDebug()<<"list"<<list;
       foreach (QString i,list)
       {
           data.append(i.toInt(0,10));
       }
}

bool MainWindow::checkSensorsAndSurface()
{
    return((m_sick400.doWorkCount()>100)&&
           (m_sick511_b.doWorkCount()>100)&&
           (m_sick511_f.doWorkCount()>100)&&
           (m_surfaceComm.doWorkCount()>100));
}

void MainWindow::slot_on_initSICK511()
{
//	connect(&m_sick511_f, SIGNAL(sigUpdateCourseAngle(int)), this, SLOT(slot_on_updateForwardCourseAngle(int)));
//	connect(&m_sick511_f, SIGNAL(sigUpdateLateralOffset(int)), this, SLOT(slot_on_updateForwardLateralOffset(int)));
//	connect(&m_sick511_b, SIGNAL(sigUpdateCourseAngle(int)), this, SLOT(slot_on_updateBackwardCourseAngle(int)));
//	connect(&m_sick511_b, SIGNAL(sigUpdateLateralOffset(int)), this, SLOT(slot_on_updateBackwardLateralOffset(int)));
	if (m_sick511_f.init("forward", "192.168.1.50", 2111))
	{
		m_sick511_f.continuousStart();
	}
   m_sick511_f.useData(true);
	if (m_sick511_b.init("backward", "192.168.1.51", 2111))
	{
		m_sick511_b.continuousStart();
	}
    m_sick511_b.useData(false);
}

void MainWindow::slot_on_stopSICK511()
{
    emit sig_stopPermanentReq();
}

bool MainWindow::slot_on_initCAN()
{
    m_can.moveToThread(&m_thread_CAN);
    m_timer_CAN.setInterval(1);
    m_timer_CAN.moveToThread(&m_thread_CAN);
    connect(&m_thread_CAN,SIGNAL(started()),&m_timer_CAN,SLOT(start()));
    connect(&m_can,SIGNAL(sigUpdateCAN304(QVector<int>)),this,SLOT(slot_on_updateCAN304(QVector<int>)));
    connect(&m_can,SIGNAL(sigUpdateCAN305(QVector<int>)),this,SLOT(slot_on_updateCAN305(QVector<int>)));
    connect(&m_can,SIGNAL(sig_statusTable(QString)),this,SLOT(slot_on_updateStatusTable(QString)));
    connect(&m_timer_CAN,SIGNAL(timeout()),&m_can,SLOT(slot_dowork()));
    connect(&m_thread_CAN,SIGNAL(finished()),&m_thread_CAN,SLOT(deleteLater()));
    return m_can.init(0);//0 for CAN0
}

void MainWindow::slot_on_initSICK400()
{
    connect(&m_sick400, SIGNAL(sig_statusTable(QString)), this, SLOT(slot_on_updateStatusTable(QString)));
    connect(this, SIGNAL(sig_addTickCount()), &m_sick400, SLOT(slot_on_addTickCount()));
    if(m_sick400.init("SICK400","192.168.1.52",2111))
    {
        m_sick400.continuousStart();
    }
}

void MainWindow::slot_on_stopSICK400()
{
    m_sick400.continuousStop();
}

void MainWindow::slot_on_readFrame()
{
    m_thread_CAN.start();
    emit sig_statusTable("read CAN Start!");
    //_can8900.read_message();
}

void MainWindow::slot_on_initSurface()
{
    m_surfaceComm.init("192.168.1.3",6001);
    connect(&m_timer_surface,SIGNAL(timeout()),&m_surfaceComm,SLOT(slot_doWork()));
    connect(this,SIGNAL(sig_informInfo2surface(CYJData)),&m_surfaceComm,SLOT(slot_on_mainwindowUpdate(CYJData)));
    connect(&m_surfaceComm,SIGNAL(sig_informMainwindow(CYJData)),this,SLOT(slot_on_surfaceUpdate(CYJData)));
    connect(this, SIGNAL(sig_addTickCount()), &m_surfaceComm, SLOT(slot_on_addTickCount()));
    m_timer_surface.start(10);
}

 void MainWindow::slot_on_mileAccumulator_timeout()
 {
     m_mileMeter += m_mileInstant;
 }
/****************************************************************************/
/****************************************************************************/
/*********************update data and send to CR0032*************************/
/*
TO:SICK obj~~
INFORM:direction
TO:CAN obj~~
INFORM:
*************************/
void MainWindow::slot_on_mainTimer_timeout()
{
    updateControlMode();
    updateData2DataSaver();
    updateData2Surface();
    //updateData2Algorithm();
    if(m_controlMode==Auto)
    {
        if(_maintimerCount<20)
        {
            _maintimerCount++;
        }
        else
            _maintimerCount = 0;
        qDebug()<<"m_cyjData_actual.forward:"<<m_cyjData_actual.forward;
        qDebug()<<"m_cyjData_actual.backward:"<<m_cyjData_actual.backward;
        if(m_cyjData_actual.forward&&!m_cyjData_actual.backward){
            m_sick511_f.useData(true);
            m_sick511_b.useData(false);
        }
        if(m_cyjData_actual.backward&&!m_cyjData_actual.forward){
            m_sick511_f.useData(false);
            m_sick511_b.useData(true);
        }
        emit sig_addTickCount();
        emit sig_informAlgrithmMile(m_mileMeter);
        emit sig_spliceAngle2Algorithm(m_cyjData_actual.spliceAngle-(LEFTLIMIT+RIGHTLIMNIT)/2);
    }
    qDebug()<<"control mode is:"<<m_controlMode;
    switch (m_controlMode)
    {
    case Remote:
        {
        uchar data[8] = {0,0,0,0,0,0,0,0};
        data[0] = m_cyjData_surface.backward +
                2*m_cyjData_surface.forward +
                4*m_cyjData_surface.neutral +
                8*m_cyjData_surface.stop +
                16*m_cyjData_surface.scram +
                32*m_cyjData_surface.light +
                64*m_cyjData_surface.horn +
                128*m_cyjData_surface.zero;
//            qDebug()<<"===========================================>";
//            qDebug()<<"data from surface forward:"<<m_cyjData_surface.forward;
//            qDebug()<<"data from surface backward:"<<m_cyjData_surface.backward;
//            qDebug()<<"data from surface neutral:"<<m_cyjData_surface.neutral;
//            qDebug()<<"data from surface stop:"<<m_cyjData_surface.stop;
//            qDebug()<<"data from surface scram:"<<m_cyjData_surface.scram;
//            qDebug()<<"data from surface light:"<<m_cyjData_surface.light;
//            qDebug()<<"data from surface horn:"<<m_cyjData_surface.horn;
//            qDebug()<<"data from surface zero:"<<m_cyjData_surface.zero;
//            qDebug()<<"data from surface start:"<<m_cyjData_surface.start;
//            qDebug()<<"data from surface flameout:"<<m_cyjData_surface.flameout;
//            qDebug()<<"data from surface middle:"<<m_cyjData_surface.middle;
//            qDebug()<<"data from surface localRemote:"<<m_cyjData_surface.localRemote;
//            qDebug()<<"data from surface manualVisual:"<<m_cyjData_surface.manualVisual;
//            qDebug()<<"data from surface rise:"<<m_cyjData_surface.rise;
//            qDebug()<<"data from surface fall:"<<m_cyjData_surface.fall;
//            qDebug()<<"data from surface turn:"<<m_cyjData_surface.turn;
//            qDebug()<<"data from surface back:"<<m_cyjData_surface.back;
//            qDebug()<<"data from surface left:"<<m_cyjData_surface.left;
//            qDebug()<<"data from surface right:"<<m_cyjData_surface.right;
//            qDebug()<<"data from surface acc:"<<m_cyjData_surface.acc;
//            qDebug()<<"data from surface deacc:"<<m_cyjData_surface.deacc;
        data[1] = 4*m_cyjData_surface.start +
                8*m_cyjData_surface.flameout +
                16*m_cyjData_surface.middle +
                32*m_cyjData_surface.warn1 +
                64*m_cyjData_surface.warn2 +
                128*m_cyjData_surface.warn3;

        data[2] = m_cyjData_surface.rise*1.27;
        data[3] = m_cyjData_surface.fall*1.27;
        data[4] = m_cyjData_surface.turn*1.27;
        data[5] = m_cyjData_surface.back*1.27;
        data[6] = m_cyjData_surface.left*1.27;
        data[7] = m_cyjData_surface.right*1.27;
        m_can.slot_on_sendFrame(0x161,8,data);
        data[0] = m_cyjData_surface.acc*1.27;
        data[1] = m_cyjData_surface.deacc*1.27;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        m_can.slot_on_sendFrame(0x261,8,data);
        break;
        }
    case Auto:
        {
        m_algorithm.update();
    //hydraulic control
//        int left = 0;
//        int right = 0;
//        QMap<int, int>::const_iterator lastItr = m_map.end();
//        if(m_mileMeter<lastItr.key())
//        {
//            QMap<int, int>::const_iterator i = m_map.lowerBound(m_mileMeter);
//            int hyd = i.value();
//            if(hyd<0)
//            {
//                left = 0 - hyd;
//            }
//            else
//            {
//                right = hyd;
//            }
//        }
//        else
//        {
//            left = 0;
//            right = 0;
//        }

        updateAcc();
        updateDirection();
        uchar data[8] = {0,0,0,0,0,0,0,0};
        data[0] = _backward +//back
                2*_forward +//forward
                4*_neutral+//neutral
                8*_stop +//stop
                16*m_cyjData_actual.scram +
                32*m_cyjData_actual.light +
                64*m_cyjData_actual.horn +
                128*m_cyjData_actual.zero;
        data[1] = 4*m_cyjData_actual.start +
                8*m_cyjData_actual.flameout +
                16*m_cyjData_actual.middle +
                32*m_cyjData_actual.warn1 +
                64*m_cyjData_actual.warn2 +
                128*m_cyjData_actual.warn3;
        data[2] = m_cyjData_actual.rise;
        data[3] = m_cyjData_actual.fall;
        data[4] = m_cyjData_actual.turn;
        data[5] = m_cyjData_actual.back;
        data[6] = m_algorithm.left();
        data[7] = m_algorithm.right();
        qDebug()<<"m_algorithm.left():"<<data[6];
        qDebug()<<"m_algorithm.right():"<<data[7];
        m_can.slot_on_sendFrame(0x161,8,data);
        data[0] = _acc;
        data[1] = _deacc;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
        data[7] = 0;
        //qDebug()<<"m_algorithm.accelerator():"<<data[0];
        //qDebug()<<"m_algorithm.deaccelerator():"<< data[1];
        m_can.slot_on_sendFrame(0x261,8,data);

        break;
        }
    default:
        break;
    }
}

void MainWindow::slot_on_startAccumMile()
{
    m_timer_mileAccumulator.start(200);
    //qDebug()<<"slot_on_startAccumMile";
}

void MainWindow::slot_on_stopAccumMile()
{
    m_mileMeter = 0;
    m_timer_mileAccumulator.stop();
   // qDebug()<<"slot_on_stopAccumMile";
}

void MainWindow::slot_on_acc50()
{
    _acc = 50;
}

void MainWindow::slot_on_acc85()
{
    _acc = 85;
}

void MainWindow::slot_on_acc90()
{
    _acc = 90;
}

void MainWindow::slot_on_acc95()
{
    _acc = 95;
}

void MainWindow::slot_on_acc100()
{
    _acc = 100;
}

void MainWindow::slot_on_updateCAN304(QVector<int> vec)
{
    //qDebug()<<"CAN304:"<<vec;
    if(vec.size()<8)
    {
        return;
    }
    //extract Data[0]
    if((vec.at(0))%2==1)
    {
        m_cyjData_actual.backward = 1;
    }
    else
    {
        m_cyjData_actual.backward = 0;
    }
    if((vec.at(0)/2)%2==1)
    {
        m_cyjData_actual.forward = 1;
    }
    else
    {
        m_cyjData_actual.forward = 0;
    }
    if((vec.at(0)/4)%2==1)
    {
        m_cyjData_actual.neutral = 1;
    }
    else
    {
        m_cyjData_actual.neutral = 0;
    }
    if((vec.at(0)/8)%2==1)
    {
        m_cyjData_actual.stop = 1;
    }
    else
    {
        m_cyjData_actual.stop = 0;
    }

    if((vec.at(0)/16)%2==1)
    {
        m_cyjData_actual.scram = 1;
    }
    else
    {
        m_cyjData_actual.scram = 0;
    }

    if((vec.at(0)/32)%2==1)
    {
        m_cyjData_actual.light = 1;
    }
    else
    {
        m_cyjData_actual.light = 0;
    }

    if((vec.at(0)/64)%2==1)
    {
        m_cyjData_actual.horn = 1;
    }
    else
    {
        m_cyjData_actual.horn = 0;
    }
    //reserved
    m_cyjData_actual.zero = (vec.at(0)/64)%2;
    //extract control mode
    switch(vec.at(1)%4)
    {
    case 0:
        m_cyjData_actual.localRemote = 0;
        m_cyjData_actual.manualVisual = 0;
        break;
    case 1:
        m_cyjData_actual.localRemote = 1;
        m_cyjData_actual.manualVisual = 1;
        break;
    case 2:
        m_cyjData_actual.localRemote = 1;
        m_cyjData_actual.manualVisual = 1;
        break;
    case 3:
        m_cyjData_actual.localRemote = 0;
        m_cyjData_actual.manualVisual = 1;
        break;
    default:
        break;
    }
//    qDebug()<<"m_cyjData_actual.localRemote"<<m_cyjData_actual.localRemote;
//    qDebug()<<"m_cyjData_actual.manualVisual"<<m_cyjData_actual.manualVisual;
    //extract engine start
    m_cyjData_actual.start = (vec.at(1)/4)%2;
    //extract engine stop
    m_cyjData_actual.flameout = (vec.at(1)/8)%2;
    //extract engine switch medium
    m_cyjData_actual.middle = (vec.at(1)/16)%2;
    //extract warn1
    m_cyjData_actual.warn1 = (vec.at(1)/32)%2;
    //extract warn2
    m_cyjData_actual.warn2 = (vec.at(1)/64)%2;
    //extract warn3
    m_cyjData_actual.warn3 = (vec.at(1)/128)%2;

    m_cyjData_actual.rise = vec.at(2);
    m_cyjData_actual.fall = vec.at(3);
    m_cyjData_actual.turn = vec.at(4);
    m_cyjData_actual.back = vec.at(5);
    m_cyjData_actual.left = vec.at(6);
    m_cyjData_actual.right = vec.at(7);
    _hydraulic = ( m_cyjData_actual.right>m_cyjData_actual.left)?
                m_cyjData_actual.right:0-m_cyjData_actual.left;

}

void MainWindow::slot_on_updateCAN305(QVector<int> vec)
{
    //qDebug()<<"CAN305:"<<vec;
    if(vec.size()<8)
    {
        return;
    }
    m_cyjData_actual.acc = vec.at(0);
    m_cyjData_actual.deacc = vec.at(1);
    m_cyjData_actual.speed = vec.at(2);
    m_cyjData_actual.engine = vec.at(3);
    m_cyjData_actual.spliceAngle = vec.at(4);
    m_cyjData_actual.temperature = vec.at(5);
    m_mileInstant = 1.08*(vec.at(6)*256 + vec.at(7));
    m_speed = m_mileInstant * 5;
    //qDebug()<<"m_speed"<<m_speed*0.036;
    //emit sig_speed2Algorithm(m_speed);
    //qDebug()<<"m_mileInstant:"<<m_mileInstant;
}

void MainWindow::slot_on_surfaceUpdate(CYJData cyj)
{
    //qDebug()<<"MainWindow::slot_on_surfaceUpdate(CYJData cyj)";
    m_cyjData_surface.forward = cyj.forward;
    m_cyjData_surface.backward = cyj.backward;
    m_cyjData_surface.neutral = cyj.neutral;
    m_cyjData_surface.stop = cyj.stop;
    m_cyjData_surface.scram = cyj.scram;
    m_cyjData_surface.light = cyj.light;
    m_cyjData_surface.horn = cyj.horn;
    m_cyjData_surface.zero = cyj.zero;
    m_cyjData_surface.manualVisual = cyj.manualVisual;
    m_cyjData_surface.localRemote = cyj.localRemote;
    m_cyjData_surface.start = cyj.start;
    m_cyjData_surface.flameout = cyj.flameout;
    m_cyjData_surface.middle = cyj.middle;
    m_cyjData_surface.rise = cyj.rise;
    m_cyjData_surface.fall = cyj.fall;
    m_cyjData_surface.turn = cyj.turn;
    m_cyjData_surface.back = cyj.back;
    m_cyjData_surface.left = cyj.left;
    m_cyjData_surface.right = cyj.right;
    m_cyjData_surface.acc = cyj.acc;
    m_cyjData_surface.deacc = cyj.deacc;
    ui->label_sur_acc->setText(QString::number(m_cyjData_surface.acc));
//    qDebug()<<"===========================================>";
//    qDebug()<<"data from surface forward:"<<m_cyjData_surface.forward;
//    qDebug()<<"data from surface backward:"<<m_cyjData_surface.backward;
//    qDebug()<<"data from surface neutral:"<<m_cyjData_surface.neutral;
//    qDebug()<<"data from surface stop:"<<m_cyjData_surface.stop;
//    qDebug()<<"data from surface scram:"<<m_cyjData_surface.scram;
//    qDebug()<<"data from surface light:"<<m_cyjData_surface.light;
//    qDebug()<<"data from surface horn:"<<m_cyjData_surface.horn;
//    qDebug()<<"data from surface zero:"<<m_cyjData_surface.zero;
//    qDebug()<<"data from surface start:"<<m_cyjData_surface.start;
//    qDebug()<<"data from surface flameout:"<<m_cyjData_surface.flameout;
//    qDebug()<<"data from surface middle:"<<m_cyjData_surface.middle;
//    qDebug()<<"data from surface localRemote:"<<m_cyjData_surface.localRemote;
//    qDebug()<<"data from surface manualVisual:"<<m_cyjData_surface.manualVisual;
//    qDebug()<<"data from surface rise:"<<m_cyjData_surface.rise;
//    qDebug()<<"data from surface fall:"<<m_cyjData_surface.fall;
//    qDebug()<<"data from surface turn:"<<m_cyjData_surface.turn;
//    qDebug()<<"data from surface back:"<<m_cyjData_surface.back;
//    qDebug()<<"data from surface left:"<<m_cyjData_surface.left;
//    qDebug()<<"data from surface right:"<<m_cyjData_surface.right;
//    qDebug()<<"data from surface acc:"<<m_cyjData_surface.acc;
//    qDebug()<<"data from surface deacc:"<<m_cyjData_surface.deacc;
}

void MainWindow::slot_on_updateBeaconLength(QVector<int> vec)
{
    if(vec.isEmpty())
        return;
    //qDebug()<<"beacon length vector is:"<<vec;
    for(int i=0;i<vec.size();i++)
    {
        if(abs(vec.at(i)-275)<20)
        {
            _isBeaconFound = true;
            return;
        }
    }
}
//easy to debug:all info shows into the statusBar
void MainWindow::slot_on_updateStatusTable(QString qstr)
{
    //qDebug()<<"slot_on_updateStatusBar:"<<qstr;
    //qDebug()<<"rowcount:"<<ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    QTableWidgetItem *newItem = new QTableWidgetItem(QTime::currentTime().toString());
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,newItem);
    QTableWidgetItem *newItem2 = new QTableWidgetItem(qstr);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,newItem2);
}
