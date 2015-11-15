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

//Centerpoints for each servo
#define FRONT_RIGHT_BASE 1490
#define FRONT_LEFT_BASE  1484
#define BACK_LEFT_BASE 1482
#define BACK_RIGHT_BASE 1480

int maximumRange = 200; //Maximum range needed
int minimumRange = 0;  // Minimum range needed
int i=0;
long duration, distance1, distance2, distance3, distance4; // Duration to Calculate Distance
const int buttonPin = PUSH2; //Push Button
int buttonState = 0; // Initial Button State

Servo frontleftMotor;
Servo frontrightMotor;
Servo backrightMotor;
Servo backleftMotor;

void readUltrasonicSensor1();
void readUltrasonicSensor2();
void readUltrasonicSensor3();
void readUltrasonicSensor4();
void driveLeftBottom();
void driveForward();
void driveStop();
void forwardSlow();
void reverseSlow();

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
    pinMode(buttonPin, INPUT_PULLUP);
    buttonState = digitalRead(buttonPin);
}

void loop()
{
  /*
    while (i==0 && buttonState == HIGH) {
    driveStop();
    buttonState = digitalRead(buttonPin);
  }
  i=1;
  
   int count = 0;

   readUltrasonicSensor1();
   readUltrasonicSensor2();
   forwardSlow();
 
    while (distance1 < 10)
    {
       readUltrasonicSensor1();
       readUltrasonicSensor2();
    }
    
   driveStop();
   delay(10);
   driveReverse();
   delay(1000);
   driveStop();
   delay(50);
   readUltrasonicSensor2();
   driveLeftTop();
   delay(700);
   forwardSlow();
   delay(300);

   while (distance2 < 10 && count < 50)
   {
      readUltrasonicSensor2();
      count++;
   }

   reverseSlow();
   
   readUltrasonicSensor2();
   readUltrasonicSensor3();
 
   while (distance3 < 10)
   {
      readUltrasonicSensor3();
      readUltrasonicSensor2();
   }

   driveStop();
   delay(10);
   driveForward();
   delay(1250);
   readUltrasonicSensor2();
   driveLeftBottom();
   delay(700);
   reverseSlow();
   delay(300);
   
   count = 0;

   while (distance2 < 10 && count < 50)
   {
      readUltrasonicSensor2();
      count++;
   }

   if (distance2 > 7)
   {
     driveStop();
     delay(100);
     driveRight();
     delay(1000);
     driveStop();
     buttonState = HIGH;
     i=0;
   }
   driveStop();
   delay(250); 
   */
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
 delay(10);   
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
 delay(10);   
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
 delay(10);   
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
 delay(10);   
}

void driveForward()
{
   frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-37);
   frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE+17);
   backleftMotor.writeMicroseconds(BACK_LEFT_BASE+20);
   backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-40);
}

void driveStop()
{
   frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-1);
   frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-1);
   backleftMotor.writeMicroseconds(BACK_LEFT_BASE);
   backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-4);
}

void driveReverse()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE+30);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-40);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE-42);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+30);
}

void driveLeftBottom()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-28);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-28);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE+18);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+15);
}

void driveLeftTop()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-23);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-40);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE+11);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+27);
}

void driveRight()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE+32);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE+31);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE-38);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-37);
}

void forwardSlow()
{
   frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-30);
   frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE+13);
   backleftMotor.writeMicroseconds(BACK_LEFT_BASE+12);
   backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-29);
}

void reverseSlow()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE+18);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-28);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE-30);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+17);
}
