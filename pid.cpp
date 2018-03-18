#include "pid.h"
#include <cmath>
#include <QDebug>
#include <QList>

using namespace std;

class PIDImpl
{
public:
    PIDImpl(double dt,double max,double min,double Kp,double Kd,double Ki);
    ~PIDImpl();
    void setAllowError(double e);
    double calculate(const double setpoint,const double pv);

private:
    double _dt;
    double _max;
    double _min;
    double _Kp;
    double _Kd;
    double _Ki;
    double _pre_error;
    double _integration;
    double _allowError;
    QList<double> _errorList;
    const static int INTEGRATIONLENGTH = 100;
};

PID::PID(double dt,double max,double min,double Kp,double Kd,double Ki)
{
    m_pidImpl = new PIDImpl(dt,max,min,Kp,Kd,Ki);
}

double PID::calculate(double setpoint, double pv)
{
    return m_pidImpl->calculate(setpoint,pv);
}

PID::~PID()
{
    delete m_pidImpl;
}

void PID::setAllowError(double e)
{
    m_pidImpl->setAllowError(e);
}

/**
  *implementation

  * */
PIDImpl::PIDImpl(double dt, double max, double min, double Kp, double Kd, double Ki)
      :_max(max),
      _min(min),
      _Kp(Kp),
      _Kd(Kd),
      _Ki(Ki),
      _pre_error(0)
{
    if(dt <= 0)
    {
        qDebug("Impossible to create a PID regulator with a none loop interval time!");
    }
    else
    {
        _dt = dt;
    }
}

double PIDImpl::calculate(const double setpoint, const double pv)
{
    double _integration = 0;
    double set = setpoint;
    //caculate error
    double error = set - pv;
    if(_allowError==15)
    //qDebug()<<"error:"<<error;
    if(abs(set-pv)<_allowError)
        return 0;
   //proportional term
    double Pout = _Kp * error;
    //qDebug()<<"Pout:"<<Pout;
    //Derivative term
    double Dout = _Kd * (error - _pre_error) / _dt;
    //qDebug()<<"Dout:"<<Dout;
    _pre_error = error;
    //Integral term
    if(_errorList.size()<INTEGRATIONLENGTH)
    {
        _errorList.append(error * _dt);
    }
    else
    {
        _errorList.removeFirst();
        _errorList.append(error * _dt);
    }
    if(_errorList.isEmpty())
        return 0;
    for(int i=0;i<_errorList.size();i++)
    {
       // qDebug()<<"_errorList:"<<_errorList;
        _integration +=  _errorList.at(i);
    }
    //qDebug()<<"error * _dt"<<error * _dt;
    double Iout = _Ki * _integration;
   // qDebug()<<"Iout:"<<Iout;
    //qDebug()<<"_Ki:"<<_Ki;
     if(_allowError==15)
     {/*
          qDebug()<<"Iout:"<<Iout;
          qDebug()<<"_errorList:"<<_errorList;*/
     }

    //Total output
    double output = Pout + Dout + Iout;
    //qDebug()<<"before max output:"<<output<<":Pout:"<<Pout<<"Iout:"<<Iout<<"Dout:"<<Dout;
    //restrict to max/min
    if(output>_max)
        output = _max;
    if(output<_min)
         output = _min;
    return output;
}

PIDImpl::~PIDImpl()
{

}

void PIDImpl::setAllowError(double e)
{
    if(e<0)
        return;
    else
        _allowError = e;
}

