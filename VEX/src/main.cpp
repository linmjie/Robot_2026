#pragma region VEXcode Generated Robot Configuration
// Make sure all required headers are included.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>


#include "vex.h"

using namespace vex;

// Brain should be defined by default
brain Brain;
controller controller1;


// START V5 MACROS
#define waitUntil(condition)                                                   \
  do {                                                                         \
    wait(5, msec);                                                             \
  } while (!(condition))

#define repeat(iterations)                                                     \
  for (int iterator = 0; iterator < iterations; iterator++)
// END V5 MACROS


// Robot configuration code.
motor MotorGroup1MotorA = motor(PORT1, ratio18_1, true);
motor MotorGroup1MotorB = motor(PORT3, ratio18_1, true);
motor_group MotorGroup1 = motor_group(MotorGroup1MotorA, MotorGroup1MotorB);



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

template <typename T>
std::string toString(T thing){
  std::ostringstream strm;
  strm << thing;
  return strm.str();
}

int num = 0;
std::vector<int> list;

template <typename T>
double mean(std::vector<T> vect){
  double sum = 0;
  for (int i = 0; i < vect.size(); i++){
    sum += vect[i];
  }
  return sum/vect.size();
}

template <typename T>
double stddev(std::vector<T> vect){
  double men = mean(vect);
  double devSum = 0;
  for (int i = 0; i < vect.size(); i++){
    devSum += pow(vect[i] - men, 2);
  }
  return pow(devSum/vect.size(), 0.5);
}

//Listener Helper Functions
void brainPrint(std::string string){ //Print new line after
  Brain.Screen.print("%s", string.c_str());
  Brain.Screen.newLine();
}

void numSet(int set){
  num = set;
  brainPrint("Num: " + toString(num));
}

//Listener Functions
void increment(){
  numSet(num+1);
}

void decrement(){
  numSet(num-1);
}

void addListItem(){
  brainPrint("Added data: " + toString(list.back()));
  list.push_back(num);
  numSet(0);
}

void removeListItem(){
  brainPrint("Removed data: " + toString(list.back()));
  list.pop_back();
}

void findMean(){
  brainPrint("Mean: " + toString(mean(list)));
}

void findStddev(){
  brainPrint("Standard Deviation: " + toString(stddev(list)));
}

void spinThing(){
  MotorGroup1.setVelocity(100, percent);
  MotorGroup1.spinFor(1, seconds);
}
//End Listener Functions


int main() {
  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();
  // Begin project code

  numSet(0);
  std::cout << "Hey" << "\n";
  controller1.ButtonUp.pressed(increment);
  controller1.ButtonDown.pressed(decrement);

  controller1.ButtonB.pressed(addListItem);
  controller1.ButtonA.pressed(removeListItem);
  controller1.ButtonX.pressed(findMean);
  controller1.ButtonY.pressed(findStddev);
  while (true){
    MotorGroup1.setVelocity(100, percent);
    if (controller1.ButtonL1.pressing()){
      MotorGroup1.spin(forward);
    }
  }
}