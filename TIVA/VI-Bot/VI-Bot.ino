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
#define LED RED_LED   // Onboard LED
#include <Servo.h>
#include <math.h>
#include <Stabilize.h>
#define FRONT_RIGHT_BASE 1490
#define FRONT_LEFT_BASE  1484
#define BACK_LEFT_BASE 1482
#define BACK_RIGHT_BASE 1480

#define Kp_X 0.5
#define Ki_X 0.0
#define Kd_X 0.0

#define Kp_Y 0.5
#define Ki_Y 0.0
#define Kd_Y 0.0

int maximumRange = 200; //Maximum range needed
int minimumRange = 0;  // Minimum range needed
int i=0;
long duration, distance1, distance2, distance3, distance4; // Duration to Calculate Distance
const int buttonPin = PUSH2; //Push Button
int buttonState = 0; // Initial Button State
double Correction1, Correction2, Correction3, Correction4;
double xError, yError;
float X, Y;
int globalLoopCounter;
long startTime, endTime;
char xyPos[15];
Servo frontleftMotor;
Servo frontrightMotor;
Servo backrightMotor;
Servo backleftMotor;

void readUltrasonicSensor1();
void readUltrasonicSensor2();
void readUltrasonicSensor3();
void readUltrasonicSensor4();
void driveStop();

Stabilize xStabilize(Kp_X, Ki_X, Kd_X);
Stabilize yStabilize(Kp_Y, Ki_Y, Kd_Y);



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
    endTime = micros();
}

void loop()
{ 
  startTime=micros();
  xyPID();
  endTime=micros();
  while (i==0 && buttonState == HIGH) {
    driveStop();
    buttonState = digitalRead(buttonPin);
  }
  i=1;
  
   int count = 0;

   readUltrasonicSensor1();
   readUltrasonicSensor2();
   driveForward();
 
    while (distance1 < 10)
    {
       readUltrasonicSensor1();
       readUltrasonicSensor2();
       xyPID();
       driveForward();
       ResetCorrection();
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
   driveForward();
   delay(300);

   while (distance2 < 10 && count < 50)
   {
      readUltrasonicSensor2();
      count++;
      xyPID();
      driveForward();
      ResetCorrection();
   }

   driveReverse();
   
   readUltrasonicSensor2();
   readUltrasonicSensor3();
 
   while (distance3 < 10)
   {
      readUltrasonicSensor3();
      readUltrasonicSensor2();
      xyPID();
      driveReverse(); 
      ResetCorrection();
    }

   /*driveStop();
   delay(10);
   driveForward();
   delay(1250);
   readUltrasonicSensor2();
   driveLeftBottom();
   delay(700);
   driveReverse();
   delay(300); */
   
   count = 0;

   while (distance2 < 10 && count < 50)
   {
      readUltrasonicSensor2();
      count++;
      xyPID();
   
   }

 /*  if (distance2 > 7)
   {
     driveStop();
     delay(100);
     driveRight();
     delay(1000);
     driveStop();
     buttonState = HIGH;
     i=0;
   } */
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
  

void driveStop()
{
   frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-1);
   frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-1);
   backleftMotor.writeMicroseconds(BACK_LEFT_BASE);
   backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-4);
}

void UpdateXY(float* X, float* Y ) {
  Serial.readBytesUntil(',', xyPos, 15);
  *X = atof(xyPos);
  Serial.readBytesUntil('\n', xyPos, 15);
  *Y = atof(xyPos);
}

void xyPID() {
  float X, Y;
  UpdateXY(&X, &Y);
  double xErrorNow = X;
  double yErrorNow = Y;
  
  xStabilize.errorCorrection(xErrorNow, yError, endTime-startTime);
  yStabilize.errorCorrection(yErrorNow, xError, endTime-startTime);
  
  if(xErrorNow > 0)
  {
    Correction1 = xStabilize.compute(xError = xErrorNow);
    Correction2 = -Correction1;
    Correction3 = xStabilize.compute(xError = xErrorNow);
    Correction4 = -Correction3;
  }
  
  else if (xErrorNow < 0)
  {
    Correction1 = -xStabilize.compute(xError = xErrorNow);
    Correction2 = -Correction1;
    Correction3 = -xStabilize.compute(xError = xErrorNow);
    Correction4 = -Correction3;
  }
  
   if(yErrorNow > 0)
 {
   Correction1 = yStabilize.compute(yError = yErrorNow);
   Correction2 = -Correction1;
   Correction3 = yStabilize.compute(yError = yErrorNow);
   Correction4 = -Correction3;
 }
 
 else if (yErrorNow < 0)
 {
   Correction1 = -yStabilize.compute(yError = yErrorNow);
   Correction2 = -Correction1;
   Correction3 = -yStabilize.compute(yError = yErrorNow);
   Correction4 = -Correction3;
 }
}

void driveForward()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-30+Correction1);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE+13+Correction2);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE+12+Correction3);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-29+Correction4);
}

void driveReverse()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE+18+Correction1);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-28+Correction2);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE-30+Correction3);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+17+Correction4);
}

void driveRight()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE+32+Correction1);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE+31+Correction2);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE-38+Correction3);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE-37+Correction4);
}

void driveLeftBottom()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-28+Correction1);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-28+Correction2);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE+18+Correction3);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+15+Correction4);
}

void driveLeftTop()
{
  frontrightMotor.writeMicroseconds(FRONT_RIGHT_BASE-23+Correction1);
  frontleftMotor.writeMicroseconds(FRONT_LEFT_BASE-40+Correction2);
  backleftMotor.writeMicroseconds(BACK_LEFT_BASE+11+Correction3);
  backrightMotor.writeMicroseconds(BACK_RIGHT_BASE+27+Correction4);
}

void ResetCorrection(){
  Correction1=0;
  Correction2=0;
  Correction3=0;
  Correction4=0;
}
