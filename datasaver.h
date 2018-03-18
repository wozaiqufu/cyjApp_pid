#ifndef DATASAVER_H
#define DATASAVER_H
#include <QFile>
#include <QTextStream>
#include <QObject>

typedef struct param
{
    int dis;
    int velocity;
    int angle;
    int courseAngle;
    int acc;
    int offset;
    int angleOrg;
    int hydraulic;
}PARAM;

class DataSaver : public QObject
{
    Q_OBJECT
public:
    explicit DataSaver(QObject *parent = 0);
    void openFiles();
signals:
    void finished();
public slots:
    void slot_doWork();
    void slot_update_dis(int dis);
    void slot_update_angle(int an);
    void slot_update_velocity(int ve);
    void slot_update_courseAngle(int courseAngle);
    void slot_update_lateralOffset(int offset);
    void slot_update_acc(int acc);
    void slot_on_update_angleOrg(int anOrg);
    void slot_on_update_hydraulic(int hyd);
private:
    QFile file;
    QTextStream stream;
    QFile file1;
    QTextStream stream1;
    param m_param;
    int m_lastDis;
    static const int disDelta = 150;//in cm
    static const int ANGLEMIDDLE = 44;
};

#endif // DATASAVER_H
