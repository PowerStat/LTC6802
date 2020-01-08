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
#include <SPI.h>


/**
 * SPI settings.
 */
static const SPISettings spiSettings = SPISettings(1000000, MSBFIRST, SPI_MODE3);

/**
 * Write configuration register group.
 */
static const byte WRCFG   = 0x01;

/**
 * Read configuration register group.
 */
static const byte RDCFG   = 0x02;

/**
 * Read cellvoltage register group.
 */
static const byte RDCV    = 0x04;

/**
 * Read flag register group.
 */
static const byte RDFLG   = 0x06;

/**
 * Read temperature register group.
 */
static const byte RDTMP   = 0x08;

/**
 * Start cell voltage A/D conversion and poll status.
 *
 * | 0x00 : all cell voltage inputs
 * | 0x01 : cell 1 only
 * | 0x02 : cell 2 only
 * | 0x03 : cell 3 only
 * | 0x04 : cell 4 only
 * | 0x05 : cell 5 only
 * | 0x06 : cell 6 only
 * | 0x07 : cell 7 only
 * | 0x08 : cell 8 only
 * | 0x09 : cell 9 only
 * | 0x0a : cell 10 only
 * | 0x0b : cell 11 only, if CELL10 bit=0
 * | 0x0c : cell 12 only, if CELL10 bit=0
 * | 0x0e : cell self test 1; all CV=0x555
 * | 0x0f : cell self test 2; all CV=0xaaa
 */
static const byte STCVAD  = 0x10;

/**
 * Start Open-Wire A/D conversion and poll status.
 *
 * | 0x00 : all cell voltage inputs
 * | 0x01 : cell 1 only
 * | 0x02 : cell 2 only
 * | 0x03 : cell 3 only
 * | 0x04 : cell 4 only
 * | 0x05 : cell 5 only
 * | 0x06 : cell 6 only
 * | 0x07 : cell 7 only
 * | 0x08 : cell 8 only
 * | 0x09 : cell 9 only
 * | 0x0a : cell 10 only
 * | 0x0b : cell 11 only, if CELL10 bit=0
 * | 0x0c : cell 12 only, if CELL10 bit=0
 * | 0x0e : cell self test 1; all CV=0x555
 * | 0x0f : cell self test 2; all CV=0xaaa
 */
static const byte STOWAD  = 0x20;

/**
 * Start temperature A/D conversion and poll status.
 *
 * | 0x00 : all temperature inputs
 * | 0x01 : external temp 1 only
 * | 0x02 : external temp 2 only
 * | 0x03 : internal temp only
 * | 0x0e : temp self test 1; all TMP=0x555
 * | 0x0f : temp self test 2; all TMP=0xaaa
 */
static const byte STTMPAD = 0x30;

/**
 * Poll A/D convertr status.
 */
static const byte PLADC   = 0x40;

/**
 * Poll interrupt status.
 */
static const byte PLINT   = 0x50;

/**
 * Start cell voltage A/D conversion and poll status, with discharge permitted.
 *
 * | 0x00 : all cell voltage inputs
 * | 0x01 : cell 1 only
 * | 0x02 : cell 2 only
 * | 0x03 : cell 3 only
 * | 0x04 : cell 4 only
 * | 0x05 : cell 5 only
 * | 0x06 : cell 6 only
 * | 0x07 : cell 7 only
 * | 0x08 : cell 8 only
 * | 0x09 : cell 9 only
 * | 0x0a : cell 10 only
 * | 0x0b : cell 11 only, if CELL10 bit=0
 * | 0x0c : cell 12 only, if CELL10 bit=0
 * | 0x0e : cell self test 1; all CV=0x555
 * | 0x0f : cell self test 2; all CV=0xaaa
 */
static const byte STCDC   = 0x60;

/**
 * Start open-Wire A/D conversions and poll status, with Discharge Permitted.
 *
 * | 0x00 : all cell voltage inputs
 * | 0x01 : cell 1 only
 * | 0x02 : cell 2 only
 * | 0x03 : cell 3 only
 * | 0x04 : cell 4 only
 * | 0x05 : cell 5 only
 * | 0x06 : cell 6 only
 * | 0x07 : cell 7 only
 * | 0x08 : cell 8 only
 * | 0x09 : cell 9 only
 * | 0x0a : cell 10 only
 * | 0x0b : cell 11 only, if CELL10 bit=0
 * | 0x0c : cell 12 only, if CELL10 bit=0
 * | 0x0e : cell self test 1; all CV=0x555
 * | 0x0f : cell self test 2; all CV=0xaaa
 */
static const byte STOWDC  = 0x70;

/**
 * Configuration register 0 watchdog timer bit.
 */
static const byte CFG0_WDT_BIT    = 7;

/**
 * Configuration register 0 GPIO2 bit.
 */
static const byte CFG0_GPIO2_BIT  = 6;

/**
 * Configuration register 0 GPIO1 bit.
 */
static const byte CFG0_GPIO1_BIT  = 5;

/**
 * Configuration register 0 level polling mode bit.
 */
static const byte CFG0_LVLPL_BIT  = 4;

/**
 * Configuration register 0 10-cell mode bit.
 */
static const byte CFG0_CELL10_BIT = 3;

// static const byte CFG0_CDC_BITS   = 0-2;
// static const byte CFG1_DCC_BITS   = 0-7;
// static const byte CFG2_DCC_BITS   = 0-3;
// static const byte CFG2_MCI_BITS   = 4-7;
// static const byte CFG3_MCI_BITS   = 0-7;

/**
 * Configuration register 0 watchdog timer bitmask.
 */
static const byte CFG0_WDT_MSK    = 0x80;

/**
 * Configuration register 0 GPIO2 bitmask.
 */
static const byte CFG0_GPIO2_MSK  = 0x40;

/**
 * Configuration register 0 GPIO1 bitmask.
 */
static const byte CFG0_GPIO1_MSK  = 0x20;

/**
 * Configuration register 0 level polling bitmask.
 */
static const byte CFG0_LVLPL_MSK  = 0x10;

/**
 * Configuration register 0 10-cell mode bitmask.
 */
static const byte CFG0_CELL10_MSK = 0x08;

/**
 * Configuration register 0 comparator duty cycle bitmask.
 */
static const byte CFG0_CDC_MSK    = 0x07;

/**
 * Configuration register 1 discharge cell bitmask.
 */
static const byte CFG1_DCC_MSK    = 0xff;

/**
 * Configuration register 2 discharge cell bitmask.
 */
static const byte CFG2_DCC_MSK    = 0x0f;

/**
 * Configuration register 2 mask cell interrupts bitmask.
 */
static const byte CFG2_MCI_MSK    = 0xf0;

/**
 * Configuration register 3 mask cell interrupts bitmask.
 */
static const byte CFG3_MCI_MSK    = 0xff;


/**
 * Configuration register 0 watchdog timer inverse bitmask.
 */
static const byte CFG0_WDT_INVMSK    = 0x7f;

/**
 * Configuration register 0 GPIO2 inverse bitmask.
 */
static const byte CFG0_GPIO2_INVMSK  = 0xbf;

/**
 * Configuration register 0 GPIO1 inverse bitmask.
 */
static const byte CFG0_GPIO1_INVMSK  = 0xdf;

/**
 * Configuration register 0 level polling mode inverse bitmask.
 */
static const byte CFG0_LVLPL_INVMSK  = 0xef;

/**
 * Configuration register 0 10-cell mode inverse bitmask.
 */
static const byte CFG0_CELL10_INVMSK = 0xf7;

/**
 * Configuration register 0 comparator duty cycle inverse bitmask.
 */
static const byte CFG0_CDC_INVMSK    = 0xf8;

/**
 * Configuration register 1 discharge cell inverse bitmask.
 */
static const byte CFG1_DCC_INVMSK    = 0x00;

/**
 * Configuration register 2 discharge cell inverse bitmask.
 */
static const byte CFG2_DCC_INVMSK    = 0xf0;

/**
 * Configuration register 2 mask cell interrupts inverse bitmask.
 */
static const byte CFG2_MCI_INVMSK    = 0x0f;

/**
 * Configuration register 3 mask cell interrupts inverse bitmask.
 */
static const byte CFG3_MCI_INVMSK    = 0x00;


void LTC6802::initSPI(const byte pinMOSI, const byte pinMISO, const byte pinCLK)
 {
  // TODO parameters for different arduinos (pins, clock)
  pinMode(pinMOSI, OUTPUT);
  pinMode(pinMISO, INPUT);
  pinMode(pinCLK, OUTPUT);

  // SPI.setBitOrder(MSBFIRST);
  // SPI.setDataMode(SPI_MODE3);
  // SPI.setClockDivider(SPI_CLOCK_DIV16);
  // SPI.begin();

  // SPI.begin(ETHERNET_SHIELD_SPI_CS);
  // SPI.setClockDivider(ETHERNET_SHIELD_SPI_CS, SPI_CLOCK_DIV16);
  // SPI.setDataMode(ETHERNET_SHIELD_SPI_CS, SPI_MODE3);
 }


void LTC6802::destroySPI()
 {
  // SPI.end();
 }


LTC6802::LTC6802(const byte address, const byte csPin)
 : address(address), csPin(csPin)
 {
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  for (int i = 0; i < cfgRegisters; ++i)
   {
    CFG[i] = 0;
   }
  for (int i = 0; i < tmpRegisters; ++i)
   {
    TMP[i] = 0;
   }
  for (int i = 0; i < cellRegisters; ++i)
   {
    CV[i] = 0;
   }
  for (int i = 0; i < flgRegisters; ++i)
   {
    FLG[i] = 0;
   }

 }


/*
LTC6802::~LTC6802()
 {
 }
*/


void LTC6802::measure(const byte cmd, const bool broadcast) const
 {
  SPI.beginTransaction(spiSettings);
  digitalWrite(csPin, LOW);
  if (!broadcast)
   {
    SPI.transfer(this->address);
   }
  SPI.transfer(cmd);

  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
  // check SDO for measure finished
 }


 void LTC6802::read(const byte cmd, const byte numOfRegisters, byte *const arr) // TODO eliminate buffer overflow risk
  {
   SPI.beginTransaction(spiSettings);
   digitalWrite(csPin, LOW);
   if (true)
    {
     SPI.transfer(this->address); // TODO broadcast
    }
   SPI.transfer(cmd);

   for (int i = 0; i < numOfRegisters; ++i)
    {
     arr[i] = SPI.transfer(cmd);
    }
   /* byte pec = */ SPI.transfer(cmd);

   digitalWrite(csPin, HIGH);
   SPI.endTransaction();
  }


void LTC6802::readValues(const byte cmd, const byte numOfRegisters, byte *const arr)
 {
  do
   {
    read(cmd, numOfRegisters, arr);
   }
  while (arr[0] == 0xff);
 }


 void LTC6802::flagsRead()
  {
   read(RDFLG, flgRegisters, FLG);
  }


 void LTC6802::flagsDebugOutput()
  {
   for (int i = flgRegisters - 1; i >= 0; --i)
    {
     Serial.print(FLG[i], HEX);
     Serial.print(" ");
    }
   Serial.println();
  }


void LTC6802::cfgRead()
 {
  read(RDCFG, cfgRegisters, CFG);
 }


void LTC6802::cfgWrite(const bool broadcast) const
 {
  SPI.beginTransaction(spiSettings);
  digitalWrite(csPin, LOW);
  if (!broadcast)
   {
    SPI.transfer(this->address);
   }
  SPI.transfer(WRCFG);
  for (int i = 0; i < cfgRegisters; ++i)
   {
    SPI.transfer(this->CFG[i]);
   }
  digitalWrite(csPin, HIGH);
  SPI.endTransaction();
 }


void LTC6802::cfgDebugOutput() const
 {
  Serial.print("WDT GPIO2/1 LVLPL Cell10 CDC: ");
  Serial.println(CFG[0], BIN); // WDT  GPIO2 GPIO1  LVLPL  Cell10  CDC[2] CDC[1] CDC[0]

  word dcc = CFG[1]; // DCC8 DCC7 DCC6 DCC5 DCC4 DCC3 DCC DCC1
  dcc |= (CFG[2] & 0x000F) << 8; // MC4I MC3I MC2I MC1I   DCC12 DCC11 DCC10 DCC9
  Serial.print("DCC: ");
  Serial.println(dcc, HEX);

  word mci = CFG[3] << 4; // MC12I MC11IMC10I MC9I MC8I MC7I MC6I MC5I
  mci |= ((CFG[2] & 0xF0) >> 4); // MC4I MC3I MC2I MC1I   DCC12 DCC11 DCC10 DCC9
  Serial.print("MC I: ");
  Serial.println(mci, HEX);

  Serial.print("VUV: ");
  Serial.println(CFG[4], HEX); // VUV 7-0

  Serial.print("VOV: ");
  Serial.println(CFG[5], HEX); // VOV 7-0
 }


bool LTC6802::cfgGetWDT() const
 {
  return (CFG[0] & CFG0_WDT_MSK);
 }


bool LTC6802::cfgGetGPIO1() const
 {
  return (CFG[0] & CFG0_GPIO1_MSK);
 }


void LTC6802::cfgSetGPIO1(const bool gpio)
 {
  CFG[0] = (CFG[0] & CFG0_GPIO1_INVMSK) | (gpio << CFG0_GPIO1_BIT);
 }


bool LTC6802::cfgGetGPIO2() const
 {
  return (CFG[0] & CFG0_GPIO2_MSK);
 }


void LTC6802::cfgSetGPIO2(const bool gpio)
 {
  CFG[0] = (CFG[0] & CFG0_GPIO2_INVMSK) | (gpio << CFG0_GPIO2_BIT);
 }


bool LTC6802::cfgGetLVLPL() const
 {
  return (CFG[0] & CFG0_LVLPL_MSK);
 }


void LTC6802::cfgSetLVLPL(const bool lvlpl)
 {
  CFG[0] = (CFG[0] & CFG0_LVLPL_INVMSK) | (lvlpl << CFG0_LVLPL_BIT);
 }


byte LTC6802::cfgGetCDC() const
 {
  return (CFG[0] & CFG0_CDC_MSK);
 }


void LTC6802::cfgSetCDC(const byte cdc)
 {
  // assert cdc 0-7
  CFG[0] = (CFG[0] & CFG0_CDC_INVMSK) | cdc;
 }


word LTC6802::cfgGetDCC() const
 {
  return ((CFG[1] & CFG1_DCC_MSK) | ((CFG[2] & CFG2_DCC_MSK) << 8));
 }


void LTC6802::cfgSetDCC(const word dcc)
 {
  // assert 0x0fff
  CFG[1] = (dcc & 0x00ff); // (CFG[1] & CFG1_DCC_INVMSK) |
  CFG[2] = (CFG[2] & CFG2_DCC_INVMSK) | ((dcc & 0x0f00) >> 8);
 }


word LTC6802::cfgGetMCI() const
 {
  return ((CFG[3] << 4) | ((CFG[2] & CFG2_MCI_MSK) >> 4));
 }


void LTC6802::cfgSetMCI(const word mci)
 {
  // assert 0x0fff
  CFG[2] = (CFG[2] & CFG2_MCI_INVMSK) | ((mci & 0x0f) << 4);
  CFG[3] = (mci & 0x0ff0) >> 4;
 }


byte LTC6802::cfgGetVUV() const // TODO float vs double
 {
  return (CFG[4] * 16 / 1.5);
 }


void LTC6802::cfgSetVUV(const byte vuv) // TODO float vs double
 {
  CFG[4] = vuv * 1.5 / 16;
 }


byte LTC6802::cfgGetVOV() const // TODO float vs double
 {
  return (CFG[5] * 16 / 1.5);
 }


void LTC6802::cfgSetVOV(const byte vov) // TODO float vs double
 {
  CFG[5] = vov * 1.5 / 16;
 }


void LTC6802::temperatureMeasure()
 {
  measure(STTMPAD, false);
 }


void LTC6802::temperatureRead()
 {
  readValues(RDTMP, tmpRegisters, TMP);
 }


void LTC6802::temperatureDebugOutput() const
 {
  /*
  word etmp1 = TMP[0]; // ETMP1
  etmp1 |= (TMP[1] & 0x0f) << 8; // ETMP2 ETMP1
  Serial.print("ETMP1: ");
  Serial.println(etmp1, HEX);
  */

  /*
  word etmp2 = TMP[2] << 4; // ETMP2
  etmp2 |= (etmp1 & 0xf0) >> 4; // ETMP2 ETMP1
  Serial.print("ETMP2: ");
  Serial.println(etmp2, HEX);
  */

  word itmp = TMP[3];
  itmp |= (TMP[4] & 0x0f) << 8; // REV  THSD  ITMP

  int cel = (itmp * 1.5 / 8) - 273;
  Serial.print("iCelsius: ");
  Serial.println(cel);

  Serial.print("THSD: ");
  Serial.println((TMP[4] >> 4) & 0x01, HEX); // REV  THSD  ITMP

  Serial.print("REV: ");
  Serial.println(TMP[4] >> 5, HEX); // REV  THSD  ITMP
 }


void LTC6802::cellsMeasure()
 {
  measure(STCVAD, false);
 }


void LTC6802::cellsRead()
 {
  readValues(RDCV, cellRegisters, CV);
 }


void LTC6802::cellsDebugOutput() const
 {
  word cellvolts[maxCells];
  cellvolts[0] = CV[0] | ((CV[1] & 0x0F) << 8);
  cellvolts[1] = ((CV[1] & 0xf0) >> 4) | (CV[2] << 4);

  cellvolts[2] = CV[3] | ((CV[4] & 0x0F) << 8);
  cellvolts[3] = ((CV[4] & 0xf0) >> 4) | (CV[5] << 4);

  cellvolts[4] = CV[6] | ((CV[7] & 0x0F) << 8);
  cellvolts[5] = ((CV[7] & 0xf0) >> 4) | (CV[8] << 4);

  cellvolts[6] = CV[9] | ((CV[10] & 0x0F) << 8);
  cellvolts[7] = ((CV[10] & 0xf0) >> 4) | (CV[11] << 4);

  cellvolts[8] = CV[12] | ((CV[13] & 0x0F) << 8);
  cellvolts[9] = ((CV[13] & 0xf0) >> 4) | (CV[14] << 4);

  cellvolts[10] = CV[15] | ((CV[16] & 0x0F) << 8);
  cellvolts[11] = ((CV[16] & 0xf0) >> 4) | (CV[17] << 4);

  Serial.print(cellvolts[0] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[1] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[2] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[3] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[4] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[5] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[6] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[7] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[8] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[9] * 1.5 / 1000);
  Serial.print(", ");
  Serial.print(cellvolts[10] * 1.5 / 1000);
  Serial.print(", ");
  Serial.println(cellvolts[11] * 1.5 / 1000);
 }
