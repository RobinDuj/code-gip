#include<Keypad.h>               //bibliotheek keypad
#include<Wire.h>                 //bibliotheek I2C
#include<LiquidCrystal_I2C.h>    //bibliotheek lcd
#include<EEPROM.h>               //bibliotheek opslag geheugen
#include<SoftwareSerial.h>       //bibliotheek seriele monitor
#include <SPI.h>                 //bibliotheek SPI interface zender/ontvanger module
#include <nRF24L01.h>            //bibliotheek nRF24L01 module
#include <RF24.h>                //bibliotheek nRF24L01 communicatie

RF24 radio (9, 10);              //CE, CSN pinnen zender/ontvanger module

LiquidCrystal_I2C lcd(0x27,20,4); //pinnen van lcd

const byte address[6] = "00001";  //adres van zender/ontvanger module
boolean motion_state = 0;         //boolean van bewegingssensor

char wachtwoord[4];                             //variabele met wachtwoord
char oud_wachtwoord[4], nieuw_wachtwoord[4];    //variabele met het oude en nieuwe wachtwoord

int i=0;
int w=0;
int a=0;

int timerWaarde = 60; //timer van 60 seconden
int cijfer1 = 0;
int cijfer2 = 0;
//uint32_t lastTime;        //32 bit integer timer

int relais_pin = 8;     //relais op pin 10
int switchPin = 7;      //test drukknop
int sirenePin = 6;      //sirene

int alarmActief = 0;    //alarm aan/uit
int alarmStatus = 0;    //status alarm
int sireneState = LOW;  // ledState used to set the LED
int switchState = LOW;  //test drukknop

char key_pressed=0;
const byte rows = 4;         //4 kolommen
const byte columns = 4;      //4 rijen
const long interval = 1000;  //

char hexaKeys[rows][columns] = {

{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}

};

byte row_pins[rows] = {A3,A2, A1, A0};        //rijen verbonden met pinnen:
byte column_pins[columns] = {5, 4, 3, 2};     //kolommen verbonden met pinnen:

Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);


void setup() {
  Serial.begin(9600);

  lcd.init();                               //lcd opstarten
  lcd.backlight();                          //lcd backlight aanpassen
  pinMode(relais_pin, OUTPUT);              //relaispin is een uitgang
  pinMode(sirenePin, OUTPUT);               //sirene is een uitgang
  pinMode(switchPin, INPUT);                //test drukknop
  
  lcd.setCursor(0,1);                       //zet de cursor op 0,1
  lcd.print("Home Security");               //print op lcd: Slot
  
  delay(2000);                              //wacht 2000ms
  
  lcd.clear();                              //clear het display
  lcd.print("Enter Wachtwoord");            //print op het display Enter Wachtwoord
  lcd.setCursor(0,1);                       //zet cursor op 0,1

  radio.begin();                       //start de communicatie
  radio.openReadingPipe(0, address);   //Adres instellen waarop de data binnenkomt
  radio.setPALevel(RF24_PA_MIN);       //sterkte signaal MIN/MAX
  radio.startListening();              //begin met lezen van data 
  }

void loop() {
  
  oudwachtwoord();                          //ga naar de functie oudwachtwoord
  
  alarmStatus = 0;                          //zet status alarm 0
  switchState = digitalRead(switchPin);     //lees de drukknop
  key_pressed = keypad_key.getKey();        //char key_pressed = lees de keypad

  if(radio.available()){                    //als er een verbinding is dan
    Serial.println("signaal");              //print in seriele monitor
    char text[32] = "";                     //Sla de binnenkomende data op
    radio.read(&text, sizeof(text));        //Lees de data
    radio.read(&motion_state, sizeof(motion_state));    //Lees de status van de bewegingssensor
    if(motion_state == HIGH){
      lcd.clear();
      lcd.print("Alarm");
      Serial.println("ontvangen");
      delay(200);        
      while(alarmStatus = 1){
        alarm();                              //ga naar de alarm functie
      }
    }
  }

  
  if((key_pressed)&&(alarmStatus==0)){        //als er een toets is ingedrukt
      
    wachtwoord[i++]=key_pressed;              //i+1
    lcd.print(key_pressed);                   //print op lcg key_pressed
  }
  if((i==4)&&(alarmStatus==0)){               //als i gelijk is aan 4
    delay(200);                               //wacht 200ms

    for(int j=0;j<4;j++)
    
      oud_wachtwoord[j]=EEPROM.read(j);       //lees het geheugen

      if(!(strncmp(wachtwoord, oud_wachtwoord,4))){   //vergelijk het wachtwoord met het oud wachtwoord
       
       alarmStatus = 0;                               //zet de alarmstatus op 0

       while(a==0){                                   //wanneer a is gelijk aan 0
        lcd.clear();                                  //clear het lcd
        lcd.print("Wachtwoord goed");                 //print op het lcd wachtwoord goed
        lcd.setCursor(0,1);                           //zet de cursor op column 0 en rij 1       
        lcd.print("A = change key");                  //print op het lcd A= change key
        delay(1500);                                  //wacht 1500ms
        lcd.clear();                                  //clear het lcd
        lcd.setCursor(0,1);                           //zet de cursor op column 0 en rij 1          
        lcd.print("B = arm");                         //print op het lcd B = arm
        delay(1500);                                  //wacht 1500ms
        lcd.clear();                                  //clear het lcd
        lcd.setCursor(0,1);                           //zet de cursor op column 0 en rij 1   
        lcd.print("C = disarm");                      //print op het lcd c = disarm
        delay(1500);                                  //wacht 1500ms
        lcd.clear();                                  //clear het lcd
        lcd.setCursor(0,1);                           //zet de cursor op column 0 en rij 1  
        lcd.print("D = sirene");                      //print op het lcd d = sirene
        delay(1500);                                  //wacht 1500ms
        a++;                                          //a +1
       }
       
       lcd.clear();                                   //clear het lcd
       lcd.print("Home Security");                    //print op het lcd Home Security
       key_pressed = keypad_key.getKey();             // char key_pressed = lees de keypad
         if(key_pressed =='A'){                       //als A is ingedrukt
          change();                                   //ga naar de functie change
          
         }else if(key_pressed == 'B'){                //als knop b is ingedrukt
          lcd.clear();                                //clear het lcd
          lcd.print("Alarm aan");                     //print op het lcd alarm aan
          alarmStatus = 1;                            //zet alarmstatus op 1
          alarm();                                    //ga naar de functie alarm
          delay(200);                                 //wacht 200ms
 
         }else if(key_pressed == 'C'){                //als de knop c is ingedrukt
          lcd.clear();                                //clear het lcd
          lcd.print("Alarm uit");                     //print op het lcd alarm uit
          alarmStatus = 0;                            //zet de alarmstatus op 0
          delay(200);                                 //wacht 200ms
         }else if(key_pressed == 'D'){                //als de knop d is ingedrukt
          lcd.clear();                                //clear het lcd
          lcd.print("Sirene");                        //print op het scherm sirene
          alarmStatus = 1;                            //zet alarmstatus op 1
          delay(200);                                 //wacht 200ms          
         }
        }else{
          digitalWrite(relais_pin, HIGH);             //maak relais hoog

          if(w==3){                                   //als w de waarde 3 heeft
            lcd.clear();                              //clear het lcd
            lcd.print("geblokkeerd");                 //print op het lcd geblokkeerd
            delay(2000);                              //wacht 2000ms
            lcd.clear();                              //clear het lcd
            alarmStatus=1;                            //maak alarmstatus 1
            
              if(alarmStatus == 1){                   //als alarmstatus 1 is
                lcd.clear();                          //clear het lcd
                lcd.print("Alarm");                   //print op het lcd alarm
                alarm();                              //ga naar de functie alarm
              }
     
          }else if(w==2){                             //als w gelijk is aan 2
            lcd.clear();                              //clear het lcd
            lcd.print("1 kans");                      //print op het lcd 1 kans
            delay(2000);                              //wacht 2000ms
            lcd.clear();                              //clear het lcd
            w++;                                      //w +1
      
          }else if(w==1){                             //als w gelijk is aan 1
            lcd.clear();                              //clear het lcd
            lcd.print("2 kansen");                    //print op het lcd 2 kansen
            delay(2000);                              //wacht 2000ms
            lcd.clear();                              //clear het lcd
            w++;                                      //w +1
       
         }else{
            lcd.clear();                           //clear lcd
            lcd.print("Verkeerd");                 //print op lcd verkeerd wachtwoord
            delay(200);                            //wacht 200ms
            w++;                                   //w +1
         }
 
      delay(2000);                         //wacht 2000ms
      lcd.clear();                         //clear lcd
      lcd.print("Enter Wachtwoord");       //print Enter Wachtwoord
      lcd.setCursor(0,1);                  //zet cursor lcd op 0,1
      i=0;                                 //zet i is 0

      }
    }
}
  
  void alarm(){

   if((cijfer1 > 0)&&(cijfer2==0)){
    lcd.setCursor(6,1);
    lcd.print(timerWaarde);
    delay(100);
    cijfer1 = cijfer1 -1;
   }else if((cijfer1 > 0)&&(cijfer2 > 0)){
    lcd.setCursor(6,1);
    lcd.print(timerWaarde);
    delay(100);
    cijfer2 = cijfer2 -1;       
   }else if((cijfer1 = 0)&&(cijfer2 = 0)){
   
   }else if(key_pressed =='A'){
    
   }
                               
      //timerWaarde--;                  //als timerwaarde groter dan 0 is
      //uint32_t now = millis();        //32bit integer in milliseconden
      //if (now - lastTime > 1000) {    //als now min de lasttime groter dan 1000 is 
        //lastTime += 1000;             //
        //if (timerWaarde > 0){         //als timerwaarde groter dan 0 is
          //timerWaarde--;                //timerwaarde -1
          //lcd.setCursor(0,1);           //zet de cursor op column 0 en rij 1
          //lcd.print("Tijd:");           //print op het lcd Tijd:
          //lcd.setCursor(6,1);           //zet de cursor op column 6 en rij 1     
          //lcd.print(timerWaarde);       //print op het lcd de timerwaarde
      //}
    //}      
  
   while(digitalRead(alarmStatus) != 1);{       //wanneer alarmstatus 1 is
    digitalWrite(sirenePin, HIGH);              //maak de sirene hoog
    delay(500);                                 //wacht 500ms
    digitalWrite(sirenePin, LOW);               //maak de sirene laag
    delay(500);                                 //wacht 500ms
   }
   delay(100);                                  //wacht 100ms
  }


  void change(){
  
    int j=0;                              //j is 0
  
    lcd.clear();                          //clear lcd
    lcd.print("Current Wachtwoord");      //print op lcd Current Wachtwoord 
    lcd.setCursor(0,1);                   //zet cursor lcd op 0,1

    while(j<4){
      char key=keypad_key.getKey();       //lees de keypad ingangen

      if(key){
      
        nieuw_wachtwoord[j++]=key;       //nieuw_wachtwoord+1 is key
        lcd.print(key);                  //print key op display
        }
      key=0;
      }
    delay(500);                           //wacht 500ms

  if ((strncmp(nieuw_wachtwoord, oud_wachtwoord,4))){
    
    lcd.clear();                         //clear lcd
    lcd.print("Verkeerd wachtwoord");    //print op lcd verkeerd wachtwoord
    lcd.setCursor(0,1);                  //zet cursor op 0,1
    lcd.print("Try Again");              //print probeer opnieuw

    delay(1000);                         //wacht 1000ms
    
  }else{
    
    j=0;

    lcd.clear();                        //clear lcd
    lcd.print("Nieuw wachtwoord:");     //nieuw wachtwoord
    lcd.setCursor(0,1);                 //zet cursor lcd op 0,1

    while(j<4){                         //als j groter dan 4 is
      
      char key=keypad_key.getKey();     //lees het keypad
      
      if(key){ 
        
        oud_wachtwoord[j]=key;

        lcd.print(key);                 //print key op lcd

        EEPROM.write(j,key);            //schrijf naar eeprom de key

        j++;                            //j+1
      }
    }
    
    lcd.print(" Done");   //print op lcd wachtwoord verandert

    delay(1000);                         //wacht 1000ms  
  }
  
  lcd.clear();                           //clear lcd
  lcd.print("Enter Wachtwoord");         //print Enter Wachtwoord
  lcd.setCursor(0,1);                    //zet lcd cursor op 0,1

  key_pressed=0;                         //zet key_pressed op 0

  oudwachtwoord();
}

void oudwachtwoord(){

  for(int j=0;j<4;j++)

    EEPROM.write(j, j+49);

  for(int j=0;j<4;j++)

    oud_wachtwoord[j]=EEPROM.read(j);

}
