#include "lemlib/api.hpp" // IWYU pragma: keep
#include "main.h"

pros::MotorGroup left_motors({-1, 2}, pros::MotorGearset::green); // left motors use 600 RPM cartridges
pros::MotorGroup right_motors({4, -5}, pros::MotorGearset::green); // right motors use 600 RPM cartridges

// drivetrain settings
lemlib::Drivetrain drivetrain(&left_motors, // left motor group
							  &right_motors, // right motor group
                              18, // 18 inch track width
                              lemlib::Omniwheel::NEW_4, // using new 4" omnis
                              600, // drivetrain rpm is 600
                              2 // horizontal drift is 2 (for now)
);

// create an imu on port 10
pros::Imu imu(10);

// odometry sensors: no tracking wheels, IMU only
lemlib::OdomSensors sensors(
    nullptr, // vertical tracking wheel 1
    nullptr, // vertical tracking wheel 2
    nullptr, // horizontal tracking wheel 1
    nullptr, // horizontal tracking wheel 2
    &imu     // inertial sensor
);

// lateral PID controller
lemlib::ControllerSettings lateral_controller(10, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              3, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in inches
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in inches
                                              500, // large error range timeout, in milliseconds
                                              20 // maximum acceleration (slew)
);

// angular PID controller
lemlib::ControllerSettings angular_controller(2, // proportional gain (kP)
                                              0, // integral gain (kI)
                                              10, // derivative gain (kD)
                                              3, // anti windup
                                              1, // small error range, in degrees
                                              100, // small error range timeout, in milliseconds
                                              3, // large error range, in degrees
                                              500, // large error range timeout, in milliseconds
                                              0 // maximum acceleration (slew)
);


// input curve for throttle input during driver control
lemlib::ExpoDriveCurve throttle_curve(3, // joystick deadband out of 127
                                     10, // minimum output where drivetrain will move out of 127
                                     1.019 // expo curve gain
);

// input curve for steer input during driver control
lemlib::ExpoDriveCurve steer_curve(3, // joystick deadband out of 127
                                  10, // minimum output where drivetrain will move out of 127
                                  1.019 // expo curve gain
);

// create the chassis
lemlib::Chassis chassis(drivetrain,
                        lateral_controller,
                        angular_controller,
                        sensors,
                        &throttle_curve, 
                        &steer_curve
);


pros::Controller controller(pros::E_CONTROLLER_MASTER);

void on_center_button() {
}


void disabled() {}


void competition_initialize() {}


void autonomous() {}


void opcontrol() {
    // loop forever
    while (true) {
        // get left y and right x positions
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        // move the robot
        chassis.curvature(leftY, rightX);

        // delay to save resources
        pros::delay(25);
    }
}