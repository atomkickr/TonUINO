/*
 * MP3 Player with RFID card album identifiers
 * kids-enabled minimalistic functionality 
 * --------------------------------------------
 * by S. Loth 08 Nov 2019 (rev.0)
 * 
 * Pin connections for Arduino Uno
 * 
 * MFRC522 rfid card reader
 * Signal     Pin              
 * ---------------------------- 
 * Reset       9
 * SPI SS     10
 * SPI MOSI   11
 * SPI MISO   12
 * SPI SCK    13
 * 
 * 
 */

#include <SPI.h>
#include <MFRC522.h>

//initialize RFID reader module
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.


// Initialize global variables
struct tagObject {
  boolean card_present;
  byte card_history;
  byte memory_length;
  byte uid[7];
  int uid_length;
};
tagObject tag;

byte state; //Values: 0:idle, 1:new card found, 2:card removed



void setup() {
  // initialize tag object
  tag.card_present = 0;
  tag.card_history = 0;
  tag.memory_length = 4; //number of consecutive readings that must be 0 to determine absence of card
  for (byte i = 0; i < sizeof(tag.uid); i++) {
    tag.uid[i]=0;
  }
  tag.uid_length = 0;

  state = 0;

  //Start rfid (mfrc) module
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  
  //Begin serial communication
  Serial.begin(115200);
  Serial.println("MP3 Player for kids initializing...");
}

void loop() {
  checkForCard();

  //State machine: worker procedure
  Serial.print("State is: ");
  Serial.print(state);
  Serial.print(" : ");
  switch (state){
    case 0:
      //idling nothing new
      Serial.println("I am idling, nothing new");
      break;
    case 1:
      Serial.println("New card detected.");
      updateUid(mfrc522.uid.uidByte, mfrc522.uid.size);

      state = 0; //reset state
      break;
    case 2:
      Serial.println("Card was removed.");
      
      state = 0; //reset state
      break;
    default:
      Serial.println("A state happened that I do not know.");

      state = 0; //reset state
      break;
    }
  
  // put your main code here, to run repeatedly:
  Serial.print(tag.card_present);
  Serial.print(", ");
  Serial.print(tag.uid_length);
  Serial.print(", ");
  Serial.println(uidString(tag));
  delay(1000);
}

String uidString(tagObject ct){
  byte uid_length = sizeof(ct.uid);
  String strout;
  for (byte i = 0; i < uid_length; i++) {
    strout += String(ct.uid[i], HEX);
    strout +=String(" ");
  }
  return strout;
}

void checkForCard(){
  //This function attempts to read a card and will save the check result
  //to determine if a card is present or not
  boolean old_state = tag.card_present;
  boolean a = mfrc522.PICC_IsNewCardPresent();
  boolean b = mfrc522.PICC_ReadCardSerial();
  /* Serial.print("Current reading ");
  Serial.print(a);
  Serial.print(", ");
  Serial.println(b);
  */
  //If no card is present, b=0, this "if" sequence will have to be run memorylenth times
  //before card_present will become 0
  if(b == 1){
    //a card was found
    
    //check if the card has just been placed on the reader
    if(tag.card_present == 0){
      state = 1; //Change state to "new card found"
    }

    //set card presence variables
    tag.card_history = tag.memory_length-1;
    tag.card_present = 1;
    
  }
  else {
    //no card was detected (this could be spurious reading or because card was actually removed
    if(tag.card_history == 0){
      //ok, several consecutive readings did not find a card. Conclude, it was removed.
      //check if the card has just been removed
      if(tag.card_present == 1){
        state = 2; //change state to "card has been removed"
      }

      //set card presence variables
      tag.card_present = 0;
    }
    else {
      //ok, not enough consecutive readings have been 0, Assume, card may still be there.
      tag.card_history--; 
    }
  }
}

void updateUid(byte *buffer, byte buffer_size){
  Serial.println("Updating uid.");
  tag.uid_length = buffer_size;
  byte reclength = sizeof(tag.uid);
  for (byte j = 0; j < min(buffer_size,reclength); j++) {
    tag.uid[j] = buffer[j];
  }
  // in case uid length changed, ensure that unused bytes are 0
  for(byte j = buffer_size;j < reclength;j++){
    tag.uid[j]=0;
  }
}
