//code written to completion code used while loop and millis()

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C MyLCD(0x27, 20, 4); // Creates I2C LCD Object With (Address=0x27, Cols=20, Rows=4)

//pins definition
int R1 = 6;
int R2 = 5;
int modePin = 4;
int autoLed = 18;
int manualLed = 7;

//variables initializations
int iMuscleStrength = 1023; //initialize muscle strength to the lowest possible
int fMuscleStrength;
int v1 = 0;  //initialize flexion voltage to zero
int v2 = 0;  //initialize extension voltage to zero
int flexionTime;
int extensionTime;
//int flexionMax = 865; //max flexion digital voltage at auto mode
//int extensionMax = 768; //max extension digital voltage at both auto and manual mode
int ctrl1 = 0; // use to know when it reach 1 cycle
int ctrl2 = 0; // use to know if bend degree has print
int ctrl3 = 0; // use to know when thumb finishes a cycle
int ctrl4 = 0; // use to know if user is active.
int ctrl5 = 0; // use to know what bend degree is maximum
int ctrl6 = 0;
unsigned long cycleStartTime, start;
unsigned long cycleEndTime, endd;
float timePerCycle, speedPerCycle;
int bDegreeO, bDegreeTb, fbDegreeO, fbDegreeTb;
int ibDegreeO = 0;
int ibDegreeTb = 0;
int currentFlexValue;
int autoTimeMin, autoTimeMax;

void setup() {
  Serial.begin(9600);
  pinMode(R1, OUTPUT);  //Relay1 activator
  pinMode(R2, OUTPUT); //Relay2 activator
  pinMode(modePin, INPUT); //pin that controls auto or manual mode selection.
  pinMode(autoLed, OUTPUT);
  pinMode(manualLed, OUTPUT);

  MyLCD.init();
  MyLCD.backlight();
  MyLCD.setCursor(0, 0);
  MyLCD.print("    PARAMETERS:    "); 
 
}

void loop() {

  int mode = digitalRead(modePin);
  
  if (mode == 1){ //auto Mode.
    digitalWrite(manualLed, LOW); //off manual led
    digitalWrite(autoLed, HIGH); //on auto led
  
    v1 = analogRead(A4); //read flexion voltage
    v2 = analogRead(A2); //read extension voltage
 
    int minValue = 0; //change after knowing the realValue of min voltage
    int maxValue = 1023; //change after knowing the realValue of max voltage
    autoTimeMin = 1000;
    autoTimeMax = 8000;
    v1 = constrain(v1, minValue, maxValue);
    flexionTime = map(v1, minValue, maxValue, autoTimeMax, autoTimeMin); //calibrates voltages to flexion timing
    v2 = constrain(v2, minValue, maxValue);
    extensionTime = map(v2, minValue, maxValue, autoTimeMax, autoTimeMin); //calibrates voltages to extension timing

    digitalWrite(R2, LOW); //off suction pump
    digitalWrite(R1, HIGH); //on air pump
    if(ctrl6 == 0){
      start = millis();
      ctrl6 = 1;
    }
    endd = millis();
    while((endd - start) < flexionTime) {//wait until the end of flexion timing
      endd = millis();
    }
    fbDegreeO = analogRead(A13);
    fbDegreeTb = analogRead(A10);
    if(ctrl1 == 1){
      timePerCycle = ((2*flexionTime)+extensionTime) / 1000; 
      speedPerCycle = 6.3 / timePerCycle; 
      ctrl1 == 0; 
    }
    digitalWrite(R1, LOW); //off air pump
    digitalWrite(R2, HIGH); //On suction pump
    if(ctrl6 == 1){
      start = millis();
      ctrl6 = 0;
    }
    endd = millis();
    while((endd - start) < extensionTime) {//wait until the end of extension timing
      endd = millis();
    }
    ctrl1 = 1;
    
    if((fbDegreeO > ibDegreeO) && (fbDegreeTb > ibDegreeTb)){//get highest bend degree in history
          bDegreeO = fbDegreeO;
          bDegreeTb = fbDegreeTb;
          ibDegreeO = fbDegreeO;
          ibDegreeTb = fbDegreeTb;
          bDegreeO = constrain(bDegreeO, 768, 1023); //0 degree and 180 degree voltage
          bDegreeO = map(bDegreeO, 768, 1023, 0, 180); // recalibrate voltage levels to bending degrees
          bDegreeTb = constrain(bDegreeTb,760 , 840); //0 degree and 180 degree voltage
          bDegreeTb = map(bDegreeTb, 760, 840, 0, 180); // recalibrate voltage levels to bending degrees

          //print bending degree
          MyLCD.setCursor(0, 2);
          MyLCD.print("MRoF:");
          MyLCD.setCursor(5, 2);
          MyLCD.print(bDegreeO);
          MyLCD.setCursor(8,2);
          MyLCD.print("d");
          MyLCD.setCursor(11, 2);
          MyLCD.print("MRtB:");
          MyLCD.setCursor(16, 2);
          MyLCD.print(bDegreeTb);
          MyLCD.setCursor(19,2);
          MyLCD.print("d");
        }

    //print time per cycle
    MyLCD.setCursor(0, 1);
    MyLCD.print("Time/Cycle: ");
    MyLCD.setCursor(12, 1);
    MyLCD.print(timePerCycle);
    MyLCD.setCursor(16, 1);
    MyLCD.print("s");
    
    //print speed per cycle
    MyLCD.setCursor(0, 3);
    MyLCD.print("S/C:");
    MyLCD.setCursor(4, 3);
    MyLCD.print(speedPerCycle);
    MyLCD.setCursor(8, 3);
    MyLCD.print("r/s");
    
     
  }
  
  else{ //manual Mode.

  //do all you did in auto mode with both motors in off state.
  digitalWrite(R1, LOW); //off air pump
  digitalWrite(R2, LOW); //off suction pump

  digitalWrite(autoLed, LOW); //off auto led
  digitalWrite(manualLed, HIGH); //on manual led

  fMuscleStrength = analogRead(A7);
  int strengthN = map(fMuscleStrength, 1023, 200 , 0, 30);
  
  //print highest muscle strength recorded in Manual Mode.
  if(fMuscleStrength < iMuscleStrength){ //display highest muscle strength recorded. it uses negative logic
      iMuscleStrength = fMuscleStrength; //in terms of voltage

      MyLCD.setCursor(12, 3);
      MyLCD.print("M.S:");
      MyLCD.setCursor(16, 3);
      MyLCD.print(strengthN);
      MyLCD.setCursor(19, 3);
      MyLCD.print("N");
    }   
}
}   
 
