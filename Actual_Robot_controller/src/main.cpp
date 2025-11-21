#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>


#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;


// START V5 MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END V5 MACROS


// Robot configuration code.
motor leftMotorA = motor(PORT1, ratio18_1, false);
motor leftMotorB = motor(PORT2, ratio18_1, false);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);
motor rightMotorA = motor(PORT3, ratio18_1, true);
motor rightMotorB = motor(PORT4, ratio18_1, true);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);
drivetrain Drivetrain = drivetrain(LeftDriveSmart, RightDriveSmart, 319.19, 295, 40, mm, 1);

motor Chain_Motor_7 = motor(PORT7, ratio18_1, false);

motor Intake_Motor_5 = motor(PORT5, ratio18_1, false);

motor Arm_Motor_6 = motor(PORT6, ratio18_1, false);

controller Controller1 = controller(primary);
controller Controller2 = controller(partner);

const double driveTrainSpeedFactor = 1.5;
const double intakeSpeedFactor = 1.5;
const double chainSpeedFactor = 3.0;

      
Drivetrain.setDriveVelocity(100*driveTrainSpeedFactor, percent);
Intake_Motor_5.setVelocity(100*intakeSpeedFactor, percent);
Chain_Motor_7.setVelocity(100*chainSpeedFactor, percent);

// generating and setting random seed
void initializeRandomSeed(){
  int systemTime = Brain.Timer.systemHighResolution();
  double batteryCurrent = Brain.Battery.current();
  double batteryVoltage = Brain.Battery.voltage(voltageUnits::mV);

  // Combine these values into a single integer
  int seed = int(batteryVoltage + batteryCurrent * 100) + systemTime;

  // Set the seed
  srand(seed);
}



void vexcodeInit() {

  //Initializing random seed.
  initializeRandomSeed(); 
}


// Helper to make playing sounds from the V5 in VEXcode easier and
// keeps the code cleaner by making it clear what is happening.
void playVexcodeSound(const char *soundName) {
  printf("VEXPlaySound:%s\n", soundName);
  wait(5, msec);
}



// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;
// define variables used for controlling motors based on controller inputs
bool DrivetrainLNeedsToBeStopped_Controller1 = true;
bool DrivetrainRNeedsToBeStopped_Controller1 = true;

// define a task that will handle monitoring inputs from Controller1
int rc_auto_loop_function_Controller1() {
  // process the controller input every 20 milliseconds
  // update the motors based on the input values
  while(true) {
    if(RemoteControlCodeEnabled) {
      
      // calculate the drivetrain motor velocities from the controller joystick axies
      // left = Axis3 + Axis1
      // right = Axis3 - Axis1
      int drivetrainLeftSideSpeed = Controller1.Axis3.position() + Controller1.Axis1.position();
      int drivetrainRightSideSpeed = Controller1.Axis3.position() - Controller1.Axis1.position();

      const double driveTrainSpeedFactor = 1.5;
      const double intakeSpeedFactor = 1.5;
      const double chainSpeedFactor = 3.0;

      
      Drivetrain.setDriveVelocity(100*driveTrainSpeedFactor, percent);
      Intake_Motor_5.setVelocity(100*intakeSpeedFactor, percent);
      Chain_Motor_7.setVelocity(100*chainSpeedFactor, percent);


      // check if the value is inside of the deadband range
      if (drivetrainLeftSideSpeed < 5 && drivetrainLeftSideSpeed > -5) {
        // check if the left motor has already been stopped
        if (DrivetrainLNeedsToBeStopped_Controller1) {
          // stop the left drive motor
          LeftDriveSmart.stop();
          // tell the code that the left motor has been stopped
          DrivetrainLNeedsToBeStopped_Controller1 = false;
        }
      } else {
        // reset the toggle so that the deadband code knows to stop the left motor nexttime the input is in the deadband range
        DrivetrainLNeedsToBeStopped_Controller1 = true;
      }
      // check if the value is inside of the deadband range
      if (drivetrainRightSideSpeed < 5 && drivetrainRightSideSpeed > -5) {
        // check if the right motor has already been stopped
        if (DrivetrainRNeedsToBeStopped_Controller1) {
          // stop the right drive motor
          RightDriveSmart.stop();
          // tell the code that the right motor has been stopped
          DrivetrainRNeedsToBeStopped_Controller1 = false;
        }
      } else {
        // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
        DrivetrainRNeedsToBeStopped_Controller1 = true;
      }
      
      // only tell the left drive motor to spin if the values are not in the deadband range
      if (DrivetrainLNeedsToBeStopped_Controller1) {
        LeftDriveSmart.setVelocity(drivetrainLeftSideSpeed, percent);
        LeftDriveSmart.spin(forward);
      }
      // only tell the right drive motor to spin if the values are not in the deadband range
      if (DrivetrainRNeedsToBeStopped_Controller1) {
        RightDriveSmart.setVelocity(drivetrainRightSideSpeed, percent);
        RightDriveSmart.spin(forward);
      }
    }
    // wait before repeating the process
    wait(20, msec);
  }
  return 0;
}

// define variables used for controlling motors based on controller inputs
bool Controller2LeftShoulderControlMotorsStopped = true;
bool Controller2RightShoulderControlMotorsStopped = true;
bool Controller2UpDownButtonsControlMotorsStopped = true;

// define a task that will handle monitoring inputs from Controller2
int rc_auto_loop_function_Controller2() {
  // process the controller input every 20 milliseconds
  // update the motors based on the input values
  while(true) {
    if(RemoteControlCodeEnabled) {
      // check the ButtonL1/ButtonL2 status to control Chain_Motor_7
      if (Controller2.ButtonL1.pressing()) {
        Chain_Motor_7.spin(forward);
        Controller2LeftShoulderControlMotorsStopped = false;
      } else if (Controller2.ButtonL2.pressing()) {
        Chain_Motor_7.spin(reverse);
        Controller2LeftShoulderControlMotorsStopped = false;
      } else if (!Controller2LeftShoulderControlMotorsStopped) {
        Chain_Motor_7.stop();
        // set the toggle so that we don't constantly tell the motor to stop when the buttons are released
        Controller2LeftShoulderControlMotorsStopped = true;
      }
      // check the ButtonR1/ButtonR2 status to control Intake_Motor_5
      if (Controller2.ButtonR1.pressing()) {
        Intake_Motor_5.spin(forward);
        Controller2RightShoulderControlMotorsStopped = false;
      } else if (Controller2.ButtonR2.pressing()) {
        Intake_Motor_5.spin(reverse);
        Controller2RightShoulderControlMotorsStopped = false;
      } else if (!Controller2RightShoulderControlMotorsStopped) {
        Intake_Motor_5.stop();
        // set the toggle so that we don't constantly tell the motor to stop when the buttons are released
        Controller2RightShoulderControlMotorsStopped = true;
      }
      // check the ButtonUp/ButtonDown status to control Arm_Motor_6
      if (Controller2.ButtonUp.pressing()) {
        Arm_Motor_6.spin(forward);
        Controller2UpDownButtonsControlMotorsStopped = false;
      } else if (Controller2.ButtonDown.pressing()) {
        Arm_Motor_6.spin(reverse);
        Controller2UpDownButtonsControlMotorsStopped = false;
      } else if (!Controller2UpDownButtonsControlMotorsStopped) {
        Arm_Motor_6.stop();
        // set the toggle so that we don't constantly tell the motor to stop when the buttons are released
        Controller2UpDownButtonsControlMotorsStopped = true;
      }
    }
    // wait before repeating the process
    wait(20, msec);
  }
  return 0;
}

task rc_auto_loop_task_Controller1(rc_auto_loop_function_Controller1);
task rc_auto_loop_task_Controller2(rc_auto_loop_function_Controller2);

#pragma endregion VEXcode Generated Robot Configuration

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       {author}                                                  */
/*    Created:      {date}                                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/

// Include the V5 Library
#include "vex.h"
  
// Allows for easier use of the VEX Library
using namespace vex;

int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  // Begin project code
  
}
