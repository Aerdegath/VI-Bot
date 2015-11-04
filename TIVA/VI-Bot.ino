// Ultrasonic Sensor and Drive Code
// ECE 496 Fall 2015
// Written By Chris Richert
// Uses Example from Sheldon Will


#define echoPin1 PC_6 // Echo Pin
#define trigPin1 PD_3 // Trigger Pin
#define echoPin2 PC_7 // Left Echo
#define trigPin2 PE_1 // Left Trigger
#define echoPin3 PC_5 // Back Echo
#define trigPin3 PD_2 // Back Trigger
#define echoPin4 PC_4 // Right Echo
#define trigPin4 PD_1 // Right Trigger
#define LED RED_LED // Onboard LED
#include <Servo.h>

int maximumRange = 200; //Maximum range needed
int minimumRange = 0;  // Minimum range needed
long duration, distance1, distance2, distance3, distance4; // Duration to Calculate Distance


Servo frontleftMotor;
Servo frontrightMotor;
Servo backrightMotor;
Servo backleftMotor;

void readUltrasonicSensor1();
void readUltrasonicSensor2();
void readUltrasonicSensor3();
void readUltrasonicSensor4();
void stateMachine();
void driveForward();
void driveStop();

void setup()
{
    Serial.begin(9600);
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);
    pinMode(trigPin3, OUTPUT);
    pinMode(echoPin3, INPUT);
    pinMode(trigPin4, OUTPUT);
    pinMode(echoPin4, INPUT);
    pinMode(LED, OUTPUT);
    backleftMotor.attach(PF_2);   
    frontleftMotor.attach(PF_3);
    frontrightMotor.attach(PA_6);
    backrightMotor.attach(PA_7);
}

void loop()
{
 readUltrasonicSensor1();
// readUltrasonicSensor2();
// readUltrasonicSensor3();
// readUltrasonicSensor4();
 driveForward();
 
 while (distance1 < 10)
 {
     readUltrasonicSensor1();
 }

driveStop();
delay(250);

driveRight();
delay(500);

driveStop();
delay(250);

driveReverse();
readUltrasonicSensor3();
 
 while (distance3 < 10)
 {
     readUltrasonicSensor3();
 }

driveStop();
delay(250);

driveRight();
delay(500);

driveStop();
delay(250);
}

void readUltrasonicSensor1()
{
 /* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin1, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin1, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin1, LOW);
 duration = pulseIn(echoPin1, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance1 = duration/58.2; 
 
 //Delay 20ms before next reading.
 delay(2);   
}

void readUltrasonicSensor2()
{
 /* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin2, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin2, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin2, LOW);
 duration = pulseIn(echoPin2, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance2 = duration/58.2; 
 
 //Delay 20ms before next reading.
 delay(2);   
}

void readUltrasonicSensor3()
{
 /* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin3, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin3, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin3, LOW);
 duration = pulseIn(echoPin3, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance3 = duration/58.2; 
 //Delay 20ms before next reading.
 delay(2);   
}

void readUltrasonicSensor4()
{
 /* The following trigPin/echoPin cycle is used to determine the
 distance of the nearest object by bouncing soundwaves off of it. */ 
 digitalWrite(trigPin4, LOW); 
 delayMicroseconds(2); 

 digitalWrite(trigPin4, HIGH);
 delayMicroseconds(10); 
 
 digitalWrite(trigPin4, LOW);
 duration = pulseIn(echoPin4, HIGH);
 
 //Calculate the distance (in cm) based on the speed of sound.
 distance4 = duration/58.2; 
 
 //Delay 20ms before next reading.
 delay(2);   
}

void driveForward()
{
   frontrightMotor.write(88);
   frontleftMotor.write(94);
   backleftMotor.write(93);
   backrightMotor.write(87);
}

void driveStop()
{
   frontrightMotor.write(91);
   frontleftMotor.write(91);
   backleftMotor.write(91);
   backrightMotor.write(91);
}

void driveReverse()
{
  frontrightMotor.write(94);
  frontleftMotor.write(88);
  backleftMotor.write(87);
  backrightMotor.write(94);
}

void driveLeft()
{
  frontrightMotor.write(89);
  frontleftMotor.write(88);
  backleftMotor.write(93);
  backrightMotor.write(93);
}

void driveRight()
{
  frontrightMotor.write(94);
  frontleftMotor.write(94);
  backleftMotor.write(87);
  backrightMotor.write(87);
}

void stateMachine()
{

  }
