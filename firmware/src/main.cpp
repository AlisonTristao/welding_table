// define all inclusions and variables
#include <define.h>

// temperature
MAX6675 thermo(thermoCLK, thermoCS, thermoDO);

// control
control pi_control;

// states
state states[4];

// lcd display with address hex 0x27, 16 columns and 2 lines
LiquidCrystal_I2C lcd(0x27, 16, 2);

// global variables
uint8_t     idx_states             = 0;    // index of the states
uint32_t    time_filter_1          = 0;    // filter time btn1
uint32_t    time_filter_2          = 0;    // filter time btn2
uint32_t    current_time_states    = 0;    // time of the states
uint32_t    timer                  = 0;    // time of the loop
uint32_t    u                      = 0;    // signal
double      temp                   = 0;    // temperature of table
double      sp                     = 0;    // manual setpoint 
bool        states_on              = true; // states flow on/off


void setup() {
    // interrupts
    init_btn();

    // init wire I2C
    Wire.begin();

    // init display
    lcd.begin();
    lcd.setBacklight(HIGH);

    // init fan and pwm
    pinMode(fan, OUTPUT);
    pinMode(pwmSSR, OUTPUT);

    /* --- states --- */ 
    // default
    states[0].state     = DEFAULT_;
    states[0].name      = "OFF";
    states[0].time_ms   = 0;
    states[0].set_point = 0;

    // pre heater
    states[1].state     = PRE_HEATER;
    states[1].name      = "PRE_HEATER";
    states[1].time_ms   = 10e3;
    states[1].set_point = 30;

    // reflow
    states[2].state     = REFLOW;
    states[2].name      = "REFLOW";
    states[2].time_ms   = 20e3;
    states[2].set_point = 40;

    // pos-reflow
    states[3].state     = POS_REFLOW;
    states[3].name      = "POS_REFLOW";
    states[3].time_ms   = 30e3;
    states[3].set_point = 50;

    // gain and time of the control
    pi_control.Kp = KP;
    pi_control.Ki = KI;
    pi_control.Ts = 500;
}

void loop() {
    // start time
    timer = millis(); 

    // meansure temperature
    temp = meansure(10, 10, thermo);

    // manual setpoint
    sp = get_temp(POT);

    // state flow
    if(states[idx_states].state != DEFAULT_){
        // skip to the next state
        if((millis() - current_time_states) > states[idx_states].time_ms) 
            next_state();
    }else{
        // turn on/off fan 
        if(states_on && temp > 50)  digitalWrite(fan, HIGH);
        else                        digitalWrite(fan, LOW);
    }

    // control signal
    if(!states_on)  u = controlePI(temp, sp, pi_control);
    else            u = controlePI(temp, states[idx_states].set_point, pi_control);

    // apply control
    analogWrite(pwmSSR, u);

    // delay
    while((millis() - timer) < 500){
        // atualiza display
        
    }
}