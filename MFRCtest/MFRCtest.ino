/*
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 * Sample program showing how to read data from a PICC using a MFRC522 reader on the Arduino SPI interface.
 *----------------------------------------------------------------------------- empty_skull 
 * Aggiunti pin per arduino Mega
 * add pin configuration for arduino mega
 * https://mac86project.altervista.org/
 ----------------------------------------------------------------------------- Nicola Coppola
 * Pin layout should be as follows:
 * Signal     Pin              Pin               Pin
 *            Arduino Uno      Arduino Mega      MFRC522 board
 * ------------------------------------------------------------
 * Reset      9                5                 RST
 * SPI SS     10               53                SDA
 * SPI MOSI   11               51                MOSI
 * SPI MISO   12               50                MISO
 * SPI SCK    13               52                SCK
 *
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#define IRQ_PIN 8


#define SS_PIN 10
#define RST_PIN 9


MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance.

unsigned long t = 0;
int i = 0;


void setup() {
  Serial.begin(115200); // Initialize serial communications with the PC
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
  t = millis();
  pinMode(IRQ_PIN, INPUT_PULLUP);
}

void loop() {
  // Look for new cards
  //if ( ! mfrc522.PICC_IsNewCardPresent()) {
  //  return;
  //}

  // Select one of the cards
  //if ( ! mfrc522.PICC_ReadCardSerial()) {
  //  return;
  //}
  i++;
  
  boolean a = mfrc522.PICC_IsNewCardPresent();
  boolean b = mfrc522.PICC_ReadCardSerial();
  int c = 2+digitalRead(IRQ_PIN);
  //Serial.println(i);
  //Serial.print(F("Card UID:"));
  //dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  //Serial.println();
  Serial.print(a);
  Serial.print(", ");
  Serial.print(b);
  Serial.print(", ");
  dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
  Serial.println("");
  delay(1000);
  

  //RESULTS:
  /*
   * First:
   * PICC_IsNewCarPresent() and PICC_ReadCardSerial() must always be called together.
   * Only calling ReadCard will not yield a reading.
   * 
   * With rfid chip present on the reader the results of PICC_IsNewCarPresent() and PICC_ReadCardSerial
   * alternate between (1,1) and (0,0).
   * So, to see if a card is present, two consecutive readings must be performed.
   * If one of those two reading returns (1,1) a card is present. 
   * If both readings are (0,0), no card is present.
   * 
   * This seems to be robust and two readings should be sufficient.
   * To increase error tolerance, one could perform more than two readings and do an OR
   * statement to see if any of the readings returned (1,1)
   * 
   * 
   * Note: The value in uid.uidByte is not cleared when the card is removed. After card removal this variable holds the id
   * of the last card that was read.
   */
  
  //delay(1000);
  
  //Serial.println(digitalRead(8));
  //delay(1);
  // Dump debug info about the card. PICC_HaltA() is automatically called.
  //mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
