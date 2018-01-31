#ifndef AUTOALGORITHM_H
#define AUTOALGORITHM_H
#include <QVector>
#include <QObject>
#include <QStringList>
#include <QByteArray>
#include <QTime>

class TrackMemory;
class PID;

class autoAlgorithm : public QObject
{
    Q_OBJECT
public:
    enum StageType{Teach,Auto};//Teach for saving beacon.txt
    enum AlgorithmType{PIDType,TrackMemoryType,MixedType};//0,1,2
    explicit autoAlgorithm(QObject *parent = 0);
	/*
	* tells tech mode or auto mode
	* @input param type :0 for teach mode,1 for auto mode
	* */
    void    setStageType(const int type);
	/*
	* tells algorithm type is auto mode
	* @input param type:0 for PID, 1 for trackmemory, 2 for Mixed
	* */
    void    setAlgorithmType(const int type);
	/*
	* RSSI threshold for beacon recognization 
	* @input param threshold:RSSI value higher than it is valid
	* */
    void    setBeaconRSSIThreshold(const int threshold);
	/*
	* update:
	* if in TEACH MODE,save path.txt and beacon.txt
	* if in AUTO MODE, update acc, angle according to algorithm type
	* */
	void	update();
    /*
    * finish auto or teach
    * @return
    * */
    void finish();
    /*
    * returns final left
    * @return turn left command(opposite when backward)
    * */
    int     left() const;
	/*
	* returns final right
	* @return turn right command(opposite when backward)
	* */
    int     right() const;
	/*
	* returns final acc
	* @return acc command
	* */
    int     accelerator() const;
	/*
	* returns final deacc
	* @return deacc command
	* */
    int     deaccelerator() const;
    void    testTrackMemory();
    void    testTrackMemory2();
    void    loadData();
private:
	/*
	* increase to certain value specified with inc,return true if succeed
	* add a noise scale if not increase to (origin+inc) EXACTLY
	* @return true if succeed
	* */
    bool isCertainMileIncrement(const int mile);
signals:
    void sig_statusTable(QString);
public slots:
    void slot_on_updateControlMode(bool isAuto);
    void slot_on_updateBeaconLength(QVector<int> vec);
    void slot_on_updateCourseAngle(int angle);
    void slot_on_updateLateralOffset(int of);
    void slot_on_updateMile(int mile);
	void slot_on_updateControlInfo(QVector<int> vec);
    void slot_on_updateSpliceAngle(int angle);
    //for test
    void slot_on_updateAngleCommand(int angle);
private:
    int                 m_courseAngle;
    int                 m_lateralOffset;
    QVector<int>        m_beaconLength;
    bool                m_isAuto;
    QVector<int>        m_mile_acc_deacc_left_right;//signal from mainwindow
    int                 m_mile_saved;//saved data into path.txt
    int                 m_mile_current;//current mile
    StageType           m_stage;//teach mode or auto mode
    AlgorithmType       m_type;//PID or TrackMemory
    QTime                                                 m_time;
    TrackMemory         *p_track;
    PID                 *p_disntancePID;
    PID                 *p_anglePID;
    static const int    MILEDELTA = 20;//path.txt mile increment is 20cm
    static const double RATIO = 0.8;//k1*track + k2*pid k2=RATIO
    int                 m_left;
    int                 m_right;
    int                 m_acc;
    int                 m_spliceAnlge;
    //for test
    int                 m_angleCmm;
};
#endif // AUTOALGORITHM_H
