#region VEXcode Generated Robot Configuration
from vex import *
import urandom
import math

# Brain should be defined by default
brain=Brain()

# Robot configuration code
left_drive_smart = Motor(Ports.PORT1, GearSetting.RATIO_18_1, False)
right_drive_smart = Motor(Ports.PORT2, GearSetting.RATIO_18_1, True)
drivetrain = DriveTrain(left_drive_smart, right_drive_smart, 319.19, 295, 40, MM, 1)
motor_group_3_motor_a = Motor(Ports.PORT3, GearSetting.RATIO_18_1, True)
motor_group_3_motor_b = Motor(Ports.PORT4, GearSetting.RATIO_18_1, False)
motor_group_3 = MotorGroup(motor_group_3_motor_a, motor_group_3_motor_b)


# wait for rotation sensor to fully initialize
wait(30, MSEC)


# Make random actually random
def initializeRandomSeed():
    wait(100, MSEC)
    random = brain.battery.voltage(MV) + brain.battery.current(CurrentUnits.AMP) * 100 + brain.timer.system_high_res()
    urandom.seed(int(random))
      
# Set random seed 
initializeRandomSeed()


def play_vexcode_sound(sound_name):
    # Helper to make playing sounds from the V5 in VEXcode easier and
    # keeps the code cleaner by making it clear what is happening.
    print("VEXPlaySound:" + sound_name)
    wait(5, MSEC)

# add a small delay to make sure we don't print in the middle of the REPL header
wait(200, MSEC)
# clear the console to make sure we don't have the REPL in the console
print("\033[2J")

#endregion VEXcode Generated Robot Configuration

 

# ------------------------------------------

#

#   Project:

#   Author:

#   Created:

#   Configuration:

#

# ------------------------------------------

 

# Library imports

from vex import *

 

# Begin project code

 

def pre_autonomous():


    # actions to do when the program starts

    brain.screen.clear_screen()

    brain.screen.print("pre auton code")

    wait(1, SECONDS)

 

def autonomous():


    

    # place automonous code here

    drivetrain.set_drive_velocity(100,PERCENT)

    drivetrain.set_turn_velocity(100,PERCENT)

    drivetrain.drive_for(REVERSE, 6, INCHES)

    

    drivetrain.turn_for(RIGHT, 90, DEGREES)

    drivetrain.drive_for(REVERSE,9, INCHES)

    drivetrain.turn_for(RIGHT, 90, DEGREES)

    drivetrain.drive_for(REVERSE,6, INCHES)




    motor_group_3.spin(REVERSE)
   

    

   

 

def user_control():

    brain.screen.clear_screen()

    # place driver control in this while loop

    while True:

        wait(20, MSEC)

 

# create competition instance

comp = Competition(user_control, autonomous)

pre_autonomous()

 

autonomous()