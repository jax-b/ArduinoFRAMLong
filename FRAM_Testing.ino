// FRAM Setup
#include <Wire.h>
#include "Adafruit_FRAM_I2C.h"
#define FRAMLONGPTRLOC 0x00
Adafruit_FRAM_I2C fram = Adafruit_FRAM_I2C();
bool framAvalible = false;
uint16_t framLongPTR;
long testingValue = -800000L;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Press the Any Key to continue");
  while (!Serial.available());
  
  if (fram.begin()) {  // you can stick the new i2c addr in here, e.g. begin(0x51);
    Serial.println("Found I2C FRAM");
    framAvalible = true;
    // Grab where our long is in the FRAM
    framLongPTR = (fram.read8(FRAMLONGPTR) << 8 ) | fram.read8(FRAMLONGPTR + 1) ;
    // For testing Write it out
    Serial.print("long is at: ");
    Serial.println(framLongPTR, HEX);
    // Read out long from FRAM
    long framlongvalue = readLongFromMem(framLongPTR);
    Serial.print("Long Value Stored in FRAM is at: ");
    Serial.println(framLongPTR, HEX);
    Serial.print("And its value is: ");
    Serial.println(framLongPTR);
  } else {
    Serial.println("I2C FRAM not identified ... check your connections?\r\n");
    Serial.println("Will continue in case this processor doesn't support repeated start\r\n");
    framAvalible = false;
  }
  Serial.print("Writeing long to memory with a value of: ");
  Serial.println(testingValue);
  writeLongToMem(framLongPTR, testingValue);
  Serial.println("Reading Memory Contents");

  uint8_t value;
  for (uint16_t a = 0; a < 10; a++) {
    value = fram.read8(a);
    if ((a % 32) == 0) {
      Serial.print("\n 0x"); Serial.print(a, HEX); Serial.print(": ");
    }
    Serial.print("0x");
    if (value < 0x1)
      Serial.print('0');
    Serial.print(value, HEX); Serial.print(" ");
  }
  Serial.println();
  
  long memlong = readLongFromMem(framLongPTR);
  Serial.print("Reading Same Long From Mem: ");
  Serial.println(memlong);
}

void loop() {
  // put your main code here, to run repeatedly:

}

long readLongFromMem(uint16_t address) {
  // Read Each byte from ram
  unsigned bitset1 = fram.read8(address);
  unsigned bitset2 = fram.read8(address + 1);
  unsigned bitset3 = fram.read8(address + 2);
  unsigned bitset4 = fram.read8(address+3);
  // for clarty in testing print out each byte we extracted
  Serial.print("0x");
  Serial.print(bitset1, HEX);
  Serial.print(" 0x");
  Serial.print(bitset2, HEX);
  Serial.print(" 0x");
  Serial.print(bitset3, HEX);
  Serial.print(" 0x");
  Serial.println(bitset4, HEX);
  // Assemble bytes into a long, Shifting each byteset over to and oring them
  // for example lets say we have a long of -800000
  // that is made up of:
  //  1    2    3    4
  // 0xFF 0xF3 0x35 0x00
  // byte 1 becomes 0xFF000000 by shifting it over 24 so on an so forth
  // then its anded together
  // 0xFF000000
  //   0xF30000
  //     0x3500
  //       0x00
  // ----------
  // 0xFFF33500
  //which is then stored into a long variable 
  long totalbytes = (bitset1 << 24) | (bitset2 << 16) | (bitset3 << 8) | (bitset4);
  return totalbytes;
}
void writeLongToMem(uint16_t address, long value) {
  // Disassemble a long into individual bytes
  // for example lets say we have a long of -800000
  // this is sorred as 0xFFF33500
  // We and out each byte using a mask then shift out the trailling zeros
  // for the first byte this would look like
  // 0xFFF33500 & 0xFF000000 = 0xFF000000
  // 0xFF000000 >> 24 = 0xFF
  // for the third byte this would look like
  // 0xFFF33500 & 0xFF00 = 0x3500
  // 0x3500 >> 8 = 0x35
  unsigned bitset1 = (value & 0xFF000000) >> 24;
  unsigned bitset2 = (value & 0xFF0000) >> 16;
  unsigned bitset3 = (value & 0xFF00) >> 8;
  unsigned bitset4 = (value & 0xFF) ;
  // for clarty in testing print out each byte we extracted
  Serial.print("0x");
  Serial.print(bitset1, HEX);
  Serial.print(" 0x");
  Serial.print(bitset2, HEX);
  Serial.print(" 0x");
  Serial.print(bitset3, HEX);
  Serial.print(" 0x");
  Serial.println(bitset4, HEX);
  // Write each byte we exracted to our FRAM
  fram.write8(address, bitset1);
  fram.write8(address + 1, bitset2);
  fram.write8(address + 2, bitset3);
  fram.write8(address + 3, bitset4);
}
