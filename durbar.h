#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


//Classes

class Led
{ // led
public:
    // constructor
    Led(int pin);

    // methods
    void on();
    void off();
    void Blink(int onPulse, int offPulse);

private:
    int pinNumber;
};


class RemoteController
{ //Remote Controller
public:
    //constructor
    RemoteController(uint8_t ch1, uint8_t ch2, uint8_t ch3, uint8_t ch4, uint8_t ch5, uint8_t ch6);

    //variables

    //methods
    void up_data();
    void show_data();
    void update_data(int * arr);
    void set_mode();
    void show_mode();
    char mode = 'd';
    

private:
    int ch[6] = {0, 0, 0, 0, 0, 0};
    int ch_pin[6] = {0, 0, 0, 0, 0, 0};
};

class Motor
{ // motor
public:
    // constructor
    Motor(uint8_t r_pwm, uint8_t l_pwm);

    // methods
    void forward(uint8_t motor_speed);
    void backward(uint8_t motor_speed);
    void stop();

private:
    uint8_t left_pwm;
    uint8_t right_pwm;
};

class Rover
{
public:
    //don't know why I have to pass the rc object in Rover object. But this is how it works. Otherwise it just doesn't. 
    Rover(Motor * FL_m, Motor * FR_m, Motor * BL_m, Motor * BR_m, RemoteController * rc_);


    void forward(uint8_t r_speed);
    void backward(uint8_t r_speed);
    void turn_right(uint8_t r_speed, uint8_t s_diff);
    void turn_left(uint8_t r_speed, uint8_t s_diff);
    void turn_right_hard(uint8_t r_speed);
    void turn_left_hard(uint8_t r_speed);
    void stop();
    void rc_drive();
    void set_max_rover_speed(uint8_t r_speed);
    void show_rover_motor_speeds();

private:
    void rc_mapped_data_update(int * arr);
    Motor * FR_motor;
    Motor * FL_motor;
    Motor * BR_motor;
    Motor * BL_motor;
    RemoteController * rc;
    int ch[6];
    int ch_mapped[6];
    uint8_t max_rover_speed = 255;
    int right_motor_speed = 0;
    int left_motor_speed = 0;
};

class Relay
{
public:
    Relay(uint8_t clk, uint8_t anticlk);

    void rotate_clockwise();
    void rotate_anticlockwise();
    void stop();    

private:
    uint8_t clk_pin;
    uint8_t anticlk_pin;
};

class Arm
{
public:
    Arm(Relay * base_, Relay * joint1_, Relay * joint2_, Relay * joint3_, Relay * end_rotator_, Relay * claw_, RemoteController * rc_);

    void rc_drive();
    void show_actuator_state();

private:
    Relay * base;
    Relay * joint1;
    Relay * joint2;
    Relay * joint3;
    Relay * end_rotator;
    Relay * claw;
    RemoteController * rc;
    int ch[6];
    uint16_t switch_thresh = 200;
    int actuator_state[6] = {0, 0, 0, 0, 0, 0};
};
