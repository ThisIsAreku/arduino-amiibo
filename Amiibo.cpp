#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Amiibo.h"
#include "Adafruit_PN532.h"

Amiibo::Amiibo(Adafruit_PN532 nfc) : nfc(nfc)
{
  this->amiiboDataCache = new uint8_t[AMIIBO_PAGE_COUNT*4];
  this->amiiboDataCacheStatus = new bool[AMIIBO_PAGE_COUNT];

  memset(this->amiiboDataCache, 0, AMIIBO_PAGE_COUNT*4);
  memset(this->amiiboDataCacheStatus, false, AMIIBO_PAGE_COUNT);
}

Amiibo::~Amiibo()
{
  delete [] this->amiiboDataCache;
  delete [] this->amiiboDataCacheStatus;
}

void Amiibo::clearCache()
{
  memset(this->amiiboDataCacheStatus, false, AMIIBO_PAGE_COUNT);
}

void Amiibo::begin()
{
  this->nfc.begin();
  uint32_t versiondata = this->nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1);
  }

  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  this->nfc.SAMConfig();
}

bool Amiibo::read()
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = this->nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (!success) {
    return false;
  }

  // Serial.println("Found an ISO14443A card");
  // Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
  // Serial.print("  UID Value: ");
  // for (uint8_t i = 0; i < uidLength; i++) {
  //   if (uid[i] <= 0x0F)
  //     Serial.print(F("0"));
  //   Serial.print(uid[i], HEX);
  // }
  
  // Serial.println();
  
  if (uidLength != 7) {
    return false;
  }

  if (this->getByteAt(22, 3) != 0x02) {
    return false;
  }

  return true;
}

bool Amiibo::readFull()
{
  bool success = true;
  for (uint8_t i = 0; i < AMIIBO_PAGE_COUNT; i++) 
  {
    if (!this->readPage(i)) {
        success = false;
        Serial.print("Cannot read page "); Serial.println(i);
    }
  }

  return success;
}

void Amiibo::printCache()
{
  for (uint8_t i = 0; i < AMIIBO_PAGE_COUNT; i++) {
    switch (i) {
      case 0:
        Serial.println(F("-------- Header data --------"));
      break;
      case 4:
        Serial.println(F("-------- User page START --------"));
      break;
      case 129+1:
        Serial.println(F("-------- User page END --------"));
      break;
      case 131:
        Serial.println(F("-------- Cfg page START --------"));
      break;
      case 132+1:
        Serial.println(F("-------- Cfg page END --------"));
      break;
    }

    Serial.print(this->amiiboDataCacheStatus[i] ? F("+") : F("-"));
    Serial.print(" PAGE ");
    if (i < 10)
    {
      Serial.print("0");
      Serial.print(i);
    }
    else
    {
      Serial.print(i);
    }
    Serial.print(": ");
    
    nfc.PrintHexChar(this->amiiboDataCache+(i*4), 4);
  }
}

bool Amiibo::readPage(uint8_t page)
{
  if (this->amiiboDataCacheStatus[page]) {
    return true;
  }

  if (this->nfc.ntag2xx_ReadPage(page, this->amiiboDataCache + (page * 4))) {
    this->amiiboDataCacheStatus[page] = true;

    return true;
  }

  return false;
}

uint8_t Amiibo::getByteAt(uint8_t page, uint8_t pos)
{
  if (!this->readPage(page)) {
      return -1;
  }

  return this->amiiboDataCache[page*4+pos];
}

bool Amiibo::readInfo(Amiibo_Info *info)
{
  if (!(this->readPage(21) && this->readPage(22))) {
    return false;
  }

  memcpy(info, this->amiiboDataCache+84, 8);

  // reversing endianness on amiibo id
  info->amiibo_id = (info->amiibo_id << 8) | ((info->amiibo_id >> 8) & 0x00ff);

  return info->static_byte == 0x02;
}
