#include "logger.h"

using namespace vex;

// Brain should be defined by default
brain Brain;

//GLOBAL VARS
auto LOGGER = Logger::create(&Brain.Screen)
    .disableWordWrap()
    .build();
const double SMALL_INCREASE = 0.05;
//idk if these are the correct left and right
double LEFT_MOTOR_RATIO = 1.0;
double RIGHT_MOTOR_RATIO = 1.0;

#pragma region VEXcode Generated Robot Configuration
// START V5 MACROS
#define waitUntil(condition) \
    do                       \
    {                        \
        wait(5, msec);       \
    } while (!(condition))

#define repeat(iterations) \
    for (int iterator = 0; iterator < iterations; iterator++)
// END V5 MACROS

// Pneumatic stuff
pneumatics pneumatic = pneumatics(Brain.ThreeWirePort.A);
bool isOpen = false;

void togglePneumaticState() {
    isOpen = !isOpen;
    if (isOpen) pneumatic.open();
    else pneumatic.close();
}

// Robot configuration code.
motor leftMotorA = motor(PORT1, ratio18_1, false);
motor leftMotorB = motor(PORT2, ratio18_1, false);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);
motor rightMotorA = motor(PORT3, ratio18_1, true);
motor rightMotorB = motor(PORT4, ratio18_1, true);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);
drivetrain Drivetrain = drivetrain(LeftDriveSmart, RightDriveSmart, 319.19, 295, 40, mm, 1);

controller Controller1 = controller(primary);
motor Arm_Motor = motor(PORT3, ratio18_1, false);

motor Intake_motor = motor(PORT4, ratio18_1, false);

// generating and setting random seed
void initializeRandomSeed()
{
    int systemTime = Brain.Timer.systemHighResolution();
    double batteryCurrent = Brain.Battery.current();
    double batteryVoltage = Brain.Battery.voltage(voltageUnits::mV);

    // Combine these values into a single integer
    int seed = int(batteryVoltage + batteryCurrent * 100) + systemTime;

    // Set the seed
    srand(seed);
}

void vexcodeInit()
{

    // Initializing random seed.
    initializeRandomSeed();
}

// Helper to make playing sounds from the V5 in VEXcode easier and
// keeps the code cleaner by making it clear what is happening.
void playVexcodeSound(const char *soundName)
{
    printf("VEXPlaySound:%s\n", soundName);
    wait(5, msec);
}

// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;
// define variables used for controlling motors based on controller inputs
bool Controller1LeftShoulderControlMotorsStopped = true;
bool Controller1RightShoulderControlMotorsStopped = true;
bool DrivetrainLNeedsToBeStopped_Controller1 = true;
bool DrivetrainRNeedsToBeStopped_Controller1 = true;

//ACTUAL CODE STUFF FOR CONTROLLER1
int rc_auto_loop_function_Controller1()
{
    // process the controller input every 20 milliseconds
    // update the motors based on the input values
    while (true)
    {
        if (RemoteControlCodeEnabled)
        {

            // calculate the drivetrain motor velocities from the controller joystick axies
            // left = Axis3 + Axis1
            // right = Axis3 - Axis1
            int drivetrainLeftSideSpeed = Controller1.Axis3.position() + Controller1.Axis1.position();
            int drivetrainRightSideSpeed = Controller1.Axis3.position() - Controller1.Axis1.position();

            //These two are just for multipliers to test speeds
            drivetrainLeftSideSpeed *= LEFT_MOTOR_RATIO;
            drivetrainRightSideSpeed *= RIGHT_MOTOR_RATIO;

            // check if the value is inside of the deadband range
            if (drivetrainLeftSideSpeed < 5 && drivetrainLeftSideSpeed > -5)
            {
                // check if the left motor has already been stopped
                if (DrivetrainLNeedsToBeStopped_Controller1)
                {
                    // stop the left drive motor
                    LeftDriveSmart.stop();
                    // tell the code that the left motor has been stopped
                    DrivetrainLNeedsToBeStopped_Controller1 = false;
                }
            }
            else
            {
                // reset the toggle so that the deadband code knows to stop the left motor nexttime the input is in the deadband range
                DrivetrainLNeedsToBeStopped_Controller1 = true;
            }
            // check if the value is inside of the deadband range
            if (drivetrainRightSideSpeed < 5 && drivetrainRightSideSpeed > -5)
            {
                // check if the right motor has already been stopped
                if (DrivetrainRNeedsToBeStopped_Controller1)
                {
                    // stop the right drive motor
                    RightDriveSmart.stop();
                    // tell the code that the right motor has been stopped
                    DrivetrainRNeedsToBeStopped_Controller1 = false;
                }
            }
            else
            {
                // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
                DrivetrainRNeedsToBeStopped_Controller1 = true;
            }

            // only tell the left drive motor to spin if the values are not in the deadband range
            if (DrivetrainLNeedsToBeStopped_Controller1)
            {
                LeftDriveSmart.setVelocity(drivetrainLeftSideSpeed, percent);
                LeftDriveSmart.spin(reverse);
            }
            // only tell the right drive motor to spin if the values are not in the deadband range
            if (DrivetrainRNeedsToBeStopped_Controller1)
            {
                RightDriveSmart.setVelocity(drivetrainRightSideSpeed, percent);
                RightDriveSmart.spin(reverse);
            }
            // check the ButtonL1/ButtonL2 status to control Arm_Motor
            if (Controller1.ButtonL1.pressing()) //FROM L1
            {
                Arm_Motor.spin(forward);
                Controller1LeftShoulderControlMotorsStopped = false;
            }
            else if (Controller1.ButtonR1.pressing()) //FROM L2
            {
                Arm_Motor.spin(reverse);
                Controller1LeftShoulderControlMotorsStopped = false;
            }
            else if (!Controller1LeftShoulderControlMotorsStopped)
            {
                Arm_Motor.stop();
                // set the toggle so that we don't constantly tell the motor to stop when the buttons are released
                Controller1LeftShoulderControlMotorsStopped = true;
            }
            // check the ButtonR1/ButtonR2 status to control Intake_motor
            if (Controller1.ButtonR2.pressing()) //FROM R1
            {
                Intake_motor.spin(forward);
                Controller1RightShoulderControlMotorsStopped = false;
            }
            else if (Controller1.ButtonL2.pressing()) //FROM R2
            {
                Intake_motor.spin(reverse);
                Controller1RightShoulderControlMotorsStopped = false;
            }
            else if (!Controller1RightShoulderControlMotorsStopped)
            {
                Intake_motor.stop();
                // set the toggle so that we don't constantly tell the motor to stop when the buttons are released
                Controller1RightShoulderControlMotorsStopped = true;
            }
        }
        // wait before repeating the process
        wait(20, msec);
    }
    return 0;
}

//(assuming) async code for controller1 function that runs in background
task rc_auto_loop_task_Controller1(rc_auto_loop_function_Controller1);

#pragma endregion VEXcode Generated Robot Configuration

/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       {author}                                                  */
/*    Created:      {date}                                                    */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#pragma region motor speed adjustment

void clampAndLog() {
  if (LEFT_MOTOR_RATIO > 1.0) {
    LEFT_MOTOR_RATIO = 1.0;
  } else if (LEFT_MOTOR_RATIO < 0) {
    LEFT_MOTOR_RATIO = 0.01;
  }

  if (RIGHT_MOTOR_RATIO > 1.0) {
    RIGHT_MOTOR_RATIO = 1.0;
  } else if (RIGHT_MOTOR_RATIO < 0) {
    RIGHT_MOTOR_RATIO = 0.01;
  }
  LOGGER.println("Left Motor Ratio: %.2f", LEFT_MOTOR_RATIO);
  LOGGER.println("Right Motor Ratio: %.2f", RIGHT_MOTOR_RATIO);
  LOGGER.println("=======================");
}

void decreaseLeft() {
  LEFT_MOTOR_RATIO -= SMALL_INCREASE;
  clampAndLog();
}
void increaseLeft() {
  LEFT_MOTOR_RATIO += SMALL_INCREASE;
  clampAndLog();
}
void decreaseRight() {
  RIGHT_MOTOR_RATIO -= SMALL_INCREASE;
  clampAndLog();
}
void increaseRight() {
  RIGHT_MOTOR_RATIO += SMALL_INCREASE;
  clampAndLog();
}
#pragma endregion motor speed adjustment

int main() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();
    // Begin project code
    Controller1.ButtonA.pressed(increaseLeft);
    Controller1.ButtonY.pressed(decreaseLeft);

    Controller1.ButtonX.pressed(increaseRight);
    Controller1.ButtonB.pressed(decreaseRight);

    Controller1.ButtonUp.pressed(togglePneumaticState);

    Controller1.ButtonDown.pressed([](){
        LOGGER.println("pneumatic value thing: %d", pneumatic.value());
    });

    Arm_Motor.setVelocity(90, percent);
    Drivetrain.setDriveVelocity(90, percent);
    Intake_motor.setVelocity(90, percent);
}