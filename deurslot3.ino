#include <SPI.h>          //bibliotheek SPI interface
#include <MFRC522.h>      //bibliotheek rfid lezer
 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

const int switchReed = 7;           //reed contact zit op pin 7
const int RELAY_PIN = 6;            //relay zit op pin 6
const int buttonPin = A0;
int ledRood = 3;
int ledGroen = 2;
int buttonState = 0;

void setup(){
  pinMode(switchReed, INPUT);
  pinMode(buttonPin, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(ledRood, OUTPUT);
  pinMode(ledGroen, OUTPUT);
  Serial.begin(9600);             // Initiate a serial communication
  SPI.begin();                    // Initiate  SPI bus
  mfrc522.PCD_Init();             // Initiate MFRC522
  Serial.println("Hou de kaart tegen de lezer...");
  Serial.println();

}
void loop(){                      // Look for new cards
  buttonState = digitalRead(buttonPin);
  digitalWrite(ledRood, HIGH);
  digitalWrite(ledGroen, LOW);
    
  if(buttonState == HIGH){
   Serial.println();
   Serial.println("deur open van binnen");
   digitalWrite(ledRood, LOW);
   digitalWrite(ledGroen, HIGH);  
   Serial.println("Toegang");
   digitalWrite(RELAY_PIN, HIGH);    //maakt de deur open
   Serial.println("Deur open");
   digitalRead(switchReed); 
   delay(100);
     while(digitalRead(switchReed) != LOW);{
        digitalWrite(ledGroen, HIGH);      
      }

    if(digitalRead(switchReed) == LOW){
      Serial.println("Controleer de deur");
      delay(500);
    }
  
    Serial.println("Deur geopend");
    delay(1000);
    digitalWrite(RELAY_PIN, LOW);       //deurslot dicht
    while(digitalRead(switchReed) != HIGH);{
      
    }
    Serial.println();
    Serial.println("Deur gesloten");
    delay(500);
  }

  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }

  Serial.print("UID tag :");  //Show UID on serial monitor
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  digitalWrite(ledRood, LOW);
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();

  if (content.substring(1) == "AA AC 12 BE") {                //change here the UID of the card/cards that you want to give access
    digitalWrite(ledGroen, HIGH);
    Serial.println("Toegang");
    digitalWrite(RELAY_PIN, HIGH);    //maakt de deur open
    Serial.println("Deur open");
    digitalRead(switchReed); 
    delay(100);
       while(digitalRead(switchReed) != LOW);{
        digitalWrite(ledGroen, HIGH);      
       }

    Serial.println("Deur geopend");
    delay(1000);
    digitalWrite(RELAY_PIN, LOW);       //deurslot dicht
    while(digitalRead(switchReed) != HIGH);{
      
    }
    Serial.println();
    Serial.println("Deur gesloten");
    delay(500);
     
  }else{
    Serial.println(" Geen Toegang");
    Serial.println();
    Serial.println("Probeer opnieuw");
    digitalWrite(ledRood, LOW);
    delay(100);
    digitalWrite(ledRood, HIGH);
    delay(100);
    digitalWrite(ledRood, LOW);
    delay(100);
    digitalWrite(ledRood, HIGH);
    delay(100);
    digitalWrite(ledRood, LOW);
    delay(100);
    digitalWrite(ledRood, HIGH);
    delay(3000);
  }
} 
