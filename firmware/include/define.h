#ifndef DEFINE_H
#define DEFINE_H

// states
#define DEFAULT_    0
#define PRE_HEATER  1
#define REFLOW      2
#define POS_REFLOW  3

// control constants
#define KP          2
#define KI          1

// pwm solid state relay
#define pwmSSR      9
#define fan         8

// thermocouple
#define thermoDO    5
#define thermoCS    6
#define thermoCLK   7
#define POT         A0

// pins of the buttons
#define btn1        2
#define btn2        3

// btn filter
#define BTN_FILTER  500

// library and includes
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include <max6675.h>
#include <controle.h>
#include <Wire.h>
#include <temp.h>
#include <interrupt.h>

#endif