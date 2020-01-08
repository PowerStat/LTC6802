/**
 * Copyright 2017, 2019 Dipl.-Inform. Kai Hofmann
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <LTC6802.h>


/**
 * Chip 1 SPI bus address.
 */
static const byte address1 = 0x80;

/**
 * Chip select pin.
 */
static const byte csPin1 = 10;

/**
 * Chip 1 LTC6802 object.
 */
static LTC6802 chip1 = LTC6802(address1, csPin1);


/**
 * Arduino setup.
 */
void setup()
 {
  Serial.begin(9600);
  LTC6802::initSPI();      // Init SPI bus
  chip1.cfgRead();         // Read configuration from chip
  chip1.cfgSetCDC(1);      // Measure mode 13ms
  chip1.cfgSetMCI(0x0fff); // Disable interrupts
  chip1.cfgWrite(false);   // Write configuration back to chip
  Serial.println("Initialized chip");
  delay(1000);
 }


/**
 * Arduino main loop.
 */
void loop()
 {
  chip1.cfgWrite(false);          // Write configuration back to chip, because chip resets these every 2.5s when nothing happens on SPI
  chip1.temperatureMeasure();     // Measure temperatures on chip
  chip1.temperatureRead();        // Read temperatures from chip
  chip1.temperatureDebugOutput(); // Send temperatures to serial
  chip1.cellsMeasure();           // Measure cell voltages on chip
  chip1.cellsRead();              // Read cell voltages from chip
  chip1.cellsDebugOutput();       // Send cell voltages to serial
  delay(3000);
 }
