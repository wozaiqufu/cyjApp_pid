#include "trackmemory.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QMap>
#include <QList>
#include <math.h>

/*
 * TrackMemory implementation Class
 * */

typedef struct Beacon
{
    int length;
    int mile;
}BEACON;

typedef struct controlCommand
{
    int acc;
	int deacc;
    int left;
    int right;
}CONTROL;

class TrackMemoryImpl
{
public:
    TrackMemoryImpl();
    ~TrackMemoryImpl();
    void            init(const int accmin,const int accmax,const int anglemin,const int anglemax);
    QVector<int>	update(int mile, QVector<int> beaconLength);
    bool			saveBeacon(const int length,const int mile);
    bool            savePath(const int mile, const int acc, const int deacc, const int left, const int right);
    bool            saveAll();
    bool			loadData(const QString txtName);
    bool            isDataLoaded(const QString fileName);
    bool            matchBeacon(const QVector<int> &vec,const double threshold);//m_beaconIndex is updated!
	int				currentBeacon();
private:
     QVector<int> beaconLength(QVector<int> dist,QVector<int> rssi,const int delta);
     QVector<int> Pro_binary(QVector<int> vec) const;//for beaconLength use(111001111 filter 00)
     void         calibMile(const int mile);//using m_beaconIndex get mile in beacon.txt
     QVector<int> matchMile(const int mile);//using m_mile_calib generate acc left and right
     bool         closeAllFile();
private:
     QFile                      m_pathFile;
     QFile                      m_beaconFile;
     QFile                      m_beaconRawFile;
     QTextStream                m_pathTextStream;
     QTextStream                m_beaconTextStream;
     QTextStream                m_beaconRawTextStream;
     QList <int>                m_beacon;//beacon length actual
     QList <Beacon>             m_beaconAndMile;//key:left and right beacon width find:mile(calibrated)
     QMap <int,controlCommand>  m_trackMap;//memory data are loaded here:key:disp find:control command
     int                        m_beaconIndex;
     int                        m_mile;
     int                        m_mile_calib;
     int                        m_mile_deltaCalib;
     int                        m_accmax;
     int                        m_accmin;
     int                        m_angmax;
     int                        m_angmin;
     static const int           MAXANGLE = 30;
     static const double        ANGLEDEGREE2RADIUS = 0.0174532925;
     static const int           MAXACC = 100;
     //const int           m_angleMax = 30;
     //const double        m_Angle_degree2Radian = 0.0174532925;
     //const int           m_acceleratorMax = 100;
};

TrackMemory::TrackMemory()
{
    m_Impl = new TrackMemoryImpl();
}

QVector<int> TrackMemory::update(int mile, QVector<int> beaconLength)
{
    return m_Impl->update(mile,beaconLength);
}

bool TrackMemory::saveBeacon(const int length,const int mile)
{
    return m_Impl->saveBeacon(length, mile);
}

bool TrackMemory::savePath(const int mile, const int acc, const int deacc, const int left, const int right)
{
    return m_Impl->savePath(mile,acc,deacc,left,right);
}

bool TrackMemory::saveAll()
{
    return m_Impl->saveAll();
}

bool TrackMemory::loadData(const QString fileName)
{
    return m_Impl->loadData(fileName);
}

bool TrackMemory::isDataLoaded(const QString fileName)
{
    return m_Impl->isDataLoaded(fileName);
}

bool TrackMemory::matchBeacon(const QVector<int> vec)
{
    return m_Impl->matchBeacon(vec,0.1);
}

int	TrackMemory::currentBeacon()
{
	return m_Impl->currentBeacon();
}
TrackMemory::~TrackMemory()
{
    delete m_Impl;
}

void TrackMemory::init(const int accmin,const int accmax,const int anglemin,const int anglemax)
{
    m_Impl->init(accmin,accmax,anglemin,anglemax);
}
/*******************************************************************************************************
 * *****************************************************************************************************
 * */
TrackMemoryImpl::TrackMemoryImpl()
    :m_beaconIndex(0),
      m_mile(0),
      m_mile_calib(0),
      m_mile_deltaCalib(0)
{
	//file names
    m_pathFile.setFileName("path.txt");
    m_beaconFile.setFileName("beacon.txt");
    m_beaconRawFile.setFileName("beaconRaw.txt");
}

TrackMemoryImpl::~TrackMemoryImpl()
{

}

void TrackMemoryImpl::init(const int accmin,const int accmax,const int anglemin,const int anglemax)
{
    m_accmin = accmin;
    m_accmax = accmax;
    m_angmin = anglemin;
    m_angmax = anglemax;
    loadData("beaconRaw.txt");
    loadData("path.txt");
    loadData("beacon.txt");
}

QVector<int> TrackMemoryImpl::update(int mile, QVector<int> beaconLength)
{
    if(matchBeacon(beaconLength,0.1))
    {
        calibMile(mile);
        return matchMile(mile);
    }
    else
        matchMile(mile);
}

bool TrackMemoryImpl::saveBeacon(const int length,const int mile)
{
    Beacon bea;
    bea.length = length;
    bea.mile = mile;
    m_beaconAndMile.append(bea);
//    if(fileName==m_pathFile.fileName())
//    {
//        if(!m_pathFile.isOpen())
//        {
//            m_pathFile.open(QIODevice::Text|QIODevice::WriteOnly);
//            m_pathTextStream.setDevice(&m_pathFile);
//        }
//        int numEachLine = vec.size();
//        if(numEachLine<=0)
//        {
//            qDebug()<<"saveData to " + fileName + ":vector Error!";
//            return false;
//        }
//		m_pathTextStream << numEachLine << "#";
//        for(int i=0;i<vec.size();i++)
//        {
//			if (i == vec.size() - 1)
//				m_pathTextStream << vec.at(i)<<endl;
//			else
//			m_pathTextStream << vec.at(i) << ",";
//        }
//         return true;
//    }
//    else if(fileName==m_beaconFile.fileName())
//    {
//        if(!m_beaconFile.isOpen())
//        {
//            m_beaconFile.open(QIODevice::Text|QIODevice::WriteOnly);
//            m_beaconTextStream.setDevice(&m_beaconFile);
//        }
//        int numEachLine = vec.size();
//        if(numEachLine<=0)
//        {
//            qDebug()<<"saveData to " + fileName + ":vector Error!";
//            return false;
//        }
//		m_beaconTextStream << numEachLine << "#";
//        for(int i=0;i<vec.size();i++)
//        {
//            if(i==vec.size()-1)
//            {
//				m_beaconTextStream << vec.at(i) << endl;
//            }
//            else
//            {
//                 m_beaconTextStream<<vec.at(i)<<",";
//            }
//        }
//         return true;
//    }
//    else
    //        return false;
}

bool TrackMemoryImpl::savePath(const int mile, const int acc, const int deacc, const int left, const int right)
{
    controlCommand co;
    co.acc = acc;
    co.deacc = deacc;
    co.left = left;
    co.right = right;
    m_trackMap.insert(mile,co);
}

bool TrackMemoryImpl::saveAll()
{
    if(m_beaconAndMile.size()==0)
        return false;
    if(m_trackMap.size()==0)
        return false;
    QFile beaconFile("beacon.dat");
    beaconFile.open(QIODevice::WriteOnly);
    QDataStream beaconStream(&beaconFile);
    for(int i=0;i<m_beaconAndMile.size();i++)
    {
        beaconStream <<m_beaconAndMile.at(i).length<<m_beaconAndMile.at(i).mile;
        qDebug()<<"beacon length:"<<m_beaconAndMile.at(i).length;
        qDebug()<<"beacon mile:"<<m_beaconAndMile.at(i).mile;
    }
    QFile pathFile("path.dat");
    pathFile.open(QIODevice::WriteOnly);
    QDataStream pathStream(&pathFile);
    for(int i=0;i<m_trackMap.size();i++)
    {
        pathStream <<m_trackMap.keys().at(i)
                  <<m_trackMap.value(m_trackMap.keys().at(i)).acc
                 <<m_trackMap.value(m_trackMap.keys().at(i)).deacc
                <<m_trackMap.value(m_trackMap.keys().at(i)).left
               <<m_trackMap.value(m_trackMap.keys().at(i)).right;

        qDebug()<<"trackMap acc:"<<m_trackMap.value(m_trackMap.keys().at(i)).acc;
        qDebug()<<"trackMap deacc:"<<m_trackMap.value(m_trackMap.keys().at(i)).deacc;
        qDebug()<<"trackMap left:"<<m_trackMap.value(m_trackMap.keys().at(i)).left;
        qDebug()<<"trackMap right:"<<m_trackMap.value(m_trackMap.keys().at(i)).right;
    }
}

bool TrackMemoryImpl::loadData(const QString txtName)
{
    if (txtName == "beaconRaw.dat")
	{
        if(m_beacon.size()>0)
            m_beacon.clear();
		//load beacon data:beaconRaw.txt
        QFile beaconRawFile("beaconRaw.dat");
        if (beaconRawFile.open(QIODevice::ReadOnly))
		{
            QDataStream readStream(&beaconRawFile);
            while (!readStream.atEnd())
            {
                int value;
                readStream >> value;
                m_beacon.append(value);
            }
            qDebug()<<"m_beacon:"<<m_beacon;
		}
	}
    else if (txtName == "beacon.dat")
	{
        if(m_beaconAndMile.size()>0)
            m_beaconAndMile.clear();
        //load beacon and mile data:beacon.dat
        QFile readFile("beacon.dat");
        if (readFile.open(QIODevice::ReadOnly))
		{
            QDataStream readStream(&readFile);
            QList<int> dataRaw;
            while (!readStream.atEnd())
			{
                 int value;
                 readStream>>value;
                 dataRaw.append(value);
			}
            for(int i=0;i<dataRaw.size()/2;i++)
            {
                Beacon beacon;
                beacon.length = dataRaw.at(2*i);
                beacon.mile = dataRaw.at(2*i+1);
                m_beaconAndMile.append(beacon);
            }
			for (int i = 0; i<m_beaconAndMile.size(); i++)
			{
                qDebug() << "m_beaconAndMile length:" << m_beaconAndMile.at(i).length;
				qDebug() << "m_beaconAndMile mile:" << m_beaconAndMile.at(i).mile;
			}
		}
	}
    else if (txtName == "path.dat")
	{
        if(m_trackMap.size()>0)
             m_trackMap.clear();
        //load mile and control command data:path.dat
        QFile pathFile(txtName);
        if (pathFile.open(QIODevice::ReadOnly))
		{
            QDataStream pathStream(&pathFile);
            QList<int> dataRaw;
            while (!pathStream.atEnd())
			{
                int value;
                pathStream>>value;
                dataRaw.append(value);
			}
            for(int i=0;i<dataRaw.size()/5;i++)
            {
                controlCommand contr;
                int disp = dataRaw.at(5*i);
                contr.acc = dataRaw.at(5*i+1);
                contr.deacc = dataRaw.at(5*i+2);
                contr.left = dataRaw.at(5*i+3);
                contr.right = dataRaw.at(5*i+4);
                m_trackMap.insert(disp,contr);
            }
            for(int i=0;i<m_trackMap.keys().size();i++)
            {
                 qDebug() << "m_trackMap control acc:" << m_trackMap.value(m_trackMap.keys().at(i)).acc;
                 qDebug() << "m_trackMap control deacc:" << m_trackMap.value(m_trackMap.keys().at(i)).deacc;
                 qDebug() << "m_trackMap control left:" << m_trackMap.value(m_trackMap.keys().at(i)).left;
                 qDebug() << "m_trackMap control right:" << m_trackMap.value(m_trackMap.keys().at(i)).right;
            }
		}
	}
    else return false;
}

bool TrackMemoryImpl::isDataLoaded(const QString fileName)
{
    if(fileName=="beaconRaw.dat")
        return m_beacon.size()>0;
    if(fileName=="beacon.dat")
        return m_beaconAndMile.size()>0;
    if(fileName=="path.dat")
        return m_trackMap.size()>0;
    return false;
}

QVector<int> TrackMemoryImpl::beaconLength(QVector<int> dist,QVector<int> rssi,const int delta)
{
    QVector<int> binary_vec;
    QVector<int> beaconLength;
    for(int ix = 0;ix < rssi.size(); ++ix)
    {
        if(rssi[ix] > delta)
            binary_vec.push_back(1);
        else
            binary_vec.push_back(0);
    }
    //qDebug()<<"before Pro binary_vec is :"<<binary_vec;
    binary_vec = Pro_binary(binary_vec);
    //qDebug()<<"after Pro binary_vec is :"<<binary_vec;
    //qDebug()<<"the dist is :"<<m_SICKdata;
    QVector<int> pos01_binary_vec ;//index of "01"
    QVector<int> pos10_binary_vec ;//index of "10"
    if(binary_vec.at(0) == 1)
    {
        pos01_binary_vec.push_back(0);
    }
    for(int ix = 0;ix < binary_vec.size() -1; ++ix)
    {
        int RSSIX = binary_vec.at(ix+1)- binary_vec.at(ix);
        if(RSSIX == 1)
        {
            pos01_binary_vec.push_back(ix + 1) ;
        }
        else if(RSSIX == -1)
        {
            pos10_binary_vec.push_back(ix) ;
        }
    }

    if(binary_vec.last() == 1)
    {
        pos10_binary_vec.push_back(binary_vec.size()-1) ;
    }
    qDebug()<<"the pos of 01 in RSSI is :"<<pos01_binary_vec;
    qDebug()<<"the pos of 10 in RSSI is :"<<pos10_binary_vec;
    //angle = pos10_binary_vec - pos10_binary_vec ;
    beaconLength.clear();
    if(pos01_binary_vec.size() == pos10_binary_vec.size())
    {
        int dist1_beacon = 0;
        int dist2_beacon = 0;
        for(int ix = 0;ix < pos10_binary_vec.size(); ++ix)
        {
            double angle_num = pos10_binary_vec.at(ix) - pos01_binary_vec.at(ix);
            if(angle_num > 2)
            //qDebug()<<"start Length";
            dist1_beacon = dist.at(pos01_binary_vec.at(ix));
            dist2_beacon = dist.at(pos10_binary_vec.at(ix));
            double angle_beacon = (pos10_binary_vec.at(ix) - pos01_binary_vec.at(ix)) * ANGLEDEGREE2RADIUS;
            qDebug()<<"dist1_beacon "<<dist1_beacon;
            qDebug()<<"dist2_beacon "<<dist2_beacon;
            qDebug()<<"angle_beacon "<<angle_beacon;
            //qDebug()<<"cos of angle_beacon "<<cos(angle_beacon);
            int temp3 = sqrt(pow(dist1_beacon,2) + pow(dist2_beacon,2)- 2*dist1_beacon*dist2_beacon*cos(angle_beacon));
            if(temp3 >0)
            {
                double angle_beacon = angle_num * ANGLEDEGREE2RADIUS * 0.5;
                int temp1 = pos01_binary_vec.at(ix);
                int dist1_beacon = dist[temp1];
                int temp2 = pos10_binary_vec.at(ix);
                int dist2_beacon = dist[temp2];
                int temp3 = sqrt(pow(dist1_beacon,2) + pow(dist2_beacon,2)- 2*dist1_beacon*dist2_beacon*cos(angle_beacon));
                qDebug()<<"dist1_beacon "<<dist1_beacon;
                qDebug()<<"dist2_beacon "<<dist2_beacon;
                qDebug()<<"angle_beacon "<<angle_beacon;
                beaconLength.push_back(temp3);
            }
            //qDebug()<<"cos of angle_beacon "<<cos(angle_beacon);
        }
    }
    return beaconLength;
}

QVector<int> TrackMemoryImpl::Pro_binary(QVector<int> vec) const
{
        QVector<int> Pro_vec = vec;
        QVector<int> expansion_vec ;
        QVector<int> corrosion_vec ;
        //expansion
        if(Pro_vec.at(0)+Pro_vec.at(1))
        {
            expansion_vec.push_back(1);
        }
        else
        {
            expansion_vec.push_back(0);
        }
        for(int ix = 1; ix < Pro_vec.size()-1; ++ix)
        {
            if(Pro_vec.at(ix-1) + Pro_vec.at(ix) + Pro_vec.at(ix+1))
            {
                expansion_vec.push_back(1);
            }
            else
            {
                expansion_vec.push_back(0);
            }
        }
        if(Pro_vec.last() + Pro_vec.at(Pro_vec.size()-2))
            {
                expansion_vec.push_back(1);
            }
            else
            {
                expansion_vec.push_back(0);
            }
            //corrosion
            corrosion_vec.push_back(expansion_vec.at(0) * expansion_vec.at(1));
            for(int ix = 1; ix < expansion_vec.size()-1; ++ix)
            {
                corrosion_vec.push_back(expansion_vec.at(ix-1) * expansion_vec.at(ix) * expansion_vec.at(ix+1));
            }
            corrosion_vec.push_back(expansion_vec.last() * expansion_vec.at(expansion_vec.size()-2));

            return corrosion_vec;
}

bool TrackMemoryImpl::matchBeacon(const QVector<int> &vec, const double threshold)
{
    double relative_error = 0.0;
    for(int i = 0; i < m_beacon.size(); ++i)
    {
        for(int j = 0; j < vec.size(); ++j)
        {
            relative_error = static_cast<double>(abs((vec.at(j) - m_beacon.at(i)))/m_beacon.at(i));
            if(relative_error <= threshold)
            {
                m_beaconIndex = i;
                return true;
            }
        }
    }
    return false;
}

int	TrackMemoryImpl::currentBeacon()
{
	if (m_beacon.size() <= 0)
		return -1;
	else
		return m_beacon.at(m_beaconIndex);
}

void TrackMemoryImpl::calibMile(const int mile)
{
    m_mile_deltaCalib = m_beaconAndMile.at(m_beaconIndex).mile - mile;
}

QVector<int> TrackMemoryImpl::matchMile(const int mile)
{
    QVector<int> vec;
    int key = 0;
    int calibMile = mile + m_mile_deltaCalib;
    key = m_trackMap.lowerBound(calibMile).key();
    if(m_trackMap.contains(key))
    {
        if(m_trackMap[key].acc>m_accmax)
            vec.push_back(m_accmax);
        else if(m_trackMap[key].acc<m_accmin)
            vec.push_back(m_accmin);
            else vec.push_back(m_trackMap[key].acc);

        if(m_trackMap[key].left>m_angmax)
            vec.push_back(m_angmax);
        else if(m_trackMap[key].left<m_angmin)
            vec.push_back(m_angmin);
            else vec.push_back(m_trackMap[key].left);

        if(m_trackMap[key].right>m_angmax)
            vec.push_back(m_angmax);
        else if(m_trackMap[key].right<m_angmin)
            vec.push_back(m_angmin);
            else vec.push_back(m_trackMap[key].right);
    }
    return vec;
}

bool TrackMemoryImpl::closeAllFile()
{
    if(m_pathFile.isOpen()) m_pathFile.close();
    if(m_beaconFile.isOpen()) m_beaconFile.close();
    if(m_beaconRawFile.isOpen()) m_beaconRawFile.close();
    return true;
}
