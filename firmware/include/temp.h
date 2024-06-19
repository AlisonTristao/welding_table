#ifndef TEMP_H
#define TEMP_H
#include <Arduino.h>
#include <max6675.h>

// thermocouple
#define thermoDO    5
#define thermoCS    6
#define thermoCLK   7
#define POT         A0

// mean temperature
double mean(uint8_t n_sample, uint8_t delay_ms, MAX6675 thermo){
    double mean = 0;
    for(uint8_t i = 0; i < n_sample; i++){
        mean += thermo.readCelsius();
        delay(delay_ms);
    };
    
    return mean /= n_sample;
}

// reader temperature in celsius
uint32_t get_temp(uint8_t pin){
    // mean
    uint32_t mean;
    for(uint8_t i = 0; i < 10; i++){
        mean = analogRead(pin);
    }

    // analog to celsius
    mean = map(mean, 0, 1023, 0, 240);

    return mean;
}

#endif  