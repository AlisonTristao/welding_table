#ifndef TEMP_H
#define TEMP_H
#include <max6675.h>
#include <define.h>

// measure temperature
double meansure(uint8_t n_sample, uint8_t delay_ms, MAX6675 thermo){
    double meansure = 0;
    for(uint8_t i = 0; i < n_sample; i++){
        meansure += thermo.readCelsius();
        delay(delay_ms);
    }
    return meansure/n_sample;
}

// reader temperature in celsius
uint32_t get_temp(uint8_t pin){
    uint32_t temp;
    for(uint8_t i = 0; i < 10; i++)
        temp = analogRead(pin);
    // analog to celsius
    temp = map(temp, 0, 1023, 0, 240);
    return temp;
}

#endif  