#include <LiquidCrystal_I2C.h>
#include <max6675.h>
#include <Arduino.h>
#include <Wire.h>
#include "temp.h"
#include "controle.h"
#include "interrupt.h"

// temperature
MAX6675 thermo(thermoCLK, thermoCS, thermoDO);

// control
control pi_control;

// states
state states[4];

//Inicializa o display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);

// variables
uint8_t idx_states = 0;             // index of the states
bool states_on = true;             // states on/off
uint32_t time_filter_1 = 0;         // filter time btn1
uint32_t time_filter_2 = 0;         // filter time btn2
uint32_t current_time_states = 0;   // time of the states

void setup() {
    Serial.begin(9600);
    // interrupções
    init_btn();

    // Inicia Wire I2C
    Wire.begin();

    //Inicializa display
    lcd.begin();
    lcd.setBacklight(HIGH);

    // init fan and pwm
    pinMode(fan, OUTPUT);
    pinMode(pwmSSR, OUTPUT);

    // states
    // default
    states[0].state = DEFAULT;
    states[0].time_ms = 0;
    states[0].set_point = 0;

    // pre heater
    states[1].state = PRE_HEATER;
    states[1].time_ms = 10e3;
    states[1].set_point = 30;

    // reflow
    states[2].state = REFLOW;
    states[2].time_ms = 20e3;
    states[2].set_point = 40;

    // pos-reflow
    states[3].state = POS_REFLOW;
    states[3].time_ms = 30e3;
    states[3].set_point = 50;

    // gain and time of the control
    pi_control.Kp = KP;
    pi_control.Ki = KI;
    pi_control.Ts = TS;
}

void loop() {
    // time
    uint32_t timer = millis(); 

    // mean temperature
    double temp = mean(10, 10, thermo);

    // manual setpoint
    double sp = get_temp(POT);

    // control signal
    uint32_t u = 0;

    // controle manual
    if(!states_on){
        u = controlePI(temp, sp, pi_control);
        analogWrite(pwmSSR, u);
        delay(500 - (millis() - timer));

        lcd.setCursor(0, 0);
        lcd.print("F:");
        lcd.print(idx_states);
        lcd.print(" sp:");
        lcd.print(sp);
        lcd.setCursor(0, 1);
        lcd.print("T:");
        lcd.print(temp, 1);
        return;
    }

    lcd.setCursor(0, 0);
    lcd.print("F:");
    lcd.print(idx_states);
    lcd.print(" t:");
    String tempo = ((millis()-current_time_states)/1000) < 1 ? "   " : String((millis()-current_time_states)/1000);
    lcd.print(tempo);
    lcd.setCursor(0, 1);
    lcd.print("T:");
    lcd.print(temp, 1);

    // default
    if(states[idx_states].state == DEFAULT){
        analogWrite(pwmSSR, 0);
        return;
    }

    // control
    u = controlePI(temp, states[idx_states].set_point, pi_control);
    analogWrite(pwmSSR, u);

    // states
    if((millis() - current_time_states) > states[idx_states].time_ms) next_state();

    // delay
    delay(500 - (millis() - timer));
}