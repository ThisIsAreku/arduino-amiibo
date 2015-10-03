#include <Wire.h>
#include <SPI.h>

#include <Adafruit_PN532.h>
#include <Amiibo.h>

// If using the breakout with SPI, define the pins for SPI communication.
#define PN532_SCK  (2)
#define PN532_MOSI (3)
#define PN532_SS   (4)
#define PN532_MISO (5)

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (2)
#define PN532_RESET (3)  // Not connected by default on the NFC Shield

// Uncomment just _one_ line below depending on how your breakout or shield
// is connected to the Arduino:

// Use this line for a breakout with a software SPI connection (recommended):
//Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Use this line for a breakout with a hardware SPI connection.  Note that
// the PN532 SCK, MOSI, and MISO pins need to be connected to the Arduino's
// hardware SPI SCK, MOSI, and MISO pins.  On an Arduino Uno these are
// SCK = 13, MOSI = 11, MISO = 12.  The SS line can be any digital IO pin.
Adafruit_PN532 nfc(PN532_SS);
Amiibo amiibo(nfc);
Amiibo_Info info;

// Or use this line for a breakout or shield with an I2C connection:
//Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Helper function
void print_hex(int v, int num_places)
{
  int mask=0, n, num_nibbles, digit;
  for (n=1; n<=num_places; n++) {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask; // truncate v to specified number of places

  num_nibbles = num_places / 4;
  if ((num_places % 4) != 0) {
    ++num_nibbles;
  }

  do {
    digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
    Serial.print(digit, HEX);
  } while(--num_nibbles);

   Serial.println();
}

void setup(void)
{
  Serial.begin(115200);
  amiibo.begin();

  Serial.println(F("Place an amiibo on the reader"));
}

void loop(void)
{
  int oldId = info.amiibo_id;
  amiibo.clearCache();
  if (!amiibo.read()) {
    Serial.println(F("It may not be an amiibo, or nothing found"));
    delay(1000);
    
    return;
  }

  if (!amiibo.readInfo(&info)) {
    Serial.println(F("Unable to read amiibo informations"));
    delay(1000);

    return;
  }

  if (oldId != info.amiibo_id) {
    Serial.println(F("New Amiibo detected !"));
    Serial.print(F("Game serie: ")); print_hex(info.game_serie, 8);
    Serial.print(F("Character: ")); print_hex(info.character, 8);
    Serial.print(F("Character variation: ")); print_hex(info.character_variation, 8);
    Serial.print(F("Format: ")); print_hex(info.format, 8);
    Serial.print(F("Amiibo ID: ")); print_hex(info.amiibo_id, 16);
    Serial.print(F("Amiibo serie: ")); print_hex(info.amiibo_serie, 8);
    Serial.print(F("Static byte: ")); print_hex(info.static_byte, 8);
    Serial.println(F("-------------------------"));

    Serial.println(F("Reading memory..."));
    long b = millis();
    amiibo.readFull();
    b = millis() - b;
    Serial.print(F("Took ")); Serial.print(b); Serial.println("ms");
    
    Serial.println(F("Memory dump: "));
    amiibo.printCache();
  }

  delay(1000);
}
