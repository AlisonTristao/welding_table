#ifndef INTERRUPT_H
#define INTERRUPT_H
#include <controle.h>
#include <define.h>

// variables
extern uint32_t time_filter_1;
extern uint32_t time_filter_2;
extern uint32_t current_time_states;
extern uint8_t idx_states;
extern bool states_on;

// skip to the next state
void next_state(){
    // filter time and state flow on
    if ((millis() - time_filter_1) < BTN_FILTER || !states_on) return;
    
    // next state
    idx_states++;
    if (idx_states == 4) idx_states = 0;

    // filter
    time_filter_1 = millis();
    current_time_states = millis();
}

// turn off the state flow
void states_off(){
    // filter
    if ((millis() - time_filter_2) < BTN_FILTER) return;

    // turn off/on the states
    idx_states = 0;
    states_on = !states_on;

    // filter
    time_filter_2 = millis();
    current_time_states = millis();
}

void init_btn(){
    pinMode(btn1, INPUT_PULLUP);
    pinMode(btn2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(btn1), next_state, LOW);
    attachInterrupt(digitalPinToInterrupt(btn2), states_off, LOW);
}

#endif  