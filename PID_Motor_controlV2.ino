/* Basic bang-bang control plant for DC motor positional control
  *  12-7-2016
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Arsenio Dev wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   
 * ----------------------------------------------------------------------------
  *  tminusarduino.blogspot.com
  *  @ascii211
  */
#include <PID_v1.h>

 int val; 
 int encoderPinA = 2; //interrupt pin 0 
 int encoderPinB = 3; //interrrupt pin 1
 int encoderPos = 0; //initial encoder count on reset
 int encoderPinALast = LOW; //assume leading signal is low
 int n = LOW;
 int range = 1024; //Setpoint range
 int STBY = 10; //standby
 int PWMA = 7; //Speed control 
 int AIN1 = 8; //Direction
 int AIN2 = 9; //Direction


int Input = 0, Output = 0, Setpoint = 0;

int Kp=.5, Ki=1, Kd=1;

PID myPID(Input, Output, Setpoint, Kp, Ki, Kd, DIRECT);


 #define LEFT 1 //Defining direction control for motor driver tb6612fng 
 #define RIGHT 0
 #define Set A0 //analog pin for setpoint potentiometer
 

 void setup() { //Straightforward setup
   myPID.SetMode(AUTOMATIC);
   myPID.SetSampleTime(50);
   myPID.SetOutputLimits(-255, 255);
   pinMode (encoderPinA,INPUT); //encoder phases need to be inputs to sense
   pinMode (encoderPinB,INPUT);
   attachInterrupt(0, doEncoderA, CHANGE); //State change triggers interrupt to prevent loss of counts
   attachInterrupt(1, doEncoderB, CHANGE);
   Serial.begin (115200); //fast serial to speed loop
   pinMode(STBY, OUTPUT);
   pinMode(PWMA, OUTPUT);
   pinMode(AIN1, OUTPUT);
   pinMode(AIN2, OUTPUT);
   pinMode(Set, INPUT);
   TCCR1B = TCCR1B & 0b11111000 | 1; // set 31Kh PWM to prevent motor noise
 } 
 
 void loop() { 
  int Target = map(analogRead(Set),0, 1024, 0, range); //map analog read value to range of counts 0-300
  // int Target = random(-255, 255);  //Random position go to 
  Setpoint = Target;
  Input = encoderPos;
  myPID.Compute();
  Drive(Output);
  
  Serial.print(Target);// Prints target count
  Serial.print(" ");
  Serial.print(Setpoint);
  Serial.print(" ");
  Serial.print(encoderPos); //Prints actual count
  Serial.print(" ");
  Serial.print(Setpoint - Input);
  Serial.print(" ");
  Serial.println(Output);
 }
 
void Drive(int out) {              //Motor control section
  if (out > 0) {
    right();                    //Drive motor CW
    analogWrite(PWMA, -1*out));
  }
  else {
    left();                      //Drive motor CCW
     analogWrite(PWMA, out);
  }
}

void right(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, HIGH);
}

void left(){
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);
}

void stop(){
  digitalWrite(AIN1, LOW);
  digitalWrite(AIN2, LOW);
}

void doEncoderA(){

  // look for a low-to-high on channel A
  if (digitalRead(encoderPinA) == HIGH) { 
    // check channel B to see which way encoder is turning
    if (digitalRead(encoderPinB) == LOW) {  
      encoderPos = encoderPos + 1;         // CW
    } 
    else {
      encoderPos = encoderPos - 1;         // CCW
    }
  }
  else   // must be a high-to-low edge on channel A                                       
  { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoderPinB) == HIGH) {   
      encoderPos = encoderPos + 1;          // CW
    } 
    else {
      encoderPos = encoderPos - 1;          // CCW
    }
  }
  //Serial.println (encoderPos, DEC);          
  // use for debugging - remember to comment out
}

void doEncoderB(){

  // look for a low-to-high on channel B
  if (digitalRead(encoderPinB) == HIGH) {   
   // check channel A to see which way encoder is turning
    if (digitalRead(encoderPinA) == HIGH) {  
      encoderPos = encoderPos + 1;         // CW
    } 
    else {
      encoderPos = encoderPos - 1;         // CCW
    }
  }
  // Look for a high-to-low on channel B
  else { 
    // check channel B to see which way encoder is turning  
    if (digitalRead(encoderPinA) == LOW) {   
      encoderPos = encoderPos + 1;          // CW
    } 
    else {
      encoderPos = encoderPos - 1;          // CCW
    }
  }
}