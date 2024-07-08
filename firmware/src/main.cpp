// define all inclusions and variables
#include <define.h>

// temperature
MAX6675 thermo(thermoCLK, thermoCS, thermoDO);

// control
control pi_control;

// states
state states[4];

// degree symbol
byte grau[8] = {
  B00111,
  B00101,
  B00111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

// lcd display with address hex 0x27, 16 columns and 2 lines
LiquidCrystal_I2C lcd(0x27, 16, 2);

// global variables
uint8_t     idx_states             = 0;    // index of the states
uint32_t    time_filter_1          = 0;    // filter time btn1
uint32_t    time_filter_2          = 0;    // filter time btn2
uint32_t    current_time_states    = 0;    // time of the states
uint32_t    timer                  = 0;    // time of the loop
uint16_t    u                      = 0;    // signal
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

    // add º to the display
    lcd.createChar(0, grau);

    // init fan and pwm
    pinMode(fan, OUTPUT);
    pinMode(pwmSSR, OUTPUT);

    /* ----- states ----- */ 
    // default
    states[0].state     = DEFAULT_;
    states[0].name      = "OFF";
    states[0].time_ms   = 0;
    states[0].set_point = 0;

    // pre heater
    states[1].state     = PRE_HEATER;
    states[1].name      = "HEATER";
    states[1].time_ms   = 180e3;
    states[1].set_point = 150;

    // reflow
    states[2].state     = REFLOW;
    states[2].name      = "REFLOW";
    states[2].time_ms   = 60e3;
    states[2].set_point = 200;

    // pos-reflow
    states[3].state     = POS_REFLOW;
    states[3].name      = "POSREF";
    states[3].time_ms   = 45e3;
    states[3].set_point = 500;

    // gain and time of the control
    pi_control.Kp = KP;
    pi_control.Ki = KI;
    pi_control.Ts = 500;
}

void loop() {
    // buttons pressed 
    if(digitalRead(btn1) == LOW) next_state();
    if(digitalRead(btn2) == LOW) states_off();

    // start time
    timer = millis(); 

    // meansure temperature
    temp = meansure(10, 10, thermo);

    // manual setpoint
    sp = get_temp(POT);

    // skip to the next state
    if(states[idx_states].state != DEFAULT_) 
        if((millis() - current_time_states) > states[idx_states].time_ms) 
            next_state();

    // control signal
    if(!states_on)   u = controlePI(temp, sp, pi_control);
    else             u = controlePI(temp, states[idx_states].set_point, pi_control);

    // apply control
    analogWrite(pwmSSR, u);

    // clear display
    lcd.clear();

    // time of the state
    float timer_t = (millis() - current_time_states)/1000;

    // print temperature
    lcd.setCursor(0, 0);
    lcd.print("T: ");
    lcd.print(temp, 0);

    // degree symbol
    lcd.write(byte(0));
    lcd.print("C");

    // if states flow is on
    if(states_on) {
        // print state name
        lcd.setCursor(0, 1);
        lcd.print("F: ");
        lcd.print(states[idx_states].name);
        // print time of the state
        if(states[idx_states].state != DEFAULT_) {
            lcd.print(" ");
            lcd.print(timer_t, 0);
            lcd.print("s");
        }
    } else {
        // print manual setpoint
        lcd.setCursor(0, 1);
        lcd.print("R: ");
        lcd.print(sp, 0);
        // degree symbol
        lcd.write(byte(0));
        lcd.print("C");
    }

    // delay
    while((millis() - timer) < 500) {
    }
}