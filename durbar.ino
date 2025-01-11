#include"durbar.h"

RemoteController * rc = new RemoteController(22, 24, 26, 28, 30, 32);

Motor * FL_m = new Motor(2, 3);
Motor * FR_m = new Motor(4, 5);
Motor * BL_m = new Motor(6, 7);
Motor * BR_m = new Motor(8, 9);

Rover rover(FL_m, FR_m, BL_m, BR_m, rc);


Relay * base = new Relay(23, 25);
Relay * joint1 = new Relay(27, 29);
Relay * joint2 = new Relay(31, 33);
Relay * joint3 = new Relay(35, 37);
Relay * end_rotator = new Relay(39, 41);
Relay * claw = new Relay(43, 45);

Arm arm(base, joint1, joint2, joint3, end_rotator, claw, rc);



void setup()
{
  Serial.begin(9600);
  rover.set_max_rover_speed(200);
}

void loop()
{
  arm.rc_drive();
  rover.rc_drive();
  arm.show_actuator_state();
  rover.show_rover_motor_speeds();
}




