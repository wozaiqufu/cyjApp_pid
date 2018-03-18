#include <QtCore>
#include "SICK511.h"
#include <QDebug>


SICK511::SICK511(QObject* parent)
: QObject(parent),
m_isOn(false),
m_numberDataOneSocket(0),
m_courseAngle(0),
m_lateralOffset(0),
m_dataInUse(false),
_doWorkCount(0)
{
	connect(&m_tcpSocket, SIGNAL(readyRead()), this, SLOT(slot_on_readMessage()));
	connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slot_on_error(QAbstractSocket::SocketError)));
}

SICK511::~SICK511()
{
	if (m_isOn)
	{
		continuousStop();
	}
	m_tcpSocket.close();
}

bool SICK511::init(const QString name, const QString ip, const int port)
{
	m_name = name;
	m_ip = ip;
	m_port = port;
    if(m_name=="forward")
    {
        m_c_ANGLESACLE = 30;
        m_c_L1L2ANGLE = 30;
        m_c_STARTANGLE = 15;
    }
    else
    {
        m_c_ANGLESACLE = 30;
        m_c_L1L2ANGLE = 30;
        m_c_STARTANGLE = 0;
    }
	m_tcpSocket.abort();
	m_tcpSocket.connectToHost(m_ip, m_port);
    if (m_tcpSocket.waitForConnected(MILSECONDSWAIT))
	{
        qDebug()<<m_name<<"init succeed!";
		emit sig_statusTable(m_name + "init succeeded");
		m_isOn = true;
		return true;
	}
	else return false;
}

bool SICK511::isOn()
{
    return m_isOn == true;
}

void SICK511::useData(const bool b)
{
    m_dataInUse = b;
}

void SICK511::continuousStart()
{
	QString qstr("\x2sEN LMDscandata 1\x3");
    qDebug() << "request at:" << QTime::currentTime();
	m_dataRecieved.clear();
	m_tcpSocket.write(qstr.toLocal8Bit());
}

void SICK511::continuousStop()
{
	QString qstr("\x2sEN LMDscandata 0\x3");
    //qDebug() << "request at:" << QTime::currentTime();
	m_dataRecieved.clear();
	m_tcpSocket.write(qstr.toLocal8Bit());
}

void SICK511::slot_on_readMessage()
{
	m_dataRecieved.clear();
    m_dataRecieved = m_tcpSocket.readAll();
//    if(m_dataInUse)
//        qDebug() << "SICK511 CLASS:SICK511 of " + m_name + " is in use";
    if(!m_dataInUse) return;
    extractDISTData();
    //extractRSSIData();
    qDebug()<<"++++++++++++++++in use SICK:"<<m_name;
    emit sigUpdateCourseAngle(courseAngle());
    emit sigUpdateLateralOffset(lateralOffset(),m_name);
}
bool SICK511::isDataLoaded()
{
   // qDebug()<<"m_DISTdata size:"<<m_DISTdata.size();
//	return (m_DISTdata.size() > 179 && m_RSSIdata.size() > 179);
    return (m_DISTdata.size()==361);
}

void SICK511::extractDISTData()
{
    m_DISTdata.clear();
    QList<QString> dataList;
    splitByte(m_dataRecieved,dataList);
    //qDebug()<<"dataList is:"<<dataList;
    if(!dataList.contains("00000000"))
        return;
    if(!dataList.contains("0"))
    return;
    if(!dataList.contains("169"))
    return;
    int dataLength = dataList.indexOf("0") - dataList.indexOf("169");
    //qDebug()<<"dataLength"<<dataLength;
    if(dataLength<367)
        return;
    for(int i=0;i<361;i++)
    {
        bool ok = false;
        int data_dec = 2*dataList.at(dataList.indexOf("169") + i + 1).toInt(&ok,16);
        if(ok)
        {
            m_DISTdata.append(data_dec);
        }
    }
    //qDebug() << "dist of " + m_name + ":" << m_DISTdata;
//    if(m_dataRecieved.isEmpty()){
//        return;
//    }
//    //if using the permanent mode,the first diagram is the confirmation,not the DATA
//	if (!m_dataRecieved.contains("00000000")){
//        return;
//    }
//    QString eightZeros("00000000");
//    int zerosStartPos = m_dataRecieved.indexOf(eightZeros);
//    zerosStartPos = zerosStartPos + 9;//index of Starting Angle
//    //crop Starting Angle
//    int startAngle_End = m_dataRecieved.indexOf(" ",zerosStartPos);
//    QByteArray startAngle = m_dataRecieved.mid(zerosStartPos,startAngle_End-zerosStartPos);
//    //qDebug()<<"startAngle:"<<startAngle;
//    //crop Angular step width
//    int AngularStep_Begin = startAngle_End + 1;
//    QByteArray angularStep = m_dataRecieved.mid(AngularStep_Begin,m_dataRecieved.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
//    //qDebug()<<"angular step:"<<angularStep;
//    //crop Number Data
//    int NumberData_Begin = m_dataRecieved.indexOf(" ",AngularStep_Begin) + 1;
//    QByteArray numberData = m_dataRecieved.mid(NumberData_Begin,m_dataRecieved.indexOf(" ",NumberData_Begin)-NumberData_Begin);
//    //qDebug()<<"number data in Hex:"<<numberData;
//    //crop Data:in order to completely crop data,we need to transform Hex to Decimal
//    bool ok;
//    m_numberDataOneSocket = numberData.toInt(&ok,16);
//	m_angleResolution = 180.0 / (m_numberDataOneSocket - 1);
//	//qDebug() << "m_angleResolution is:" << m_angleResolution;
//    int data_index = m_dataRecieved.indexOf(" ",NumberData_Begin) +1;
//     //qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
//    //crop data begins
//     m_DISTdata.clear();//CAUTION!m_data_forward vector must be cleaned before new data is pushed!
//     for(int i=0;i<m_numberDataOneSocket;i++)
//     {
//         QByteArray iData = m_dataRecieved.mid(data_index,m_dataRecieved.indexOf(" ",data_index)-data_index);
//         bool ok;
//         int iData_Dec = iData.toInt(&ok,16);
//         //if 40000000,returned data*2 is the real value in decimal
//         iData_Dec = 2*iData_Dec;
//         if(ok){
//             m_DISTdata.push_back(iData_Dec);
//             //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
//         }
//         data_index = m_dataRecieved.indexOf(" ",data_index) + 1;
//     }
//     qDebug() << "dist of " + m_name + ":" << m_DISTdata;
//     //qDebug()<<"size of m_data_forward:"<<m_data_forward.size();
}

void SICK511::extractRSSIData()
{
    /***********************forward************************************/
    if(m_dataRecieved.isEmpty()){
        return;
    }
    //if using the permanent mode,the first diagram is the confirmation,not the DATA
    if(!m_dataRecieved.contains("00000000")){
        return;
    }
    QString eightZeros("00000000");
    int zerosStartPos = m_dataRecieved.lastIndexOf(eightZeros);
    zerosStartPos = zerosStartPos + 9;//index of Starting Angle
    //crop Starting Angle
    int startAngle_End = m_dataRecieved.indexOf(" ",zerosStartPos);
    QByteArray startAngle = m_dataRecieved.mid(zerosStartPos,startAngle_End-zerosStartPos);
    //qDebug()<<"startAngle:"<<startAngle;
    //crop Angular step width
    int AngularStep_Begin = startAngle_End + 1;
    QByteArray angularStep = m_dataRecieved.mid(AngularStep_Begin,m_dataRecieved.indexOf(" ",AngularStep_Begin)-AngularStep_Begin);
    //qDebug()<<"angular step:"<<angularStep;
    //crop Number Data
    int NumberData_Begin = m_dataRecieved.indexOf(" ",AngularStep_Begin) + 1;
    QByteArray numberData = m_dataRecieved.mid(NumberData_Begin,m_dataRecieved.indexOf(" ",NumberData_Begin)-NumberData_Begin);
    //qDebug()<<"number data in Hex:"<<numberData;
    //crop Data:in order to completely crop data,we need to transform Hex to Decimal
    bool ok;
    m_numberDataOneSocket = numberData.toInt(&ok,16);
    int data_index = m_dataRecieved.indexOf(" ",NumberData_Begin) +1;
     //qDebug()<<"number data in decimal :"<<m_numberDataOneSocket;
    //crop data begins
     m_RSSIdata.clear();//CAUTION!m_data_forward vector must be cleaned before new data is pushed!
     for(int i=0;i<m_numberDataOneSocket;i++)
     {
         QByteArray iData = m_dataRecieved.mid(data_index,m_dataRecieved.indexOf(" ",data_index)-data_index);
         bool ok;
         int iData_Dec = iData.toInt(&ok,16);
         //if 40000000,returned data*2 is the real value in decimal
        // iData_Dec = 2*iData_Dec;
         if(ok){
             m_RSSIdata.push_back(iData_Dec);
             //qDebug()<<"the "<<i<<" th data is: "<<iData_Dec;
         }
         data_index = m_dataRecieved.indexOf(" ",data_index) + 1;
     }
     //qDebug()<<"\n"<<"m_RSSIdata_forward in decimal:"<<m_RSSIdata_forward;
     //qDebug()<<"size of m_RSSIdata_forward:"<<m_RSSIdata_forward.size();
}

int SICK511::angle2index(const int angle)
{
	if (angle > 180 || angle < 0)
		return -1;
    return angle / ANGLERESOLUTION;
}

void SICK511::splitByte(QString str, QList <QString> &data)
{
    QStringList list;
       list=str.split(QRegExp(" "));
       //qDebug()<<"list"<<list;
       foreach (QString i,list)
       {
           data.append(i);
       }
}

void SICK511::slot_on_addTickCount()
{
    if(_doWorkCount<32768)
        _doWorkCount++;
}

//calculate course angle
int SICK511::courseAngle()
{
    //qDebug()<<"courseAngle";
	if (!isDataLoaded())
	{
		return m_courseAngle;
	}
    //qDebug()<<"first and last data:"<<m_DISTdata.at(0)<<"  "<<m_DISTdata.at(360);
    //qDebug()<<"m_angleResolution:"<<m_angleResolution;
    int courseAngle_right = 0;
    int courseAngle_left = 0;
    if ((m_DISTdata.size() == DATANUMBER))
	{
		//right side
		double sumCourse = 0;
		int validCount = 0;
        //qDebug() << "total index is:" << ANGLESACLE / m_angleResolution;
        for (int i = 0; i<m_c_ANGLESACLE/ANGLERESOLUTION; i++)
		{
			//l1!=0&&l2!=0
            if ((m_DISTdata.at(angle2index(m_c_STARTANGLE) + i) == 0) || (m_DISTdata.at(angle2index(m_c_STARTANGLE + m_c_L1L2ANGLE) + i) == 0))
			{
				continue;
			}
			//qDebug()<<"Valid count======:"<<validCount;
			//beta
            double l1 = m_DISTdata.at(angle2index(m_c_STARTANGLE) + i);
            //qDebug() << "right l1 data index:" << angle2index(STARTANGLE) + i;
            double l2 = m_DISTdata.at(angle2index(m_c_STARTANGLE + m_c_L1L2ANGLE) + i);
            //qDebug() << "right l2 data index:" << angle2index(STARTANGLE + L1L2ANGLE) + i;
			double l3 = sqrt(pow(l1, 2)
				+ pow(l2, 2)
                - 2 * l1*l2*cos(m_c_L1L2ANGLE*ANGLEDEGREE2RADIUS));
			//qDebug()<<"l3:"<<l3;
			double cos_beta = (pow(l1, 2) + pow(l3, 2) - pow(l2, 2)) / (2 * l1*l3);
			//qDebug()<<"cos_beta"<<cos_beta;
            double icourse = 90 + (m_c_STARTANGLE + i * ANGLERESOLUTION) - acos(cos_beta) / ANGLEDEGREE2RADIUS;
			//qDebug()<<"icourse:"<<icourse;
			//sum of course
			sumCourse += icourse;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "course angle all l1 l2 are invalid!";
            courseAngle_right = 0;
		}
		else
		{
			courseAngle_right = sumCourse / validCount;
		}
        //qDebug()<<"courseAngle_right"<<courseAngle_right;
		//left side
		sumCourse = 0;
		validCount = 0;
        for (int i = 0; i<m_c_ANGLESACLE/ANGLERESOLUTION; i++)
		{
			//l1!=0&&l2!=0
            if ((m_DISTdata.at(angle2index(180 - m_c_STARTANGLE) - i) == 0) || (m_DISTdata.at(angle2index(180 - m_c_STARTANGLE - m_c_L1L2ANGLE) - i) == 0))
			{
				continue;
			}
			//qDebug()<<"Valid count======:"<<validCount;
			//beta
            double l1 = m_DISTdata.at(angle2index(180 - m_c_STARTANGLE) - i);
            //qDebug() << "left l1 data index:" << angle2index(180 - STARTANGLE - i);
            double l2 = m_DISTdata.at(angle2index(180 - m_c_STARTANGLE - m_c_L1L2ANGLE) - i);
            //qDebug() << "left l2 data index:" << angle2index(180 - STARTANGLE - L1L2ANGLE - i);
            double l3 = sqrt(pow(l1, 2) + pow(l2, 2) - 2 * l1*l2*cos(m_c_L1L2ANGLE*ANGLEDEGREE2RADIUS));
			//qDebug()<<"l1:"<<l1;
			//qDebug()<<"l2:"<<l2;
			//qDebug()<<"l3:"<<l3;
			double cos_beta = (pow(l1, 2) + pow(l3, 2) - pow(l2, 2)) / (2 * l1*l3);
			//qDebug()<<"beta:"<<acos(cos_beta)/m_Angle_degree2Radian;
			//sum of beta
            double icourse = acos(cos_beta) / ANGLEDEGREE2RADIUS - 90 - i * ANGLERESOLUTION - m_c_STARTANGLE;
			//qDebug()<<"icourse"<<icourse;
			sumCourse += icourse;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "course angle all l1 l2 are invalid!";
            courseAngle_left = 0;
		}
		else
		{
			courseAngle_left = sumCourse / validCount;
		}
        //qDebug()<<"courseAngle_left"<<courseAngle_left;
        //m_courseAngle = (courseAngle_left + courseAngle_right) / 2;
        m_courseAngle = courseAngle_right;
        //qDebug()<<"courseangle of "<<m_name<<m_courseAngle;
	}
	//return (courseAngle_left+courseAngle_right)/2;
	//qDebug()<<"=====================================================>";
	//return courseAngle_right;//is ok
    //qDebug() << "course angle is:" << m_courseAngle;
	return m_courseAngle;
}

//calculate lateral offset use both Caution:if return 2000,no available data!
int SICK511::lateralOffset()
{
	if (!isDataLoaded())
	{
		return m_lateralOffset;
	}
    //qDebug()<<"courseAngle is triggered!";
     //qDebug()<<"m_currentDirection:"<<m_currentDirection;
    //qDebug()<<"m_data_forward"<<m_data_forward;
    int lateral = 1;
    if ((m_DISTdata.size() == DATANUMBER))
	{
        //qDebug()<<"m_angleResolution>0.20"<<m_angleResolution;
		double sumH1 = 0;
		double sumH2 = 0;
		double H1 = 0;
		double H2 = 0;
		int validCount = 0;
        for (int i = 0; i < m_c_ANGLESACLE / ANGLERESOLUTION; i++)
		{
			//right side h1
            double l = m_DISTdata.at(i*ANGLERESOLUTION + angle2index(m_c_STARTANGLE));
			if (l == 0)
			{
				continue;
			}
            double h1 = l*cos(ANGLEDEGREE2RADIUS*(m_courseAngle - m_c_STARTANGLE - i*ANGLERESOLUTION));
			//                qDebug()<<"l"<<l;
			//                qDebug()<<"valid:"<<validCount;
			//                qDebug()<<"h1:"<<h1;
			sumH1 += h1;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "no data for H1";
			return 2000;
		}
		H1 = sumH1 / validCount;
       //qDebug()<<"H1"<<H1;
       //left
		validCount = 0;
        for (int i = 0; i < m_c_ANGLESACLE/ANGLERESOLUTION; i++)
		{
			//left side h2
            double l = m_DISTdata.at(angle2index(180 - m_c_STARTANGLE) - i*ANGLERESOLUTION);
			if (l == 0)
			{
				continue;
			}
            double h2 = l*cos(ANGLEDEGREE2RADIUS*(m_courseAngle + m_c_STARTANGLE + i * ANGLERESOLUTION));
			sumH2 += h2;
			validCount++;
		}
		if (validCount == 0)
		{
			qDebug() << "no data for H1";
			return 2000;
		}
        H2 = sumH2 / validCount;
        //lateral = (H2 - H1) / 2;
        m_lateralOffset = H1;
        qDebug()<<"lateral offset of "<<m_name<<m_lateralOffset;
		return m_lateralOffset;

	}
    else return m_lateralOffset;
}

int SICK511::frontDistance()
{
//    for(int i=0;i<(FRONT_ANGLE_END-FRONT_ANGLE_START)/m_angleResolution;i++)
//    {

//    }
}

void SICK511::slot_on_error(QAbstractSocket::SocketError socktError)
{
     qDebug()<<"SICK 511 error";
     switch(socktError)
     {
     case QAbstractSocket::RemoteHostClosedError:
         emit sig_statusTable("SICK511 RemoteHostClosedError:"+m_tcpSocket.errorString());
         break;
     case QAbstractSocket::DatagramTooLargeError:
         emit sig_statusTable("SICK511 DatagramTooLargeError:"+m_tcpSocket.errorString());
         break;
     case QAbstractSocket::NetworkError:
         emit sig_statusTable("SICK511 NetworkError:"+m_tcpSocket.errorString());
         break;
     default:
         break;
     }
	 m_isOn = false;
}

QString SICK511::name() const
{
	return m_name;
}
QString SICK511::ip() const
{
	return m_ip;
}
int SICK511::port() const
{
    return m_port;
}

int SICK511::doWorkCount() const
{
    return _doWorkCount;
}
