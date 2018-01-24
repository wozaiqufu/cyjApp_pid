#ifndef CYJDATA
#define CYJDATA
struct CYJData
{
    unsigned char startdata1;
    unsigned char startdata2;
    unsigned char forward:1;
    unsigned char backward:1;
    unsigned char neutral:1;
    unsigned char stop:1;
    unsigned char scram:1;
    unsigned char light:1;
    unsigned char horn:1;
    unsigned char zero:1;

    unsigned char manualVisual:1;//io4
    unsigned char localRemote:1;
    unsigned char start:1;
    unsigned char flameout:1;
    unsigned char middle:1;
    unsigned char warn1:1;
    unsigned char warn2:1;
    unsigned char warn3:1;

    unsigned char rise;
    unsigned char fall;
    unsigned char turn;
    unsigned char back;
    unsigned char left;
    unsigned char right;
    unsigned char acc;
    unsigned char deacc;
    unsigned char speed;
    unsigned char engine;
    unsigned char spliceAngle;
    unsigned char oil;
    unsigned char temperature;
    unsigned char enddata;
};

#endif // CYJDATA

