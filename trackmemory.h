#ifndef TRACKMEMORY_H
#define TRACKMEMORY_H
#include <QVector>

class TrackMemoryImpl;

class TrackMemory
{
public:
    TrackMemory();
    ~TrackMemory();
    void init(const int accmin,const int accmax,const int anglemin,const int anglemax);
    /*
     * update control according to mile
     * @input param mile:original mile from mile meter
     * @input param dist:SICK distance data
     * @input param rssi:SICK RSSI     data
     * */
    QVector<int>  update(int mile, QVector<int> beaconLength);
    /*
     * data are saved into beacon.txt in the TEACH stage
     * @intput param length: beacon length
     * @intput param mile: current mile
     * */
    bool saveBeacon(const int length,const int mile);
    /*
     * data are saved into path.txt in the TEACH stage
     * @intput param mile: current mile
     * @intput param acc: command acc
     * @intput param deacc: command deacc
     * @intput param left: command left
     * @intput param right: command right
     * */
    bool savePath(const int mile,const int acc,const int deacc,const int left,const int right);
    /*
     * permanently save data into beacon.dat and path.dat
     * */
    bool saveAll();
    /*
	* load data 
	* @intput param fileName:which file data to be loaded
	* */
	bool loadData(const QString fileName);
    /*
    * load data
    * @intput param fileName:which file data to be loaded
    * */
    bool isDataLoaded(const QString fileName);
    /*
    * match with loaded beacon length
    * @intput param vec:SICK400 outputs beacon length
    * */
    bool matchBeacon(const QVector<int> vec);
	/*
	* current beacon width
	* @input
	* @output:current beacon width
	* */
	int	currentBeacon();
private:
    TrackMemoryImpl *m_Impl;
};

#endif // TRACKMEMORY_H
