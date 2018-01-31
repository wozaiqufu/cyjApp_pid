#include "pid.h"
#include <cmath>
#include <QDebug>
using namespace std;

class PIDImpl
{
public:
    PIDImpl(double dt,double max,double min,double Kp,double Kd,double Ki);
    ~PIDImpl();
    void setAllowError(double e);
    double calculate(double setpoint,double pv);

private:
    double _dt;
    double _max;
    double _min;
    double _Kp;
    double _Kd;
    double _Ki;
    double _pre_error;
    double _integral;
    double _allowError;
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
      _pre_error(0),
      _integral(0)
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

double PIDImpl::calculate(double setpoint, double pv)
{
    //caculate error
    double error = setpoint - pv;
    if(abs(setpoint-pv)<_allowError)
        return 0;
   //proportional term
    double Pout = _Kp * error;

    //Derivative term
    double derivative = (error - _pre_error) / _dt;
    double Dout = _Kd * derivative;
    _pre_error = error;
    //Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    //Total output
    double output = Pout + Dout + Iout;
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

