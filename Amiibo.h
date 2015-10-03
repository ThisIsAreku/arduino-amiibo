/**************************************************************************/
/*! 
    @file     Adafruit_PN532.h
    @author   Adafruit Industries
	@license  BSD (see license.txt)
	

	This is a library for the Adafruit PN532 NFC/RFID breakout boards
	This library works with the Adafruit NFC breakout 
	----> https://www.adafruit.com/products/364
	
	Check out the links above for our tutorials and wiring diagrams 
  These chips use SPI or I2C to communicate.
	
	Adafruit invests time and resources providing this open source code, 
	please support Adafruit and open-source hardware by purchasing 
	products from Adafruit!

	@section  HISTORY

  v2.0  - Refactored to add I2C support from Adafruit_NFCShield_I2C library.

	v1.1  - Added full command list
          - Added 'verbose' mode flag to constructor to toggle debug output
          - Changed readPassiveTargetID() to return variable length values
	
*/
/**************************************************************************/

#ifndef AMIIBO_H
#define AMIIBO_H

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Adafruit_PN532.h"

#define AMIIBO_PAGE_COUNT (135)

struct Amiibo_Info {
  uint8_t game_serie;
  uint8_t character;
  uint8_t character_variation;
  uint8_t format;

  uint16_t amiibo_id;
  uint8_t amiibo_serie;
  uint8_t static_byte;
};

class Amiibo
{
  public:
    Amiibo(Adafruit_PN532);
    ~Amiibo();

    void begin();
    bool read();
    bool readFull();
    bool readInfo(Amiibo_Info*);
    void clearCache();
    void printCache();

  private:
    void readAmiibo();
    bool readPage(uint8_t);
    uint8_t getByteAt(uint8_t, uint8_t);


    Adafruit_PN532 nfc;
    uint8_t* amiiboDataCache;
    bool* amiiboDataCacheStatus;
};

#endif
