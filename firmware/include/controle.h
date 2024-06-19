#ifndef CONTROLE_H
#define CONTROLE_H
#include <Arduino.h>

#define DEFAULT     0
#define PRE_HEATER  1
#define REFLOW      2
#define POS_REFLOW  3
#define MANUAL      4

// control constants
#define TS          500
#define KP          25
#define KI          1

// pwm solid state relay
#define pwmSSR      9
#define fan         8

// states of the control
typedef struct{
    uint8_t     state;
    uint32_t    time_ms;
    uint8_t     set_point;
} state;

// constants of the control
typedef struct{
    uint8_t     Kp;
    uint32_t    Ki;
    uint8_t     Ts;
} control;

double i = 0;
uint32_t controlePI(double y, double s, control &c){
    // error
    double error = s - y;
    
    // integral
    i += error * c.Ts/1000;

    // saturate integral
    if(i > 255) i = 255;
    if(i < 0)   i = 0;
    
    // control signal
    uint32_t u = c.Kp * error + c.Ki * i;
    
    // saturate
    if(u > 255) u = 255;
    if(u < 0)   u = 0;
    
    return u;
}

#endif