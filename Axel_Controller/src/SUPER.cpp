#pragma region LOGGER_AND_STUFF

#include "vex.h"
#include <vector>
#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

using namespace vex;
//Templates are miserable, so variadic functions(variable parameter count) are implemented with va_list from c

class Logger {
    public:
        static const int SCREEN_WIDTH = 480;
        static const int SCREEN_HEIGHT = 240;
    private:
        std::vector<std::string> buffer;

        //Properties
        brain::lcd* screen;
        uint32_t maxLineSize;
        bool doWordWrap;
        bool logExternally;
        std::string logFile;


    public:
        //Prefer construction via builder
        Logger(brain::lcd* screen, uint32_t maxLineSize,
            bool logExternally, std::string logFile);

        void clearScreen();

        //Some function declarations COPIED from vex_brain.h

        /** 
         * @brief Prints a number, string, or Boolean with a new line.
         * Provides formatting from printf
         * @param format This is a reference to a char format that prints the value of variables
         * @param ... A variable list of parameters to insert into format string
         * @note Some characters like \t and \\n don't work as they would normally
        */          
        void println(const char *format, ... );

        /** 
         * @brief Prints a number, string, or Boolean.
         * Provides formatting from printf
         * @param format This is a reference to a char format that prints the value of variables
         * @param ... A variable list of parameters to insert into format string
         * @note Some characters like \t and \\n don't work as they would normally
        */          
        void print(const char *format, ... );

        //These are just redefinitions stolen from vex_brain.h for simpler printing

        /** 
         * @brief Prints a number, string, or Boolean.
         * Provides formatting from printf
         * @param value Information to display on the screen
         * @note Some characters like \t and \\n don't work as they would normally
        */  
        template <class T>
        void     print( T value ) {
          if( std::is_same< T, int >::value )
            print( "%d", (int)value );
          else
          if( std::is_same< T, double >::value )
            print( "%.2f", (double)value );
          else {
            // primarily to handle modkit number
            if( (int)value == value )
              print( "%d", (int)value );
            else
              print( "%.2f", (double)value );
          }
        }

        /** 
         * @brief Prints a number, string, or Boolean.
         * Provides formatting from printf
         * @param value Information to display on the screen
         * @note Some characters like \t and \\n don't work as they would normally
        */  
        template <class T>
        void println( T value ) {
          if( std::is_same< T, int >::value )
            println( "%d", (int)value );
          else
          if( std::is_same< T, double >::value )
            println( "%.2f", (double)value );
          else {
            // primarily to handle modkit number (what's a modkit number??)
            if( (int)value == value )
              println( "%d", (int)value );
            else
              println( "%.2f", (double)value );
          }
        }

    private: 
        void _printBuffer();
        void _addToBuffer(bool makeNewLine, const char* format, va_list args);
        std::stack<std::string> _fillPrintStack();

        //CONSTRUCTION
        class Builder {
            brain::lcd* screen;
            uint32_t maxLineSize = 200;
            bool doWordWrap = true;
            bool logExternally = false;
            std::string logFile = "";

            public:
                Builder(brain::lcd *screen);

                /**
                 * @brief Any string printed will be cut off at this character limit
                 */
                Builder& setMaxLineSize(uint32_t bytes);

                /**
                 * @brief By default the logger wraps words onto a new line this method disables that
                 */
                Builder& disableWordWrap();

                /**
                 * @brief Enables printing to computer's standard output
                 */
                Builder& printToStdout();

                /**
                 * @brief Any output from the logger can also be logged into a file
                 * @param fileName The file path as a string.
                 */
                Builder& logToFile(std::string fileName);


                /**
                 * @brief Finishes the building process
                 * @return The completed customized logger
                 */
                Logger build();
        };

    public:
        /**
         * @brief Creates a new builder with some default values with the ability
         *  tack on methods from the builder class to customize
         * @param screen This is a pointer to the brain's screen the logger will output to.
         * Usually you would pass in @code &Brain.Screen @endcode
         */
        static Builder create(brain::lcd* screen);
};

void Logger::clearScreen() {
    this->screen->clearScreen();
    this->screen->setCursor(0, 0);
}

void Logger::_addToBuffer(bool makeNewLine, const char* format, va_list args) {
    uint32_t lineSize = this->maxLineSize;
    char buf[lineSize];

    va_list argsCopy;
    va_copy(argsCopy, args);
    //vsnprintf just takes the output of printf(format, argsCopy), or the formatted string of what we want to log,
    //and puts it into buf as long as long as it doesn't exceed the byte limit of the second arg, lineSize
    vsnprintf(buf, lineSize, format, argsCopy);

    if (makeNewLine) {
        this->buffer.push_back(std::string(buf));
    } else {
        this->buffer.back().append(buf);
    }

    if (this->logExternally) {
        std::cout << buf;
        if (makeNewLine) {
            std::cout << '\n';
        }
    }

    if (!this->logFile.empty()) {
        std::ofstream outputFile;
        outputFile.open(this->logFile);
        if (!outputFile.is_open()) {
            std::cerr << "[ERROR] Unable to open file: " << this->logFile << std::endl;
        } else {
            outputFile << buf;
            if (makeNewLine) {
                outputFile << std::endl;
            }
        }
        outputFile.close();
    }
}

void Logger::print(const char* format, ...) { 
    va_list args;
    va_start(args, format);
    this->_addToBuffer(false, format, args);
    this->_printBuffer();
    va_end(args);
}

void Logger::println(const char* format, ...) {
    this->screen->clearScreen();
    va_list args;
    va_start(args, format);
    this->_addToBuffer(true, format, args);
    this->_printBuffer();
    va_end(args);
}

void Logger::_printBuffer() { 
    //Initial state
    this->screen->clearScreen();
    //Ensures cursor is reset
    this->screen->setCursor(1, 1);

    /* this->buffer contains ALL outputs, toPrint is designed to contain only the ones that can fit on the screen.
     * Why toPrint is a stack is because we will can identify lines starting from end of this->buffer vector,
     * check if adding the line would exceed the maximum height of our screen, 
     * then push it onto our stack if it doesn't, ending the process of adding to toPrint if it does exceed.
     * The last thing we push onto the stack is the furthest our buffer goes back in history, aka where we start printing
    */
    std::stack<std::string> toPrint = this->_fillPrintStack();

    while (!toPrint.empty()) {
        const char* contents = toPrint.top().c_str();
        this->screen->print(contents);
        this->screen->newLine();
        toPrint.pop();
    }
}

std::stack<std::string> Logger::_fillPrintStack() {
    std::stack<std::string> printStack;
    int heightLeft = SCREEN_HEIGHT;
    int size = this->buffer.size();
    for (int i = 0; i < size; i++) {
        int index = size - i - 1;
        assert(i >= 0 && i < this->buffer.size());
        std::string lineContainer = this->buffer.at(index);
        const char* contents = lineContainer.c_str();
        int width = this->screen->getStringWidth(contents);
        if (!this->doWordWrap || width <= SCREEN_WIDTH) {
            int height = this->screen->getStringHeight(contents);
            if (height < heightLeft) {
                printStack.push(lineContainer);
                heightLeft -= height;
            } else {
                break; //end early, no need to go back through rest of buffer
            }
        } else {
            std::vector<std::string> splitStrings;
            int strLen = lineContainer.length();
            int avgCharSize = width / strLen;
            //Guess split
            int guessStrLen = SCREEN_WIDTH / avgCharSize;
            int frontPointer = 0;
            int backPointer = frontPointer + guessStrLen;
            do {
                std::string sub = lineContainer.substr(frontPointer, guessStrLen);
                //Fine tune
                while (backPointer < strLen) {
                    int subStrWidth = this->screen->getStringWidth(sub.c_str());
                    if (subStrWidth > SCREEN_WIDTH) {
                        sub.pop_back();
                        backPointer--;
                    } else if (subStrWidth < SCREEN_WIDTH) {
                        int nextCharWidth = this->screen->getStringWidth(
                                lineContainer.substr(backPointer, 1).c_str());
                        if (subStrWidth + nextCharWidth <= SCREEN_WIDTH) {
                            backPointer++;
                            sub.push_back(lineContainer.at(backPointer));
                        } else {
                            break;
                        }
                    } else { //subStrWidth == SCREEN_WIDTH
                        break;
                    }
                }
                int height = this->screen->getStringHeight(sub.c_str());
                if (height < heightLeft) {
                    printStack.push(sub);
                    heightLeft -= height;
                } else {
                    break; //end early, no need to go back through rest of buffer
                }
                frontPointer = backPointer + 1;
            } while (backPointer <= strLen);
        }
    }
    return printStack;
}

//CONSTRUCTION

Logger Logger::Builder::build() {
    return Logger(screen, maxLineSize, logExternally, logFile);
}

Logger::Builder& Logger::Builder::setMaxLineSize(uint32_t bytes) {
    this->maxLineSize = bytes;
    return *this;
}

Logger::Builder& Logger::Builder::disableWordWrap() {
    this->doWordWrap = false;
    return *this;
}

Logger::Builder& Logger::Builder::printToStdout() {
    this->logExternally = true;
    return *this;
}

Logger::Builder& Logger::Builder::logToFile(std::string logFile) {
    this->logFile = logFile;
    return *this;
}

Logger::Builder Logger::create(brain::lcd* screen) {
    return Logger::Builder(screen);
}

Logger::Logger(brain::lcd* screen, uint32_t maxLineSize,
    bool logExternally, std::string logFile)
{
    this->screen = screen;
    this->maxLineSize = maxLineSize;
    this->logExternally = logExternally;
    this->logFile = logFile;
}

Logger::Builder::Builder(brain::lcd* screen) {
    this->screen = screen;
}
#pragma endregion LOGGER_AND_STUFF

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

// Robot configuration code.
motor LeftDriveSmart = motor(PORT1, ratio18_1, false);
motor RightDriveSmart = motor(PORT2, ratio18_1, true);
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

    Arm_Motor.setVelocity(90, percent);
    Drivetrain.setDriveVelocity(90, percent);
    Intake_motor.setVelocity(90, percent);
}