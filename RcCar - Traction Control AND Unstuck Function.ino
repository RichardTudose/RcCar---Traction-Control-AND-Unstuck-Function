#include <SoftwareSerial.h>
#include <Servo.h>

Servo myservo;

/* L298 v2 MOTOR - PINS attachment */
#define MOTOR_L_FORWARD 9 // motor pins
#define MOTOR_L_BACKWARD 6
#define MOTOR_R_FORWARD 5 // motor pins - MOTOR1
#define MOTOR_R_BACKWARD 4
//#define 5minute = 300000; 

/* RPM counter variables declaration */
float value = 0;
volatile float rev = 0;
volatile float rev2 = 0;
float x = 0;
int oldtime = 0;
int time;
int tempCounter = 0;

/* Variables declarations */
char c = 'S'; // 'S' = halt
float speedInput = 100; 
float speedInput2 = 100;
volatile float setInput = 100; 
float topSpeed = 250; // "top speed = 255 - ( value to equal wheels speed )"
volatile boolean tc = false; // traction control button
volatile boolean tcPlus = false; // mud or snow car unlock button

/* Temperature function variables declarations */
int ThermistorNTC = 0;
float Vo;
float R100k = 100000;
float logR, Rntc;
float T, tempC;
float Beta2550 = 3950;
float T25C = 298.15; //25C in kelvin


/* RPM for wheel 1 interrupt service routine */
void isr() {
  rev++;  
}


/* RPM for wheel 2 interrupt service routine */
void isr2() {
  rev2++;  
}


void setup() {
  Serial3.begin(9600); // bluetooth attachment
  Serial.begin(9600); // RPM monitor 
  
  pinMode( MOTOR_L_FORWARD, OUTPUT );
  pinMode( MOTOR_R_FORWARD, OUTPUT );
  pinMode( MOTOR_L_BACKWARD, OUTPUT );
  pinMode( MOTOR_R_BACKWARD, OUTPUT );
  myservo.attach(8); // works with ARDUINO MEGA 2560 timer 5

  tempCalc(); // reads initial temperature of the road
  tempProg(); // setting the car topSpeed according to the tempC
  Serial.print ("External temperature is: ");
  Serial.print (tempC);
  Serial.print (" C");
  Serial.print ("\n");
 // Showing the temperature on an LCD - if you attach one ....

  attachInterrupt( digitalPinToInterrupt(2),isr,RISING ); // attaching the 1st interrupt 
  attachInterrupt( digitalPinToInterrupt(3),isr2,RISING ); // attaching the 2nd interrupt 
}


void loop() {
     
    c = Serial3.read(); //bluetooth
    //Serial.print(c);
    //Serial.print("\n");    // prints a tab

  switch(c){
/* Values ' ' come from the bluetooth aplication on the smartphone */
     case  'F':   
      forwardSpeed();
      break;
    
     case 'X':
      tc = true;
      break;
    
     case 'x':
      tc = false;
      break;     
    
     case 'V':
      tcPlus = true;
      break;
    
     case 'v':
      tcPlus = false;
      break;     
           
     case  'B':
      backwardSpeed();
      break;
    
     case  'L':
      leftTurn();
      break;
    
     case  'R':
      rightTurn();
      break;
    
     case  'G': //forward left
      forwardSpeed();
      rightTurn();
      break;
      
     case  'I': //forward right
      forwardSpeed();
      leftTurn();
      break;
    
      case  'H': //back left
      backwardSpeed();
      leftTurn();
      break;
    
      case  'J': //back right 
      backwardSpeed();
      rightTurn();
      break;
   
/* Setting the car speed using the app */

     case  '1':
          speedInput = 25;
          speedInput2 = 25;
          setInput = 25;
          break;
    
     case  '2':
          speedInput = 50;
          speedInput2 = 50;
          setInput = 50;
          break;
      
    
     case  '3':
          speedInput = 75;
          speedInput2 = 75;
          setInput = 75;
          break;
    
     case  '4':
           speedInput = 100;
           speedInput2 = 100;
           setInput = 100;
           break; 
    
     case '5':
          speedInput = 125;
          speedInput2 = 125;
          setInput = 125;
          break;
    
     case  '6':
          speedInput = 150;
          speedInput2 = 150;
          setInput = 150;
          break;
    
     case  '7':
          speedInput = 175;
          speedInput2 = 175;
          setInput = 175;
          break;
    
     case  '8':
          speedInput = 200;
          speedInput2 = 200;
          setInput = 200;
          break; 
    
     case  '9':
          speedInput = 225;
          speedInput2 = 225;
          setInput = 225;
          break; 
    
    
     case  'q':
          speedInput = topSpeed;
          speedInput2 = topSpeed;
          setInput = topSpeed;
          break; 
    
     case  'S':
          halt();
          break;



     
 }
    

time = millis() - oldtime; // finds the time since last aquisition

if ( time > 100 ) {

  tempCounter++;
  oldtime=millis();
  rev=rev*600;
  rev2=rev2*600;

/* Serial monitor used to see the set data and the RPM values of each wheel */
  Serial.println("MOTOR1");
  Serial.println(rev);
  //Serial.println(speedInput); 
  Serial.println("MOTOR2");
  Serial.println(rev2);
  //Serial.println(speedInput);  
  //Serial.println(setInput);
 
/* if the wheels speeds differ more than 20% the traction control triggers and equals the speeds */
    if (tc == true) {
      if((rev > 1.2*rev2) || (rev2 > 1.2*rev)) {
      traction();   
      }
      else{
      speedInput2 = setInput;
      speedInput = setInput;   
      }
    }

/* if the wheels speed differ more than 100% means that the car has traction on one wheel only and that wheel will
recieve the full power to unstuck the car */
    if(tcPlus == true) {
      if((rev > 2*rev2) || (rev2 > 2*rev)) {
          FtcPlus();    
      }
      else{ 
      speedInput2 = setInput;
      speedInput = setInput;   
      }
    }   
 rev=0;
 rev2=0;
  
}// END TIME

/* Temperature aquisition */
if(tempCounter > 3000) { //every 5 minutes
  tempCounter = 0;
  tempCalc();
  //Show the temperature on the LCD..insert more code here
}

}//END LOOP



//FUNCTIONS 
/* Traction control function */
void traction() {
  Serial.println("TC! TC! TC!"); // Traction control function is triggered and it displays this message
  if(rev > rev2) {
    x =  rev / rev2;
    speedInput = speedInput / x;
  }
   if(rev2 > rev) {
    x = rev2 / rev;
    speedInput2 = speedInput2 / x;
  }
   
}

void FtcPlus() {
  Serial.println("TC+++++++++"); // Unstucking car function is triggered and it displays this message
  if(rev > rev2){
    speedInput = 0;
    speedInput2 = topSpeed;
  }
   if(rev2 > rev){
    speedInput2 = 0;
    speedInput = topSpeed;
  }
   
}

/* Function to calculate the road temperature */
void tempCalc() {
  Vo = analogRead(ThermistorNTC);
  Rntc = R100k * (1023.0 / Vo - 1.0); // measures the 100k thermistor value in Ohms
  logR = log(R100k/Rntc);

  T = T25C*Beta2550/logR /(Beta2550/logR-T25C);
  tempC = T - 273.15; //K -> C
}

/* Setting the available power according to the road temperature for maxmimum grip */
void tempProg() {
  if (5 < tempC < 40 ) { // normal street temperature of 5 -> 40 Celsius
    topSpeed = topSpeed;
  }
  if ( tempC > 40 ) {
    topSpeed = 0.9 * topSpeed;
  }
  if ( tempC < 5 ) {
    topSpeed = 0.8*topSpeed;
  }    
}

/* Left - Right steering function */
void rightTurn() {
  myservo.write( 60 );  
}

void leftTurn() {
  myservo.write( 120 );
}
 
/* Forward - Backward car moving functions */
void forwardSpeed() {
  analogWrite( MOTOR_L_FORWARD, speedInput2 +5 ); // MOTOR2 "+5" for wheel speed equalistion
  digitalWrite( MOTOR_L_BACKWARD, LOW ); 
  analogWrite( MOTOR_R_FORWARD, speedInput ); // MOTOR1 
  digitalWrite( MOTOR_R_BACKWARD, LOW ); 
  myservo.write( 90 ); // volan neutru
}


void backwardSpeed() {
  digitalWrite( MOTOR_L_FORWARD, LOW );
  analogWrite( MOTOR_L_BACKWARD, speedInput2 +5 );
  digitalWrite( MOTOR_R_FORWARD, LOW );
  analogWrite( MOTOR_R_BACKWARD, speedInput ); //MOTOR1
  myservo.write(90); // centered steering wheel
}

void halt() {
  digitalWrite( MOTOR_L_FORWARD, 0 );
  digitalWrite( MOTOR_L_BACKWARD, 0 );
  digitalWrite( MOTOR_R_FORWARD, 0 );
  digitalWrite( MOTOR_R_BACKWARD, 0 );
  myservo.write(90); // centered steering wheel
}
