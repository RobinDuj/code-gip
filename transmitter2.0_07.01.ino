#include <SPI.h>            //bibliotheek transmitter
#include <nRF24L01.h>       //bibliotheek transmitter
#include <RF24.h>           //bibliotheek transmitter

int pirPin = 4;                      //bewegingssensor

RF24 radio(9, 10);                   // CE Cip Enable, CSN Chip Select Not        
const byte address[6] = "00001";     //byte adres

int val = 0;                         //waarde pir
bool motionState = false;            //status bewegingssensor false

int geluidssensorPin = 3;

int val2 = 23;
bool geluidState = false;

void setup() {
  pinMode(pirPin, INPUT);            //pirPin is een ingang
  pinMode(geluidssensorPin, INPUT);  //geluidssensor is een ingang
  Serial.begin(9600);                //start seriele connectie
  radio.begin();                     //Start draadloze communicatie
  radio.openWritingPipe(address);    //Het adres waar de data wordt naar verzonden
  radio.setPALevel(RF24_PA_MIN);     //transmitter sterkte Minimum
  radio.stopListening();             //Zet de nRF24L01 als transmitter
}

void loop() {
  val = digitalRead(pirPin);                          //lees pirpin
  val2 = digitalRead(geluidssensorPin);               //lees de geluidssensor
  if(val == HIGH)  {                                  //als pirPin hoog is
   
    const char text[] = "Bewegingssensor is HIGH";    //text is gelijk aan bewegingssensor is HIGH

    radio.write(&text, sizeof(text));                 //verzend het bericht naar de receiver
    Serial.println("signaal verzonden");              //print op de seriele monitor signaal verzonden
   if (motionState == false) {                        //als motionstate laag is
      Serial.println("beweging");                     //print in seriele monito beweging
      motionState = true;                             //maak motionState true    
      delay(100);                                     //wacht 100ms
    }
  }else if(val2 == HIGH){                             //als val2 hoog is
    const char text[] = "geluidssensor is HIGH";      //text is geluidssensor is HIGH
    Serial.println("geluid");                         //print in seriele monitor geluid
    radio.write(&text, sizeof(text));                 //verzend het bericht naar de receiver
    delay(1000);   
  }else{
    const char text[] = "Bewegingssensor is LOW";      //text is bewegingsensor is LOW
    radio.write(&text, sizeof(text));                  //verzend het bericht naar de receiver 
 
  if (motionState == true) {                          //als motionstate true is
      Serial.println("beweging gestopt");             //print in de seriele monitor beweging gestopt
      motionState = false;                            //maak motionState false

    }
  radio.write(&motionState, sizeof(motionState));     //verzend het bericht naar de receiver 
  delay(1000);                                        //wacht 1000ms                              
 }
}
