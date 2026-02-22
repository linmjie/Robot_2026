#pragma region critical init stuff
#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;


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

#pragma endregion critical init stuff

// Pneumatic stuff
pneumatics pneumaticA = pneumatics(Brain.ThreeWirePort.A);
bool A_isOpen = false;

pneumatics pneumaticB = pneumatics(Brain.ThreeWirePort.B);
bool B_isOpen = false;

void toggle_A_PneumaticState() {
    A_isOpen = !A_isOpen;
    if (A_isOpen) pneumaticA.open();
    else pneumaticA.close();
}

void toggle_B_PneumaticState() {
    B_isOpen = !B_isOpen;
    if (B_isOpen) pneumaticB.open();
    else pneumaticB.close();
}

motor leftMotorA = motor(PORT2, ratio18_1, false);
motor leftMotorB = motor(PORT3, ratio18_1, false);
motor_group LeftDriveSmart = motor_group(leftMotorA, leftMotorB);
motor rightMotorA = motor(PORT1, ratio18_1, true);
motor rightMotorB = motor(PORT4, ratio18_1, true);
motor_group RightDriveSmart = motor_group(rightMotorA, rightMotorB);
drivetrain Drivetrain = drivetrain(LeftDriveSmart, RightDriveSmart, 319.19, 295, 40, mm, 1);

controller Controller1 = controller(primary);
motor Middle_Outake = motor(PORT8, ratio6_1, false);
motor Top_Outake = motor(PORT7, ratio6_1, false);

motor Intake_motor = motor(PORT6, ratio18_1, false);

// generating and setting random seed
void initializeRandomSeed() {
    int systemTime = Brain.Timer.systemHighResolution();
    double batteryCurrent = Brain.Battery.current();
    double batteryVoltage = Brain.Battery.voltage(voltageUnits::mV);

    // Combine these values into a single integer
    int seed = int(batteryVoltage + batteryCurrent * 100) + systemTime;

    // Set the seed
    srand(seed);
}

void vexcodeInit() {
    // Initializing random seed.
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
bool Controller1LeftShoulderControlMotorsStopped = true;
bool Controller1RightShoulderControlMotorsStopped = true;
bool middleIntakeControlMotorsStopped = true;

bool DrivetrainLNeedsToBeStopped_Controller1 = true;
bool DrivetrainRNeedsToBeStopped_Controller1 = true;

//ACTUAL CODE STUFF FOR CONTROLLER1
int rc_auto_loop_function_Controller1() {
    // process the controller input every 20 milliseconds
    // update the motors based on the input values
    while (RemoteControlCodeEnabled) {
        // calculate the drivetrain motor velocities from the controller joystick axies
        // left = Axis3 + Axis1
        // right = Axis3 - Axis1
        int drivetrainLeftSideSpeed = Controller1.Axis3.position() + Controller1.Axis1.position();
        int drivetrainRightSideSpeed = Controller1.Axis3.position() - Controller1.Axis1.position();

        // check if the value is inside of the deadband range
        if (drivetrainLeftSideSpeed < 5 && drivetrainLeftSideSpeed > -5) {
            // check if the left motor has already been stopped
            if (DrivetrainLNeedsToBeStopped_Controller1) {
                // stop the left drive motor
                LeftDriveSmart.stop();
                // tell the code that the left motor has been stopped
                DrivetrainLNeedsToBeStopped_Controller1 = false;
            }
        }
        else {
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
        }
        else {
            // reset the toggle so that the deadband code knows to stop the right motor next time the input is in the deadband range
            DrivetrainRNeedsToBeStopped_Controller1 = true;
        }

        // only tell the left drive motor to spin if the values are not in the deadband range
        if (DrivetrainLNeedsToBeStopped_Controller1) {
            LeftDriveSmart.setVelocity(drivetrainLeftSideSpeed, percent);
            LeftDriveSmart.spin(reverse);
        }
        // only tell the right drive motor to spin if the values are not in the deadband range
        if (DrivetrainRNeedsToBeStopped_Controller1) {
            RightDriveSmart.setVelocity(drivetrainRightSideSpeed, percent);
            RightDriveSmart.spin(reverse);
        }
        // check the ButtonL1/ButtonL2 status to control Arm_Motor
        if (Controller1.ButtonL1.pressing()) //FROM L1
        {
            Top_Outake.spin(forward);
            Controller1LeftShoulderControlMotorsStopped = false;
        }
        else if (Controller1.ButtonR1.pressing()) //FROM L2
        {
            Top_Outake.spin(reverse);
            Controller1LeftShoulderControlMotorsStopped = false;
        }
        else if (!Controller1LeftShoulderControlMotorsStopped)
        {
            Top_Outake.stop();
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

        //Basically copied the previous section with the other motor
        if (Controller1.ButtonA.pressing()) //FROM A
        {
            Middle_Outake.spin(forward);
            middleIntakeControlMotorsStopped = false;
        }
        else if (Controller1.ButtonB.pressing()) //FROM B
        {
            Middle_Outake.spin(reverse);
            middleIntakeControlMotorsStopped = false;
        }
        else if (!middleIntakeControlMotorsStopped) {
            Middle_Outake.stop();
            // set the toggle so that we don't constantly tell the motor to stop when the buttons are released
            middleIntakeControlMotorsStopped = true;
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

void autonomous() {
    Drivetrain.setDriveVelocity(90, percent);

    Drivetrain.driveFor(reverse, 6, inches, true);
    Drivetrain.turnFor(left, 90, degrees);
    wait(1, seconds);
    Drivetrain.driveFor(reverse, 6, inches, true);
    wait(1, seconds);
    Drivetrain.turnFor(right, 85, degrees);
    wait(1, seconds);
    Intake_motor.spin(forward);
    Drivetrain.driveFor(reverse, 36, inches, true);
    wait(1, seconds);
    
    Drivetrain.setDriveVelocity(65, percent);
    Drivetrain.driveFor(reverse, 26, inches);
    wait(2, seconds);

    //Reset
    Intake_motor.stop();
    Drivetrain.stop(brake);
    Drivetrain.setDriveVelocity(100, percent);
}

int main() {
    // Initializing Robot Configuration. DO NOT REMOVE!
    vexcodeInit();
    // Begin project code
    competition comp;

    //Start the pneumatic open because mason keeps outaking into the air...
    toggle_B_PneumaticState();

    //Unlike the motors which activate on held controller input, pneumatic is toggleable
    //Absolute close/open on pneumatics may be removed later because they're not used often

    Controller1.ButtonUp.pressed(toggle_A_PneumaticState);

    Controller1.ButtonLeft.pressed([]{ pneumaticA.close(); });
    Controller1.ButtonRight.pressed([]{ pneumaticA.open(); });

    Controller1.ButtonDown.pressed(toggle_B_PneumaticState);

    Controller1.ButtonY.pressed([]{ pneumaticB.close(); });
    Controller1.ButtonX.pressed([]{ pneumaticB.open(); });

    Top_Outake.setVelocity(90, percent);
    Middle_Outake.setVelocity(90, percent);
    Drivetrain.setDriveVelocity(90, percent);
    Intake_motor.setVelocity(90, percent);

    comp.autonomous(autonomous);
}