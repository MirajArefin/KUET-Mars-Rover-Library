#include "durbar.h"

// constructor define
Led::Led(int pin)
{ // LED constructor
    pinNumber = pin;
    pinMode(pinNumber, OUTPUT);
}

RemoteController::RemoteController(uint8_t ch1, uint8_t ch2, uint8_t ch3, uint8_t ch4, uint8_t ch5, uint8_t ch6)
{ // Remote Controller constructor
    ch_pin[0] = ch1;
    ch_pin[1] = ch2;
    ch_pin[2] = ch3;
    ch_pin[3] = ch4;
    ch_pin[4] = ch5;
    ch_pin[5] = ch6;

    for (int i = 0; i < 6; i++)
    {
        pinMode(ch_pin[i], INPUT);
    }

    for (int i = 0; i < 6; i++)
    {
        pinMode(ch_pin[i], INPUT);
    }
}

Motor::Motor(uint8_t r_pwm, uint8_t l_pwm)
{ // Motor constructor
    left_pwm = l_pwm;
    right_pwm = r_pwm;
    pinMode(left_pwm, OUTPUT);
    pinMode(right_pwm, OUTPUT);
}

Rover::Rover(Motor *FL_m, Motor *FR_m, Motor *BL_m, Motor *BR_m, RemoteController *rc_)
{ // Rover constructor
    FR_motor = FR_m;
    FL_motor = FL_m;
    BR_motor = BR_m;
    BL_motor = BL_m;
    rc = rc_;
}

Relay::Relay(uint8_t clk, uint8_t anticlk)
{ // Relay constructor
    clk_pin = clk;
    anticlk_pin = anticlk;

    pinMode(clk_pin, OUTPUT);
    pinMode(anticlk_pin, OUTPUT);
}

Arm::Arm(Relay *base_, Relay *joint1_, Relay *joint2_, Relay *joint3_, Relay *end_rotator_, Relay *claw_, RemoteController *rc_)
{
    base = base_;
    joint1 = joint1_;
    joint2 = joint2_;
    joint3 = joint3_;
    end_rotator = end_rotator_;
    claw = claw_;
    rc = rc_;
}

// LED method definitions
void Led::on()
{ // led set to on
    digitalWrite(pinNumber, HIGH);
    return;
}
void Led::off()
{ // led set to off
    digitalWrite(pinNumber, LOW);
    return;
}
void Led::Blink(int onPulse, int offPulse)
{ // Led blink
    digitalWrite(pinNumber, HIGH);
    delay(onPulse);
    digitalWrite(pinNumber, LOW);
    delay(offPulse);
    return;
}

// Remote Controller methods

void RemoteController::up_data()
{
    for (int i = 0; i < 6; i++)
    {
        ch[i] = pulseIn(ch_pin[i], HIGH);
    }
    return;
}

void RemoteController::show_data()
{
    this->RemoteController::up_data(); // updating received data

    for (int i = 0; i < 6; i++)
    {
        Serial.print(ch[i]);
        Serial.print("  ");
    }
    Serial.println();
    return;
}

void RemoteController::update_data(int *arr)
{
    this->RemoteController::up_data(); // updating received data
    for (int i = 0; i < 6; i++)
    {
        arr[i] = ch[i];
    }
}

void RemoteController::set_mode()
{
    this->up_data();
    if (ch[4] < 1200)
        mode = 'd';
    else if (ch[4] > 1800)
        mode = 's';
    else
    {
        if (ch[5] > 1500)
            mode = 'A';
        else
            mode = 'a';
    }
}

void RemoteController::show_mode()
{
    this->set_mode();
    Serial.print("Mode : ");
    switch (mode)
    {
    case 'd':
        Serial.print("Drive");
        break;
    case 'A':
        Serial.print("Arm_1");
        break;
    case 'a':
        Serial.print("Arm_2");
        break;
    case 's':
        Serial.print("Science");
        break;
    }
    Serial.println();
}

// Motor methods
void Motor::forward(uint8_t motor_speed)
{
    analogWrite(left_pwm, 0);
    analogWrite(right_pwm, motor_speed);
}

void Motor::backward(uint8_t motor_speed)
{
    analogWrite(left_pwm, motor_speed);
    analogWrite(right_pwm, 0);
}

void Motor::stop()
{
    analogWrite(left_pwm, 0);
    analogWrite(right_pwm, 0);
}

// Rover methods
void Rover::forward(uint8_t r_speed)
{
    FR_motor->forward(r_speed);
    FL_motor->forward(r_speed);
    BR_motor->forward(r_speed);
    BL_motor->forward(r_speed);
}

void Rover::backward(uint8_t r_speed)
{
    FR_motor->backward(r_speed);
    FL_motor->backward(r_speed);
    BR_motor->backward(r_speed);
    BL_motor->backward(r_speed);
}

void Rover::stop()
{
    FR_motor->stop();
    FL_motor->stop();
    BR_motor->stop();
    BL_motor->stop();
}

void Rover::set_max_rover_speed(uint8_t r_speed)
{
    max_rover_speed = r_speed;
}

void Rover::rc_mapped_data_update(int *arr)
{
    rc->update_data(ch); // update raw values

    for (int i = 0; i < 6; i++)
    {
        ch_mapped[i] = map(ch[i] - 1500, 0, 500, 0, max_rover_speed);
    }

    // for (int i = 0; i < 6; i++)
    // {
    //     Serial.print(ch_mapped[i]);
    //     Serial.print("  ");
    // }
    // Serial.println();
}

void Rover::show_rover_motor_speeds()
{
    this->rc_drive();
    Serial.print("Mode : ");
    switch (rc->mode)
    {
    case 'd':
        Serial.print("Drive");
        break;
    case 'A':
        Serial.print("Arm_1");
        break;
    case 'a':
        Serial.print("Arm_2");
        break;
    case 's':
        Serial.print("Science");
        break;
    }
    Serial.print("  ");
    Serial.print("Right motors : ");
    Serial.print(right_motor_speed);
    Serial.print("  ");
    Serial.print("Left motors : ");
    Serial.print(left_motor_speed);
    Serial.println("  ");
}

void Rover::rc_drive()
{

    this->rc_mapped_data_update(ch_mapped);
    rc->set_mode();
    // rc->show_data(); // show value
    if (rc->mode == 'd')
    {
        if (ch[3] < 1500 + 50 && ch[3] > 1500 - 50)
        {
            if (ch[1] > 1500)
            {
                if (ch[0] > 1500)
                {
                    right_motor_speed = constrain(ch_mapped[1] - ch_mapped[0], 0, max_rover_speed);
                    left_motor_speed = constrain(ch_mapped[1] + ch_mapped[0], 0, max_rover_speed);
                    FR_motor->forward(right_motor_speed);
                    BR_motor->forward(right_motor_speed);
                    FL_motor->forward(left_motor_speed);
                    BL_motor->forward(left_motor_speed);
                }

                else if (ch[0] < 1500)
                {
                    right_motor_speed = constrain(ch_mapped[1] - ch_mapped[0], 0, max_rover_speed);
                    left_motor_speed = constrain(ch_mapped[1] + ch_mapped[0], 0, max_rover_speed);
                    FR_motor->forward(right_motor_speed);
                    BR_motor->forward(right_motor_speed);
                    FL_motor->forward(left_motor_speed);
                    BL_motor->forward(left_motor_speed);
                }
            }
            else if (ch[1] < 1500)
            {
                if (ch[0] > 1500)
                {
                    right_motor_speed = constrain(-1 * ch_mapped[1] - ch_mapped[0], 0, max_rover_speed);
                    left_motor_speed = constrain(-1 * ch_mapped[1] + ch_mapped[0], 0, max_rover_speed);
                    FR_motor->backward(right_motor_speed);
                    BR_motor->backward(right_motor_speed);
                    FL_motor->backward(left_motor_speed);
                    BL_motor->backward(left_motor_speed);
                }

                else if (ch[0] < 1500)
                {
                    right_motor_speed = constrain(-1 * ch_mapped[1] - ch_mapped[0], 0, max_rover_speed);
                    left_motor_speed = constrain(-1 * ch_mapped[1] + ch_mapped[0], 0, max_rover_speed);
                    FR_motor->backward(right_motor_speed);
                    BR_motor->backward(right_motor_speed);
                    FL_motor->backward(left_motor_speed);
                    BL_motor->backward(left_motor_speed);
                }
            }

            else
            {
                this->stop();
            }
        }

        else
        {
            if (ch[3] > 1500)
            {
                right_motor_speed = constrain(ch_mapped[3], 0, max_rover_speed);
                left_motor_speed = constrain(ch_mapped[3], 0, max_rover_speed);
                FR_motor->backward(right_motor_speed);
                BR_motor->backward(right_motor_speed);
                FL_motor->forward(left_motor_speed);
                BL_motor->forward(left_motor_speed);
            }
            else if (ch[3] < 1500)
            {
                right_motor_speed = constrain(-1 * ch_mapped[3], 0, max_rover_speed);
                left_motor_speed = constrain(-1 * ch_mapped[3], 0, max_rover_speed);
                FR_motor->forward(right_motor_speed);
                BR_motor->forward(right_motor_speed);
                FL_motor->backward(left_motor_speed);
                BL_motor->backward(left_motor_speed);
            }
        }
    }

    else
    {
        FR_motor->stop();
        BR_motor->stop();
        FL_motor->stop();
        BL_motor->stop();
        right_motor_speed = 0;
        left_motor_speed = 0;
    }
}

// Relay Methods
void Relay::rotate_clockwise()
{
    digitalWrite(clk_pin, LOW);
    digitalWrite(anticlk_pin, HIGH);
}

void Relay::rotate_anticlockwise()
{
    digitalWrite(clk_pin, LOW);
    digitalWrite(anticlk_pin, HIGH);
}

void Relay::stop()
{
    digitalWrite(clk_pin, LOW);
    digitalWrite(anticlk_pin, HIGH);
}

// Arm methods

void Arm::rc_drive()
{
    rc->update_data(ch);
    rc->set_mode();
    if (rc->mode == 'A')
    {
        // Base motor control. Binded with channel four of the transmitter.
        if (ch[3] > 1500 + switch_thresh)
        {
            base->rotate_clockwise();
            actuator_state[0] = 1;
        }
        else if (ch[3] < 1500 - switch_thresh)
        {
            base->rotate_anticlockwise();
            actuator_state[0] = -1;
        }
        else
        {
            base->stop();
            actuator_state[0] = 0;
        }

        // Joint1 linear actuator control. Binded with channel one of the transmitter.
        if (ch[0] > 1500 + switch_thresh)
        {
            joint1->rotate_clockwise();
            actuator_state[1] = 1;
        }
        else if (ch[0] < 1500 - switch_thresh)
        {
            joint1->rotate_anticlockwise();
            actuator_state[1] = -1;
        }
        else
        {
            joint1->stop();
            actuator_state[1] = 0;
        }

        // Joint2 linear actuator control. Binded with channel two of the transmitter.
        if (ch[1] > 1500 + switch_thresh)
        {
            joint2->rotate_clockwise();
            actuator_state[2] = 1;
        }
        else if (ch[1] < 1500 - switch_thresh)
        {
            joint2->rotate_anticlockwise();
            actuator_state[2] = -1;
        }
        else
        {
            joint2->stop();
            actuator_state[2] = 0;
        }

        // Joint3 linear actuator control. Binded with channel three of the transmitter.
        if (ch[2] > 1500 + switch_thresh)
        {
            joint3->rotate_clockwise();
            actuator_state[3] = 1;
        }
        else if (ch[2] < 1500 - switch_thresh)
        {
            joint3->rotate_anticlockwise();
            actuator_state[3] = -1;
        }
        else
        {
            joint3->stop();
            actuator_state[3] = 0;
        }
    }
    else if (rc->mode == 'a')
    {
        // end_rotator linear actuator control. Binded with channel one of the transmitter.
        if (ch[0] > 1500 + switch_thresh)
        {
            end_rotator->rotate_clockwise();
            actuator_state[4] = 1;
        }
        else if (ch[0] < 1500 - switch_thresh)
        {
            end_rotator->rotate_anticlockwise();
            actuator_state[4] = -1;
        }
        else
        {
            end_rotator->stop();
            actuator_state[4] = 0;
        }

        // claw motor control. Binded with channel four of the transmitter.
        if (ch[3] > 1500 + switch_thresh)
        {
            claw->rotate_clockwise();
            actuator_state[5] = 1;
        }
        else if (ch[3] < 1500 - switch_thresh)
        {
            claw->rotate_anticlockwise();
            actuator_state[5] = -1;
        }
        else
        {
            claw->stop();
            actuator_state[5] = 0;
        }
    }

    else 
    {
        base->stop();
        joint1->stop();
        joint2->stop();
        joint3->stop();
        end_rotator->stop();
        claw->stop();
        for(int i = 0; i < 6; i++)
        {
            actuator_state[i] = 0;
        }
    }
}

void Arm::show_actuator_state()
{
    this->rc_drive();

    Serial.print("Mode : ");
    switch (rc->mode)
    {
    case 'd':
        Serial.print("Drive");
        break;
    case 'A':
        Serial.print("Arm_1");
        break;
    case 'a':
        Serial.print("Arm_2");
        break;
    case 's':
        Serial.print("Science");
        break;
    }
    Serial.print("  ");


    // Base
    Serial.print("Base : ");
    switch (actuator_state[0])
    {
    case -1:
        Serial.print("CLK");
        break;
    case 0:
        Serial.print("REST");
        break;
    case 1:
        Serial.print("A_CLK");
        break;
    }
    Serial.print("  ");
    // Joint 1
    Serial.print("Joint1 : ");
    switch (actuator_state[1])
    {
    case -1:
        Serial.print("CLK");
        break;
    case 0:
        Serial.print("REST");
        break;
    case 1:
        Serial.print("A_CLK");
        break;
    }
    Serial.print("  ");
    // Joint 2
    Serial.print("Joint2 : ");
    switch (actuator_state[2])
    {
    case -1:
        Serial.print("CLK");
        break;
    case 0:
        Serial.print("REST");
        break;
    case 1:
        Serial.print("A_CLK");
        break;
    }
    Serial.print("  ");
    // Joint 3
    Serial.print("Joint3 : ");
    switch (actuator_state[3])
    {
    case -1:
        Serial.print("CLK");
        break;
    case 0:
        Serial.print("REST");
        break;
    case 1:
        Serial.print("A_CLK");
        break;
    }
    Serial.print("  ");
    // End Rotator
    Serial.print("End rotator : ");
    switch (actuator_state[4])
    {
    case -1:
        Serial.print("CLK");
        break;
    case 0:
        Serial.print("REST");
        break;
    case 1:
        Serial.print("A_CLK");
        break;
    }
    Serial.print("  ");
    // Claw
    Serial.print("Claw : ");
    switch (actuator_state[5])
    {
    case -1:
        Serial.print("CLK");
        break;
    case 0:
        Serial.print("REST");
        break;
    case 1:
        Serial.print("A_CLK");
        break;
    }
    Serial.println("");
}
