/*
////////////////////////////////////////////////
////////////////////////////////////////////////
      TUGAS AKHIR V1.3
///////////////////////////////////////////////
///////////////////////////////////////////////

/---------CONFIGURASI PIN-----------/
 * pin Trigger ultrasonik = 5
 * pin Echo ultrasonik 1  = 6
 * pin Echo ultrasonik 2  = 7
 * pin Echo ultrasonik 3  = 8

 * pin DT encode      = 3
 * pin CLK encoder    = 4
 * pin button encoder = 2

 * pin DT LOAD CELL   = 9
 * pin CLK LOAD CELL  = 10
 * 
 * pin BUZZER         = 22

 * led[0] <HIJAU> = 11 ->indikator run
 * led[1] <MERAH> = 12 ->indikator lock
 * led[2] <BIRU>  = 13 ->indikator kalibrasi
 */
#include <HX711_ADC.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>
#include <OneButton.h>
#include <Encoder.h>
#include <EEPROM.h>

//------rotary encoder-------//
#define encoderDTpin 3    //STATIC
#define encoderCLKpin 4   //STATIC
//--------load cell----------//
#define loadCelDTpin   9    //STATIC
#define loadCellSCKpin 10 //STATIC
//--------ultrasonik---------//
#define triggerPin1 5      //STATIC
#define triggerPin2 A1
#define triggerPin3 A2
#define echoPin1   8      //STATIC
#define echoPin2   6      //STATIC
#define echoPin3   7      //STATIC
//----------button-----------//
#define buttonReset 2     //STATIC
//----------buzzer-----------//
#define buzzer A0//22         //STATIC
//------led indikator--------//
int led[]={ 11, 12, 13};  //STATIC

//---------CONFIGURASI OOP----------//
HX711_ADC scale(loadCelDTpin, loadCellSCKpin); 
Encoder myEnc(encoderDTpin, encoderCLKpin);
OneButton button0(buttonReset, true);
HCSR04 hc1(triggerPin1,echoPin1); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
HCSR04 hc2(triggerPin2,echoPin2); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
HCSR04 hc3(triggerPin3,echoPin3); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display



//--------------CONFIGURASI VARIABEL-------------//
byte cursorLayer = 0;
byte subLayer=0;
byte currentLayer =0;
bool flagTr  = true;
bool stepLayer=1;
bool flagS=0;
bool stateRun= 1;
bool runObject = true;
bool trigger   = false;
bool state1 = false;
bool flagWeight = false;
int  length,height,width;
int  timerFlag;
int  timeRead = 0;
int  lastStep,lastSleep,lastLock;
int  timerLock,timerSleep;
float hasilP = 0.0;
float hasilL = 0.0;
float hasilT = 0.0;
float valueLength = 0.0;
float valueWidth  = 0.0;
float valueHeight = 0.0;
float errorLength = 0.0;
float errorWidth  = 0.0;
float errorHeight = 0.0;
int currentSelect = 1;
int currentLength;
int units   = -1;
int weightToInt;
int conCal =0;
int coT=0;
float unitSetting = -1;
float ounces;
float parWeight=50;
float weight;
long oldPosition  = 0;
long newPosition = 0;

float referenceLength = 0.0; // Panjang referensi dalam cm
float referenceWidth  = 0.0;  // Lebar referensi dalam cm
float referenceHeight = 0.0; // Tinggi referensi dalam cm

float calibration_factor = 696.0; //NILAI KALIBRASI DEFAULT

String menu1[]={"5","kalibrasi Beban","Kalibrasi Dimensi","Set Timer Lock","Set Timer Sleep","Back"};
String menuJarak[]={"5","Sensor 1","Sensor 2","Sensor 3","auto","Back"};
String menuSensor1[]={"3","MAXSIMUM:","ERROR   :","BACK"};
String menuSensor2[]={"3","MAXSIMUM:","ERROR   :","BACK"};
String menuSensor3[]={"3","MAXSIMUM:","ERROR   :","BACK"};
char *panah[]{" ","<",">"};
char *textWeight[]{" Gram"," KG  "};

byte zero[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = {
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};

byte two[] = {
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};

byte three[] = {
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};

byte four[] = {
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};

byte five[] = {
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};

byte speaker[] = {
  B00010,
  B00110,
  B11110,
  B11110,
  B11110,
  B11110,
  B00110,
  B00010
};

byte pointer[] = {
  B00000,
  B00100,
  B01000,
  B11111,
  B01000,
  B00100,
  B00000,
  B00000
};
 
void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  scale.begin(); 
  lcd.begin();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
  for(int i = 0; i < 3; i++){pinMode(led[i],OUTPUT);}
  button0.attachClick(singleClick);
  button0.attachDoubleClick(doubleclick1);
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.createChar(6, speaker);
  lcd.createChar(7, pointer);

  unsigned long stabilizingtime = 2000; // preciscion right after power-up can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  scale.start(stabilizingtime, _tare);
  
  if (scale.getTareTimeoutFlag()|| scale.getSignalTimeoutFlag()) {
    Serial.println("Timeout, check MCU>HX711 wiring and pin designations");
    lcd.setCursor(0,1);
    lcd.print("ERROR in LOAD CELL!!");
    while (1);
  }
  else {
    scale.setCalFactor(calibration_factor); // set calibration value (float)
    Serial.println("Startup is complete");
    for(int i = 0; i < 100; i++){
      lcd.setCursor(0,0);
      lcd.print("LOADING..");
      lcd.setCursor(16,0);
      lcd.print(i);
      lcd.print("%");
      updateProgressBar(i, 100, 1);
      delay(50);
    }
    EEPROM.get(0,calibration_factor);
    EEPROM.get(20,valueLength);
    EEPROM.get(30,valueWidth);
    EEPROM.get(40,valueHeight);
    EEPROM.get(50,timerLock);
    EEPROM.get(60,timerSleep);
    EEPROM.get(70,errorLength);
    EEPROM.get(80,errorWidth);
    EEPROM.get(90,errorHeight);
  }
  
  referenceLength= valueLength;
  referenceWidth = valueWidth;
  referenceHeight= valueHeight;

  lcd.clear();

}

void loop() {
    button0.tick();
    timerLCD();
    getRotary();
    kalkulasi();
    showSetting();
    showLed();
    //showMonitor();
}

//----------------TAMPILAN INDIKATOR LED---------------//
/*
void showLed(){
    if(state1 == 1 && runObject == 1 && currentLayer == 0){getIndikator(0,1,0);}
    else if(state1 == 0 && runObject == 1 && currentLayer == 0){getIndikator(0,0,1); getIndikator(1,0,0); getIndikator(2,0,0);}
    else if(state1 == 0 && runObject == 0 && currentLayer == 0){getIndikator(0,0,0); getIndikator(1,0,1); getIndikator(2,0,0);}
    else if(currentLayer == 1 && subLayer == 0){getIndikator(0,0,0); getIndikator(1,0,0); getIndikator(2,0,1);}
    else if(currentLayer != 1 && subLayer == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 2){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 6 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 7 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 8 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 6 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 7 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 8 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 3){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 4){ getIndikator(2,1,0); }

}
*/

void showLed(){
    if(state1 == 1 && runObject == 1 && currentLayer == 0){getIndikator(0,1,0);}
    else if(state1 == 0 && runObject == 1 && currentLayer == 0){getIndikator(0,0,1); getIndikator(1,0,0); getIndikator(2,0,0);}
    //else if(state1 == 0 && runObject == 0 && currentLayer == 0){getIndikator(0,0,0); getIndikator(1,0,1); getIndikator(2,0,0);}
    else if(currentLayer == 1 && subLayer == 0){getIndikator(0,0,0); getIndikator(1,0,0); getIndikator(2,0,1);} //masuk seting
    else if(currentLayer == 0 && runObject == 1 && flagWeight == 1){getIndikator(0,0,0); getIndikator(1,0,1); getIndikator(2,0,0);}
    else if(currentLayer != 1 && subLayer == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 2){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 6 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 7 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 8 && flagS == 0){ getIndikator(2,0,1); }
    else if(currentLayer != 1 && subLayer == 6 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 7 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 8 && flagS == 1){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 3){ getIndikator(2,1,0); }
    else if(currentLayer != 1 && subLayer == 4){ getIndikator(2,1,0); }
    
}


//-------------------PROGRAM BUTTON SINGLE CLICK------------------------//
void singleClick(){
 
  Serial.println("button 1 klik run");
  
  if(currentLayer == 0 && subLayer == 0 ){ 
    
    lcd.clear();
    hasilP  = 0; 
    hasilL  = 0; 
    hasilT  = 0; 
    weight  = 0.00;
    trigger = false; 
    runObject = true; 
    timeRead  = 0; 
    state1=0;
    stateRun  = 0;
    coT=0;
    scale.tare();
    Serial.println(String() + "trigger  :" + trigger);
    Serial.println(String() + "runObject:" + runObject);
    Serial.println(String() + "timeRead :" + timeRead);
    Serial.println(String() + "stateRun :" + stateRun);
  }

 if(currentLayer == 1 && subLayer == 0){ 
  switch(currentSelect){
    case 1 :
      clearMenu();
      subLayer = 1;
      currentLayer = -1;
      currentSelect = 1;
      conCal = 1;
      cursorSelect();
    break;

    case 2 :
      clearMenu();
      subLayer = 2;
      currentLayer =-1;
      currentSelect = 1;
     cursorSelect();
    break;

    case 3 :
      clearMenu();
      subLayer = 3;
      currentLayer = -1;
      currentSelect = 1;
      cursorSelect();
    break;

    case 4 :
      clearMenu();
      subLayer = 4;
      currentLayer = -1;
      currentSelect = 1;
      cursorSelect();
    break;

    case 5 :
      clearMenu();
      scale.tare();
      stepLayer = 1;
      currentLayer = 0;
      timeRead  = 0; 
      timerFlag = 0;
      state1=0;
      coT=0;
    break;
  };
 }
 
 else if(currentLayer != 1 && subLayer == 1){
  
  clearMenu();
  lcd.setCursor(1,1);
  lcd.print("KALIBRASI SELESAI");
  subLayer = 0;
  currentLayer = 1;
  currentSelect = 1;
  cursorSelect();
  conCal = 0;
  for(int i =0; i < 2;i++){ buzzerRun(1); delay(50); buzzerRun(0); delay(50);}
  scale.refreshDataSet();
  calibration_factor = scale.getNewCalibration(parWeight); //get the new calibration value
  EEPROM.put(0,calibration_factor);
  Serial.println("calibration_factor1:" + String(calibration_factor));
  Serial.println("parWeight:" + String(parWeight));
  clearMenu();
 }

 else if(currentLayer != 1 && subLayer == 3 ){
   clearMenu();
   subLayer = 0;
   currentLayer = 1;
   currentSelect = 3;
   cursorSelect();
   EEPROM.put(50,timerLock);
 }

 else if(currentLayer != 1 && subLayer == 4 ){
   clearMenu();
   subLayer = 0;
   currentLayer = 1;
   currentSelect = 4;
   cursorSelect();
   EEPROM.put(60,timerSleep);
 }


 else if(currentLayer != 1 && subLayer == 2 ){
    
 switch(currentSelect){
    case 1 :
      clearMenu();
      subLayer = 6;
      currentSelect = 1;
      cursorSelect();
      flagS = 1;
    break;

    case 2 :
      clearMenu();
      subLayer = 7;
      currentSelect = 1;
      cursorSelect();
      flagS = 1;
    break;

    case 3 :
     clearMenu();
     subLayer = 8;
     currentSelect = 1;
     cursorSelect();
     flagS = 1;
    break;

    case 4 :
     clearMenu();
     subLayer = 5;
    break;

    case 5 :
      clearMenu();
      subLayer = 0;
      currentLayer = 1;
      currentSelect = 2;
      flagS = 0;
      cursorSelect();
    break;
  };
 }

 if(currentLayer != 1 && subLayer == 6 ){
  switch(currentSelect){
    case 1 :
      flagS = !flagS;
      clearMenu();
    break;
    case 2 :
      flagS = !flagS;
      clearMenu();
    break;
    case 3 :
      subLayer=2;
      currentSelect = 1;
      cursorSelect();
      clearMenu();
      referenceLength = valueLength;
      EEPROM.put(20,valueLength);
      EEPROM.put(70,errorLength);
    break;
  };
 }

 if(currentLayer != 1 && subLayer == 7 ){
  switch(currentSelect){
    case 1 :
      flagS = !flagS;
      clearMenu();
    break;
    case 2 :
      flagS = !flagS;
      clearMenu();
    break;
    case 3 :
      subLayer=2;
      currentSelect = 2;
      cursorSelect();
      clearMenu();
      referenceWidth  = valueWidth;
      EEPROM.put(30,valueWidth);
      EEPROM.put(80,errorWidth);
    break;
  };
 }

 if(currentLayer != 1 && subLayer == 8 ){
  switch(currentSelect){
    case 1 :
      flagS = !flagS;
      clearMenu();
    break;
    case 2 :
      flagS = !flagS;
      clearMenu();
    break;
    case 3 :
      subLayer=2;
      currentSelect = 3;
      cursorSelect();
      clearMenu();
      referenceHeight = valueHeight;
      EEPROM.put(40,valueHeight);
      EEPROM.put(90,errorHeight);
    break;
  };
 }
}

//------------------------PROGRAM BUTTON DOUBLE CLICK--------------------------//
void doubleclick1(){
  Serial.println("button 2 klik run");
 if(currentLayer == 0){
    currentLayer = 1;
    currentSelect = 1;
    cursorSelect();
    clearMenu();
    trigger = false; 
    stateRun  = 0;
    state1 = 0;
 }
}

//----------------------PROGRAM MENGAMBIL DATA DARI ROTARY ENCODE-----------------------//
void getRotary(){
  newPosition = myEnc.read()/4;
  switch(currentLayer){
     case 1 :
       flagTr  = 1;
       state1 = 0;
       currentLength = menu1[0].toInt();
     break;
  };
 
  switch(subLayer){
     case 1 :
       currentLength = 1;
     break;
      
     case 2 :
       currentLength = menuJarak[0].toInt();
     break;
 
     case 3 :
       currentLength = 1;
     break;

     case 6 :
       currentLength = menuSensor1[0].toInt();
     break;

     case 7 :
       currentLength = menuSensor2[0].toInt();
     break;

     case 8 :
       currentLength = menuSensor3[0].toInt();
     break;


  };

  

  if (newPosition != oldPosition){
    if (newPosition < oldPosition && currentSelect < currentLength && flagS == 0) {
      clearSelect();
      currentSelect++;
      cursorSelect(); 
    }
  
    else if(newPosition > oldPosition && currentSelect != 1 && flagS == 0){
      clearSelect();
      currentSelect--;
      cursorSelect();
    }
    
    //-----TAMPILAN MENU------//
    if(currentLayer==1 || subLayer == 2){
      
      if(currentSelect == 5 && stepLayer == 1){
        stepLayer=0; 
        if(currentSelect != lastStep){lcd.clear();  }
      }
  
      if(currentSelect == 4 && stepLayer == 0){
        stepLayer=1; 
        if(currentSelect != lastStep){lcd.clear();  }
      }
        lastStep = currentSelect; 
    }
    
    //-------------TAMPILAN SUB MENU 1 (SET SENSOR BERAT)---------------//
    if(subLayer==1){
      if(newPosition < oldPosition && parWeight < 1000) { parWeight++; }
      else if(newPosition > oldPosition && parWeight != -50){ parWeight--; }
    }
  
    //-------------TAMPILAN SUB MENU 2 (SET SENSOR JARAK/DIMENSI)---------------//
    if(subLayer==6  && currentSelect == 1 && flagS == 1){
      if(newPosition < oldPosition && valueLength < 50) { valueLength+=0.1; } 
      else if(newPosition > oldPosition && valueLength > 0.1){ valueLength-=0.1; }  
    }
  
    if(subLayer==7 && currentSelect == 1 && flagS == 1){
      if(newPosition < oldPosition && valueWidth < 50) {  valueWidth+=0.1; }
      else if(newPosition > oldPosition && valueWidth > 0.1){ valueWidth-=0.1; }
    }
  
    if(subLayer==8 && currentSelect == 1 && flagS == 1){
      if(newPosition < oldPosition && valueHeight < 50) { valueHeight+=0.1; }
      else if(newPosition > oldPosition && valueHeight > 0.1){ valueHeight-=0.1; }
    }

    //-------------TAMPILAN SUB MENU 2 (SET error SENSOR JARAK/DIMENSI)---------------//
    if(subLayer==6  && currentSelect == 2 && flagS == 1){
      if(newPosition < oldPosition && errorLength < 50.0) { errorLength+=0.1; } 
      else if(newPosition > oldPosition && errorLength > -50.0){ errorLength-=0.1; }  
    }
  
    if(subLayer==7 && currentSelect == 2 && flagS == 1){
      if(newPosition < oldPosition && errorWidth < 50.0) {  errorWidth+=0.1; }
      else if(newPosition > oldPosition && errorWidth > -50.0){ errorWidth-=0.1; }
    }
  
    if(subLayer==8 && currentSelect == 2 && flagS == 1){
      if(newPosition < oldPosition && errorHeight < 50.0) { errorHeight+=0.1; }
      else if(newPosition > oldPosition && errorHeight > -50.0){ errorHeight-=0.1; }
    }
  
    //-------------TAMPILAN SUB MENU 3 (SET TIMER LOCK LCD)---------------//
    if(subLayer==3){
      if(newPosition < oldPosition && timerLock < 20) {
        timerLock++;
        if(timerLock != lastLock){ lcd.setCursor(9,2); lcd.print(panah[2]);}
      }
  
      else if(newPosition > oldPosition && timerLock != 3){
        timerLock--;
        if(timerLock != lastLock){ lcd.setCursor(4,2); lcd.print(panah[1]); }
      }
  
      delay(500);
      lastLock = timerLock;
      if(timerLock == lastLock){lcd.setCursor(4,2);lcd.print(panah[0]); lcd.setCursor(9,2);lcd.print(panah[0]);}
    }
    
    //-------------TAMPILAN SUB MENU 4 (SET TIMER SLEEP LCD)---------------//
    if(subLayer==4){
      if(newPosition < oldPosition && timerSleep < 20) {
        timerSleep++;
        if(timerSleep != lastSleep){lcd.setCursor(9,2); lcd.print(panah[2]);}
      }
  
      else if(newPosition > oldPosition && timerSleep != 0){
        timerSleep--;
        if(timerSleep != lastSleep){lcd.setCursor(4,2); lcd.print(panah[1]); }
      }
  
      delay(500);
      lastSleep = timerSleep;
      if(timerSleep == lastSleep){ lcd.setCursor(4,2);lcd.print(panah[0]); lcd.setCursor(9,2);lcd.print(panah[0]);}
    }
    if(subLayer==6){
      
    }
    oldPosition = newPosition;
  }
}

//--------------ANIMASI LOADING------------------//
void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
 {
    double factor    = totalCount/100.0;          //See note above!
    int    percent   = (count+1)/factor;
    int    number    = percent/5;
    int    remainder = percent%5;
    if(number > 0)
    {
      for(int j = 0; j < number; j++)
      {
       lcd.setCursor(j,lineToPrintOn);
       lcd.write(5);
      }
    }

    lcd.setCursor(number,lineToPrintOn);
    lcd.write(remainder); 

    if(number < 20)
    {
      for(int j = number+1; j <= 20; j++)
      {
        lcd.setCursor(j,lineToPrintOn);
        lcd.write(0);
      }
    }  
 }

//-----------------------TAMPILAN LCD--------------------//
void showSetting(){
  int dataSensor[]={valueLength,valueWidth,valueHeight,0};
  if(currentLayer==0 && subLayer == 0){

    weight = getWeight();

    if(flagTr==true){
       
      weightToInt = weight;
  
      lcd.backlight();
      lcd.setCursor(0,0);
      lcd.print("Panjang:");
      lcd.print(hasilP,1);
      if(hasilP<10.0){  lcd.setCursor(11,0); lcd.print(" "); } 
      lcd.setCursor(12,0);
      lcd.print(" cm");
      
      lcd.setCursor(0,1);
      lcd.print("Lebar  :");
      lcd.print(hasilL,1); 
      if(hasilL<10){ lcd.setCursor(11,1); lcd.print(" "); }
      lcd.setCursor(12,1);
      lcd.print(" cm");
      
      lcd.setCursor(0,2);
      lcd.print("Tinggi :");
      lcd.print(hasilT,1);
      if(hasilT<10){ lcd.setCursor(11,2); lcd.print(" "); }
      lcd.setCursor(12,2);
      lcd.print(" cm");

      lcd.setCursor(0,3);
      lcd.print("Berat  :");
      if(weight >= 1000){ lcd.print(weight /1000); } else{ lcd.print(weightToInt); }
     
      if(weightToInt < 10)       { clearChar(9,3); clearChar(10,3);  clearChar(11,3);}
      else if(weightToInt < 100) { clearChar(10,3);  clearChar(11,3); }
      else if(weightToInt < 1000){ clearChar(11,3); }
  
      lcd.setCursor(12,3);
      lcd.print((weight >= 1000)?textWeight[1]:textWeight[0]);
  
      if(weight >= 5000){
        lcd.setCursor(18,3);
        lcd.print("!");
        lcd.setCursor(19,3);
        lcd.write(6);
        if(runObject)buzzerRun(1); flagWeight = 1;
      }
      else{
        lcd.setCursor(18,3);
        lcd.print(" ");
        lcd.setCursor(19,3);
        lcd.print(" ");
        buzzerRun(0);
        flagWeight = 0;
      }
    }
   else{ lcd.noBacklight(); clearMenu();}
  }

  if(currentLayer==1 && subLayer == 0){
    lcd.backlight();
    lcd.setCursor(18,cursorLayer );
    lcd.write(byte(7));
    if(currentSelect < 5){
      for(int i=0;i<4;i++){
        lcd.setCursor(0,i);
        lcd.print(menu1[i+1]);
      }

      for(int i=0;i<2;i++){
        lcd.setCursor(19,i);
        lcd.print("|");
      }
    }

    if(currentSelect > 4){
      for(int i=0;i<currentLength-4;i++){
        lcd.setCursor(0,i);
        lcd.print(menu1[5+i]);
      }

      for(int i=0;i<2;i++){
        lcd.setCursor(19,2+i);
        lcd.print("|");
      }
    }
  }

  if(subLayer==1){
     static int conLevel=0;
     
    if(conCal==1){
      static unsigned long save = 0;
      scale.update();
      
      if(millis() - save > 500){
        save = millis();
        if(conLevel==8){ scale.tareNoDelay(); }
        lcd.setCursor(0,1);
        lcd.print("KOSONGKAN TIMBANGAN!");
        conLevel++;
        if (scale.getTareStatus() == true && conLevel >= 8) {
          conLevel = 0;
          conCal = 2;
          save = 0;
          lcd.clear();
          buzzerRun(1);
          delay(100);
          buzzerRun(0);
        }
      }
      
      Serial.println(String() + "conLevel:" + conLevel);
    }

    else if(conCal==2){
      scale.update();
      lcd.setCursor(3,0);
      lcd.print("TARUH BEBAN!!!");
      lcd.setCursor(0,2);
      lcd.print("Parameter : ");
      lcd.print(int(parWeight));
      lcd.print(" Gram");
    }
    
  }
   
  if(subLayer==2){
    lcd.setCursor(18,cursorLayer ); 
    lcd.write(byte(7));

    if(currentSelect < 5){
      for(int i=0;i<4;i++){
        lcd.setCursor(0,i);
        lcd.print(menuJarak[i+1]);
        
        for(int i=0;i<2;i++){
          lcd.setCursor(19,i);
          lcd.print("|");
        } 
      }
    }

    if(currentSelect > 4){
      for(int i=0;i<currentLength-4;i++){
        lcd.setCursor(0,i);
        lcd.print(menuJarak[5+i]);
      }

      for(int i=0;i<2;i++){
        lcd.setCursor(19,2+i);
        lcd.print("|");
      }
    }


    
  }

  if(subLayer==3){
    lcd.setCursor(2,0);
    lcd.print("SET TIMER LOCK");
    lcd.setCursor(6,2);
    lcd.print((timerLock<10)?"0"+String(timerLock):timerLock);
  }

  if(subLayer==4){
    lcd.setCursor(2,0);
    lcd.print("SET TIMER SLEEP");
    lcd.setCursor(6,2);
    lcd.print((timerSleep < 1)?"OFF" :(timerSleep<10)?"0"+String(timerSleep)+" ":timerSleep);
  }

  if(subLayer==5){
    for(int i = 0; i < 10; i++){
      referenceLength = hc1.dist() + 1; buzzerRun(1); 
      delay(60);
    }
    for(int i = 0; i < 10; i++){
      referenceWidth  = hc2.dist() + 1; buzzerRun(1);
      delay(60);
    }

    for(int i = 0; i < 10; i++){
      referenceHeight  = hc3.dist() + 1; buzzerRun(1);
      delay(60);
    }
    
    valueLength = referenceLength;
    valueWidth  = referenceWidth;
    valueHeight = referenceHeight;
    
    EEPROM.put(20,valueLength);
    EEPROM.put(30,valueWidth);
    EEPROM.put(40,valueHeight);

    lcd.setCursor(1,0);
    lcd.print("KALIBRASI SELESAI");

    lcd.setCursor(0,1);
    lcd.print("Panjang: ");
    lcd.print(referenceLength,1);
    lcd.setCursor(15,1);
    lcd.print(" CM");

    lcd.setCursor(0,2);
    lcd.print("Lebar  : ");
    lcd.print(referenceWidth,1);
    lcd.setCursor(15,2);
    lcd.print(" CM");

    lcd.setCursor(0,3);
    lcd.print("Tinggi : ");
    lcd.print(referenceHeight,1);
    lcd.setCursor(15,3);
    lcd.print(" CM");

    buzzerRun(0);
    subLayer = 2;
    delay(3000);
    lcd.clear();
  }

  if(subLayer == 6){
    lcd.setCursor(18,cursorLayer ); 
    lcd.write(byte(7));
    for(int i=0;i<currentLength;i++){lcd.setCursor(0,i); lcd.print(menuSensor1[i+1]);}
    lcd.setCursor(10,0);
    lcd.print(valueLength,1); 
    lcd.setCursor(15,0);
    lcd.print("CM"); 
    lcd.setCursor(10,1);
    lcd.print(errorLength,1); 
    lcd.setCursor(15,1);
    lcd.print("CM");
    if(flagS){
      lcd.setCursor(9,cursorLayer); 
      lcd.print("*");
      if(valueLength<10){ lcd.setCursor(13,0); lcd.print(" "); }
      if(errorLength >= 0.0 && errorLength<10){ lcd.setCursor(13,1); lcd.print(" "); }
      if(errorLength>-10.0){ lcd.setCursor(14,1); lcd.print(" "); }
    }
  }
  if(subLayer == 7){
    lcd.setCursor(18,cursorLayer ); 
    lcd.write(byte(7));
    for(int i=0;i<currentLength;i++){lcd.setCursor(0,i); lcd.print(menuSensor2[i+1]);}
    
    lcd.setCursor(10,0);
    lcd.print(valueWidth,1); 
    lcd.setCursor(15,0);
    lcd.print("CM");
    lcd.setCursor(10,1);
    lcd.print(errorWidth,1); 
    lcd.setCursor(15,1);
    lcd.print("CM");
    if(flagS){
      lcd.setCursor(9,cursorLayer); 
      lcd.print("*");
      if(valueWidth<10){ lcd.setCursor(13,0); lcd.print(" "); }
      if(errorWidth >= 0.0 && errorWidth<10){ lcd.setCursor(13,1); lcd.print(" "); }
      if(errorWidth>-10.0){ lcd.setCursor(14,1); lcd.print(" "); }
    }
  }
  if(subLayer == 8){
    lcd.setCursor(18,cursorLayer ); 
    lcd.write(byte(7));
    for(int i=0;i<currentLength;i++){lcd.setCursor(0,i); lcd.print(menuSensor3[i+1]);}
    
    lcd.setCursor(10,0);
    lcd.print(valueHeight,1);
    lcd.setCursor(15,0); 
    lcd.print("CM");
    lcd.setCursor(10,1);
    lcd.print(errorHeight,1); 
    lcd.setCursor(15,1);
    lcd.print("CM");
    if(flagS){
      lcd.setCursor(9,cursorLayer); 
      lcd.print("*");
      if(valueHeight<10){ lcd.setCursor(13,0); lcd.print(" "); }
      if(errorHeight >= 0.0 && errorHeight<10){ lcd.setCursor(13,1); lcd.print(" "); }
      if(errorHeight>-10.0){ lcd.setCursor(14,1); lcd.print(" "); }
    }
  }
}

//----------------PROGRAM MENGHITUNG VALUE SENSOR ULTRASONIK-------------------//
void kalkulasi(){
  
  unsigned long        tmr = millis();
  static unsigned long saveTmr1 = 0;
  static unsigned long saveTmr2 = 0;
  static unsigned long saveTmr3 = 0;
  static float         panjang = 0.0,lebar = 0.0,tinggi = 0.0;
  static int           co;
  static int           conSen=0;
  int                  valueMax = timerLock / 3;
 
  if(tmr - saveTmr1 > 50 && trigger == true){
    saveTmr1 = tmr;
    conSen++;
    for(int i = 0; i < valueMax; i++){
      panjang = hc1.dist() + 1; 
      Serial.println("conSen panjang:" + String(conSen));
      delay(60);
    }

    for(int i = 0; i < valueMax; i++){
      lebar  = hc2.dist() + 1; 
      Serial.println("conSen lebar   :" + String(conSen));
      delay(60);
    }

    for(int i = 0; i < valueMax; i++){
     tinggi  = hc3.dist() + 1; 
      Serial.println("conSen tinggi  :" + String(conSen));
      delay(60);
    }
    
  }


  if(trigger){ 
    co = (millis() - saveTmr3)/1000;
    lcd.setCursor(18,0);
    lcd.print(co);
    if(co<10){lcd.setCursor(19,0); lcd.print(" "); }

    if(panjang <= referenceLength){ hasilP = referenceLength - (panjang+errorLength); } else{  hasilP = 0.0; }
    if(lebar <= referenceWidth){ hasilL = referenceWidth - (lebar+errorWidth); }       else{  hasilL = 0.0; }
    if(tinggi <= referenceHeight){  hasilT = referenceHeight - (tinggi+errorHeight); }  else{  hasilT = 0.0; }

    if(hasilP < 0){ hasilP = 0.0; }
    if(hasilL < 0){ hasilL = 0.0; }
    if(hasilT < 0){ hasilT = 0.0; }

  }else{ 
    saveTmr3 = millis(); 
    co=0; 
  }

  if(tmr - saveTmr2 > 1000 && trigger == true){

    saveTmr2 = tmr;

    if(timeRead < timerLock){ timeRead++;  }

    else{
      lcd.noBacklight();
      buzzerRun(1); 
      delay(50); 
      lcd.backlight(); 
      buzzerRun(0); 
      hasilP = hasilP;
      hasilL = hasilL;
      hasilT = hasilT; 
      trigger = false; 
      runObject = false; 
      timeRead=0; 
      stateRun = 0;
      clearChar(18,0);
      clearChar(19,0);
    }
  
  }
  
  length = panjang;
  width  = lebar;
  height = tinggi;
}


//----------------DEBUGGING PROGRAM KE SERIAL MONITOR--------------------//
void showMonitor(){
  Serial.print("Panjang: ");
  Serial.print(length);
  Serial.println(" cm");

  Serial.print("panjang+errorLength: ");
  Serial.print(length+errorLength);
  
 
  Serial.print("Lebar: ");
  Serial.print(width);
  Serial.println(" cm");
 
  Serial.print("Tinggi: ");
  Serial.print(height);
  Serial.println(" cm");
 
  Serial.print("Berat: ");
  Serial.print((weight >= 1000)? weight / 1000 : weight);
  Serial.println(" KG");
  
  Serial.print("Berat: ");
  Serial.print(weight);
  Serial.println(" gram");

  Serial.print("trigger: ");
  Serial.println(trigger);

  Serial.print("runObject: ");
  Serial.println(runObject);
  
}

//---------MENGAMBIL VALUE DARI SENSOR LOADCELL UNTUK ANIMASI AWAL-------------//
float getWeight(){
 
  unsigned long tmr = millis();
  static unsigned long saveTmr=0;
  static int objek = 1;//
  const int flagWeight=0;
  static bool newDataReady = 0;

  if(scale.update()) newDataReady = true;

  if(newDataReady){
    if(millis() > saveTmr + flagWeight && runObject == true  && currentLayer == 0){

      units = scale.getData();
      if (units < 0 ){ units = 0; }
      if(units < objek && trigger == false){ stateRun = 1; }

      if(units > objek ){
        trigger = true;
        flagTr  = 1;
        stateRun = 0;
        state1=0;
      }
      newDataReady = 0;
      return units;
    }
  }
}

//-----------MENGHITUNG WAKTU MUNDUR SLEEP LCD----------//
void timerLCD(){
  static int Run=0;
  int Delay = timerSleep*1000;
  
  static unsigned long saveTmrH=0;
 unsigned long tmr = millis();
  if(timerSleep > 0){ Run = 1; } else{ Run = 0; }
  
  if(stateRun == 1 && Run == 1){
    flagTr=1;
    if((tmr - saveTmrH) > 1000 && state1==0 ){ saveTmrH = tmr; coT++;  }
    lcd.setCursor(18,0);
    lcd.print(coT);
  }
 
  if(coT >= timerSleep && stateRun == 1 && Run == 1){
    
    if(state1==0 && stateRun == 1){state1 = 1;}

  }

  if(state1 == 1){coT = 0; flagTr=0; clearChar(18,0); clearChar(19,0); }
}

//--------BUZZER-----------//
void buzzerRun(bool flag){
  if(flag){ digitalWrite(buzzer,HIGH); }
  else    { digitalWrite(buzzer,LOW); }
}

//---------------ANIMASI PANAH-------------//
void cursorSelect(){
  switch (currentSelect){
    case 1:
      cursorLayer = 0;
      break;
    case 2:
      cursorLayer = 1;
      break;
    case 3:
      cursorLayer = 2;
      break;
    case 4:
      cursorLayer = 3;
      break;
    case 5:
      cursorLayer = 0;
      break;
    case 6:
      cursorLayer = 1;
      break;
    case 7:
      cursorLayer = 2;
      break;
    case 8:
      cursorLayer = 3;
      break;
  };
}

void clearSelect(){
  lcd.setCursor (17,0);
  lcd.print("  ");
  lcd.setCursor (17,1);
  lcd.print("  ");
  lcd.setCursor (17,2);
  lcd.print("  ");
  lcd.setCursor (17,3);
  lcd.print("  ");
}

void clearChar(int charPosition, int line){
  lcd.setCursor (charPosition,line);
  lcd.print(" ");
}

void clearLine(int line){
  for(int i=0;i<20;i++){lcd.setCursor (i,line); lcd.print(" ");}
}

void clearMenu(){
  for(int i=0;i<20;i++){lcd.setCursor (i,0); lcd.print(" ");}
  for(int i=0;i<20;i++){lcd.setCursor (i,1); lcd.print(" ");}
  for(int i=0;i<20;i++){lcd.setCursor (i,2); lcd.print(" ");}
  for(int i=0;i<20;i++){lcd.setCursor (i,3); lcd.print(" ");}
}

//-----------------PROGRAM UNTUK MEMILAH INDIKATOR-------------//
void getIndikator(int conLed,int stateRun,int stateLed){

  unsigned long tmr = millis();
  static unsigned long saveTmr;
  static bool state=false;

  if((tmr - saveTmr) > 1000 && stateRun == 1){
    state = !state;
    digitalWrite(led[conLed],state);
    saveTmr = tmr;
  }
 
  else if(stateRun == 0){ digitalWrite(led[conLed],stateLed); }
  
}
