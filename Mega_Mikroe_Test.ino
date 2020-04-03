/***************************************************************************
  Sample code taken and adapted from various libraries and integrated into one sketch.

  • Adafruit_HTU21DF
  • Adafruit_BME280
  • ClosedCube_OPT3001

 ***************************************************************************/

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ClosedCube_OPT3001.h>
#include "Adafruit_HTU21DF.h"

#define OPT3001_ADDRESS 0x44
#define BME_SCK 13
#define BME_MISO 12
#define BME_MOSI 11
#define BME_CS 10
#define SEALEVELPRESSURE_HPA (1018.7)

ClosedCube_OPT3001 opt3001;
Adafruit_BME280 bme; // I2C
Adafruit_HTU21DF htu = Adafruit_HTU21DF();

unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  while (!Serial); // time to get serial running
  while (Serial.available()) char c = Serial.read();
  Serial.println("\n\nInitializing RAK811");
  Serial1.begin(115200);
  while (!Serial1); // time to get serial running
  while (Serial1.available()) Serial.write(Serial1.read());
  delay(1000);
  Serial1.println("at+version");
  delay(1000);
  while (!Serial1.available()) ;
  while (Serial1.available()) Serial.write(Serial1.read());
  Serial1.println("at+set_config=lora:work_mode:1");
  delay(1000);
  while (!Serial1.available()) ;
  while (Serial1.available()) Serial.write(Serial1.read());
  Serial1.println("at+set_config=lorap2p:869525000:9:0:1:8:20");
  delay(1000);
  while (!Serial1.available()) ;
  while (Serial1.available()) Serial.write(Serial1.read());
  Serial1.println("at+send=lorap2p:DECAFBAD");
  // Private joke, don't ask
  delay(1000);
  while (Serial1.available()) Serial.write(Serial1.read());

  Wire.begin();
  Serial.println("\nI2C Scanner");
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  Serial.print("  |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e. .f");
  for (address = 0; address < 128; address++) {
    if (address % 16 == 0) {
      Serial.print("|\n");
      uint8_t c = address / 16;
      Serial.print(c);
      Serial.print(".|");
    }
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print(" . ");
      nDevices++;
    } else if (error == 4) {
      Serial.print(" ? ");
    } else {
      Serial.print(" x ");
    }
  } Serial.print("|\n\n");
  if (nDevices == 0) Serial.println("No I2C devices found");
  Serial.println("\nOPT3001 Test");
  opt3001.begin(OPT3001_ADDRESS);
  Serial.print("OPT3001 Manufacturer ID");
  Serial.println(opt3001.readManufacturerID());
  Serial.print("OPT3001 Device ID");
  Serial.println(opt3001.readDeviceID());
  configureOPTSensor();
  printResult("High-Limit", opt3001.readHighLimit());
  printResult("Low-Limit", opt3001.readLowLimit());
  Serial.println(F("\nBME280 test"));
  unsigned status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
    Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(100);
  }
  Serial.println("  Init done.");
  Serial.println("\nHTU21D");
  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }
  Serial.println("  Init done.");
  delayTime = 5000;
  Serial.println();
}

void loop() {
  printValues();
  delay(delayTime);
}

void printValues() {
  float temp = bme.readTemperature(), hum = bme.readHumidity(), pressure = bme.readPressure() / 100.0F, alt = bme.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print("\nBME280\n  Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("  Humidity = ");
  Serial.print(hum);
  Serial.println(" %");
  Serial.print("  Pressure = ");
  Serial.print(pressure);
  Serial.print(" hPa. MSL: ");
  Serial.print(SEALEVELPRESSURE_HPA);
  Serial.println(" hPa.");
  Serial.print("  Approx. Altitude = ");
  Serial.print(alt);
  Serial.println(" m");
  char buff0[128], buff1[256];
  memset(buff0, 0, 128);
  memset(buff1, 0, 256);
  String buff = "T: "+String(temp)+" *C, H: "+String(hum)+"%, P: "+String(pressure)+" hPa, Alt: "+String(alt)+" m";
  buff.toCharArray(buff0, buff.length()+1);
  Serial.println(buff0);
  uint8_t data_length = strlen(buff0), cnt = 0;
  char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  for (int i = 0; i < data_length; ++i) {
  char const byte = buff0[i];
    buff1[cnt++] = hex_chars[(byte & 0xF0) >> 4];
    buff1[cnt++] = hex_chars[(byte & 0x0F) >> 0];
  }
  Serial1.print("at+send=lorap2p:");
  Serial1.println(buff1);

  OPT3001 result = opt3001.readResult();
  printResult("OPT3001", result);

  temp = htu.readTemperature();
  float rel_hum = htu.readHumidity();
  Serial.print("HTU21D\n  Temperature: "); Serial.print(temp); Serial.println(" *C");
  Serial.print("  Humidity: "); Serial.print(rel_hum); Serial.println(" %");
}

void configureOPTSensor() {
  OPT3001_Config newConfig;
  newConfig.RangeNumber = B1100;
  newConfig.ConvertionTime = B0;
  newConfig.Latch = B1;
  newConfig.ModeOfConversionOperation = B11;
  OPT3001_ErrorCode errorConfig = opt3001.writeConfig(newConfig);
  if (errorConfig != NO_ERROR)
    printError("OPT3001 configuration", errorConfig);
  else {
    OPT3001_Config sensorConfig = opt3001.readConfig();
    Serial.println("OPT3001 Current Config:");
    Serial.println("------------------------------");
    Serial.print("Conversion ready (R):");
    Serial.println(sensorConfig.ConversionReady, HEX);
    Serial.print("Conversion time (R/W):");
    Serial.println(sensorConfig.ConvertionTime, HEX);
    Serial.print("Fault count field (R/W):");
    Serial.println(sensorConfig.FaultCount, HEX);
    Serial.print("Flag high field (R-only):");
    Serial.println(sensorConfig.FlagHigh, HEX);
    Serial.print("Flag low field (R-only):");
    Serial.println(sensorConfig.FlagLow, HEX);
    Serial.print("Latch field (R/W):");
    Serial.println(sensorConfig.Latch, HEX);
    Serial.print("Mask exponent field (R/W):");
    Serial.println(sensorConfig.MaskExponent, HEX);
    Serial.print("Mode of conversion operation (R/W):");
    Serial.println(sensorConfig.ModeOfConversionOperation, HEX);
    Serial.print("Polarity field (R/W):");
    Serial.println(sensorConfig.Polarity, HEX);
    Serial.print("Overflow flag (R-only):");
    Serial.println(sensorConfig.OverflowFlag, HEX);
    Serial.print("Range number (R/W):");
    Serial.println(sensorConfig.RangeNumber, HEX);
    Serial.println("------------------------------");
  }
}
void printResult(String text, OPT3001 result) {
  if (result.error == NO_ERROR) {
    Serial.println(text);
    Serial.print("  ");
    Serial.print(result.lux);
    Serial.println(" lux");
  } else {
    printError(text, result.error);
  }
}

void printError(String text, OPT3001_ErrorCode error) {
  Serial.print(text);
  Serial.print(": [ERROR] Code #");
  Serial.println(error);
}
