#ifndef PID_H
#define PID_H

class PIDImpl;
class PID
{
public:
    PID(double dt,double max,double min,double Kp,double Kd,double Ki);
    ~PID();
    void setAllowError(double e);
    double calculate(double setpoint,double pv);
private:
    PIDImpl *m_pidImpl;
};

#endif // PID_H
