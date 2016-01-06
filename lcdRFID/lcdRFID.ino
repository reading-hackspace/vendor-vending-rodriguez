/*
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 *
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 *
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC (that is: a RFID
 * Tag or Card) using a MFRC522 based RFID Reader on the Arduino SPI interface.
 *
 * When the Arduino and the MFRC522 module are connected (see the pin layout
 * below), load this sketch into Arduino IDE then verify/compile and upload it.
 * To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M).
 * When you present a PICC (that is: a RFID Tag or Card) at reading distance
 * of the MFRC522 Reader/PCD, the serial output will show the ID/UID, type and
 * any data blocks it can read. Note: you may see "Timeout in communication"
 * messages when removing the PICC from reading distance too early.
 *
 * If your reader supports it, this sketch/program will read all the PICCs
 * presented (that is: multiple tag reading). So if you stack two or more
 * PICCs on top of each other and present them to the reader, it will first
 * output all details of the first and then the next PICC. Note that this
 * may take some time as all data blocks are dumped, so keep the PICCs at
 * reading distance until complete.
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

 /*
   LiquidCrystal Library - Hello World

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch prints "Hello World!" to the LCD
  and shows the time.

   The circuit:
  * LCD RS pin (4) to digital pin 6
  * LCD Enable pin (6) to digital pin 5
  * LCD D4 (11) pin to analogue pin 2
  * LCD D5 (12) pin to analogue pin 3
  * LCD D6 (13) pin to analogue pin 4
  * LCD D7 (14) pin to analogue pin 5
  * LCD R/W (5) pin to ground
  * LCD VSS (1) pin to ground
  * LCD VCC (2) pin to 5V
  * 10K resistor:
  * ends to +5V and ground
  * wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystal
  */

#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance
LiquidCrystal lcd(6, 5, A5, A4, A3, A2);
struct {
    byte		size;			// Number of bytes in the UID. 4, 7 or 10.
    byte		uidByte[10];
    byte		sak;			// The SAK (Select acknowledge) byte returned from the PICC after successful selection.
} lastUid = { 0 };

// HACK: PICC_IsNewCardPresent returns true the first time after reading the card serial. 
//       Should be fixed in the library, but we can easily hack around it here
int NewCardFudge = 1;

void dump_byte_array(byte *buffer, byte bufferSize);

void setup()
{
    Serial.begin(115200);		// Initialize serial communications with the PC
    while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

    SPI.begin();			// Init SPI bus
    mfrc522.PCD_Init();		// Init MFRC522

    // set up the LCD's number of columns and rows:
    lcd.begin(20, 4);
    lcd.clear();
    lcd.print("Hello");
}

void loop()
{
    // when characters arrive over the serial port...
    if (Serial.available())
    {
        // clear the screen
        lcd.clear();

        // read all the available characters
        while (Serial.available() > 0)
        {
            // display each character to the LCD
            lcd.print(Serial.readString());
        }
        Serial.println("OK");
    }

    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent())
    {
        if (mfrc522.PICC_ReadCardSerial())
        {
            bool bNewId = false;

            if (mfrc522.uid.size == lastUid.size)
            {
                for (int i = 0; i < lastUid.size; i++)
                {
                    if (mfrc522.uid.uidByte[i] != lastUid.uidByte[i])
                    {
                        bNewId = true;
                        break;
                    }
                }
            } else {
                bNewId = true;
            }

            if (bNewId)
            {
                for (int i = 0; i < mfrc522.uid.size; i++) {
                    lastUid.uidByte[i] = mfrc522.uid.uidByte[i];
                }
                lastUid.size = mfrc522.uid.size;

                // Dump debug info about the card; PICC_HaltA() is automatically called
                Serial.print(F("Card:"));
                dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
                Serial.println();
            }

            NewCardFudge = 0;
        } else {
            lastUid.size = 0;
        }
    }
    else
    {
        if (NewCardFudge > 0) {
            lastUid.size = 0;
        } else {
            NewCardFudge++;
        }
    }
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize)
{
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

