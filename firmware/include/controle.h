#ifndef CONTROLE_H
#define CONTROLE_H
#include <define.h>

// states of the control
typedef struct{
    uint8_t     state;
    uint32_t    time_ms;
    uint8_t     set_point;
    String      name;    
} state;

// constants of the control
typedef struct{
    uint8_t     Kp;
    uint32_t    Ki;
    uint16_t    Ts;
    double      i = 0;
} control;

uint32_t controlePI(double y, double s, control &c){
    // error
    double error = s - y;
    
    // integral
    c.i += error * c.Ts/1000;

    // saturate integral
    if(c.i > 255) c.i = 255;
    if(c.i < 0)   c.i = 0;
    
    // control signal
    uint32_t u = c.Kp * error + c.Ki * c.i;
    
    // saturate
    if(u > 255) u = 255;
    if(u < 0)   u = 0;

    return u;
}

#endif