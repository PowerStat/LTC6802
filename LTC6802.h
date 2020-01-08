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
#ifndef LTC6802_H_INCLUDED_
  #define LTC6802_H_INCLUDED_

  #include <Arduino.h>

  // 57./58./59. namespace?
  // 72./73./74./75. exceptions
  // 68. assert

  /**
   * LTC6802-2 multicell addressable battery stack monitor value class.
   *
   * https://cds.linear.com/docs/en/datasheet/68022fa.pdf
   */
  class LTC6802
   {
    public:
      /**
       * Init SPI bus for LTC6802 chips.
       *
       * Call only one time, and not for each chip!
       *
       * @param pinMOSI Pin for master out slave in
       * @param pinMISO Pin for master in slave out
       * @param pinCLK Pin for clock
       */
      static void initSPI(byte pinMOSI = 11, byte pinMISO = 12, byte pinCLK = 13);

      /**
       * Destroy SPI bus.
       *
       * Call only one time, and not for each chip!
       */
      static void destroySPI();


      /**
       * Constructor.
       *
       * @param address Chip address on SPI bus
       * @param csPin Chip select pin
       */
      explicit LTC6802(byte address, byte csPin);

      // ~LTC6802();

      /**
       * Read configuration from chip registers.
       */
      void cfgRead();

      /**
       * Write configuration to chip registers.
       *
       * @param broadcast Send as broadcast
       */
      void cfgWrite(bool broadcast) const;

      /**
       * Write configuration to serial.
       *
       * @Deprecated  Don't use anymore
       */
      void cfgDebugOutput() const;

      /**
       * Get Watchdog timer flag from configuration.
       *
       * @return 0 : WDTB pin low; 1 : WDTB pin high
       */
      bool cfgGetWDT() const;

      /**
       * Get GPIO1 pin control from configuration.
       *
       * @return 0 : GPIO1 pin low; 1 : GPIO1 pin high
       */
      bool cfgGetGPIO1() const;

      /**
       * Set GPIO1 pin control in configuration.
       *
       * @param gpio 0 : GPIO1 pin pull down on; 1 : GPIO1 pin pull down off (default)
       */
      void cfgSetGPIO1(bool gpio);

      /**
       * Get GPIO2 pin control from configuration.
       *
       * @return 0 : GPIO2 pin low; 1 : GPIO2 pin high
       */
      bool cfgGetGPIO2() const;

      /**
       * Set GPIO2 pin control in configuration.
       *
       * @param gpio 0 : GPIO2 pin pull down on; 1 : GPIO2 pin pull down off (default)
       */
      void cfgSetGPIO2(bool gpio);

      /**
       * Get level polling mode from configuration.
       *
       * @return 0 : toggle polling mode (default); 1 : level polling
       */
      bool cfgGetLVLPL() const;

      /**
       * Set level polling mode in configuration.
       *
       * @param lvlpl 0 : toggle polling mode (default); 1 : level polling
       */
      void cfgSetLVLPL(bool lvlpl);

      /**
       * Get comparator duty cycle from configuration.
       *
       * @return 0 : Standby mode; 1 : Measure mode comparator off; 2 : Comparator 13ms; 3 : Comparator 130ms; 4 : Comparator 500ms; 5: Comparator 130ms with power down; 6 : Comparator 500ms with power down; 7 : Comparator 2000ms with power down
       */
      byte cfgGetCDC() const;

      /**
       * Set comparator duty cycle in configuration.
       *
       * @param cdc 0 : Standby mode; 1 : Measure mode comparator off; 2 : Comparator 13ms; 3 : Comparator 130ms; 4 : Comparator 500ms; 5: Comparator 130ms with power down; 6 : Comparator 500ms with power down; 7 : Comparator 2000ms with power down
       */
      void cfgSetCDC(byte cdc);

      /**
       * Get discharge cell flags from configuration.
       *
       * @return bit 0-11: 0: turn off shorting switch for cell (default); 1: turn on shorting switch
       */
      word cfgGetDCC() const;

      /**
       * Set discharge cell flags in configuration.
       *
       * @param dcc bit 0-11: 0: turn off shorting switch for cell (default); 1: turn on shorting switch
       */
      void cfgSetDCC(word dcc);

      /**
       * Get mask cell interrupt flags from configuration.
       *
       * @return bit 0-11: 0: enable interrupt value for cell (default); 1: turn off interrupts and clear flags for cell
       */
      word cfgGetMCI() const;

      /**
       * Set mask cell interrupt flags in configuration.
       *
       * @param mci bit 0-11: 0: enable interrupt value for cell (default); 1: turn off interrupts and clear flags for cell
       */
      void cfgSetMCI(word mci);

      /**
       * Get undervoltage comparison voltage from configuration.
       *
       * @return Voltage (default: 0 or factory programmed)
       */
      byte cfgGetVUV() const;

      /**
       * Set undervoltage comparison voltage in configuration.
       *
       * @param vuv Voltage
       */
      void cfgSetVUV(byte vuv);

      /**
       * Get overvoltage comparison voltage from configuration.
       *
       * @return Voltage (default: 0 or factory programmed)
       */
      byte cfgGetVOV() const;

      /**
       * Set overvoltage comparison voltage in configuration.
       *
       * @param vov Voltage
       */
      void cfgSetVOV(byte vov);

      /**
       * Measure temperatures on chip.
       */
      void temperatureMeasure();

      /**
       * Read temperatures from chip.
       */
      void temperatureRead();

      /**
       * Write temperatures to serial.
       *
       * @Deprecated Don't use anymore
       */
      void temperatureDebugOutput() const;

      /**
       * Measure cell voltages on chip.
       */
      void cellsMeasure();

      /**
       * Read cell voltages from chip.
       */
      void cellsRead();

      /**
       * Write cell voltages to serial.
       *
       * @Deprecated Don't use anymore
       */
      void cellsDebugOutput() const;

      /**
       * Read flag register group from chip.
       */
      void flagsRead();

      /**
       * Write flag register group to serial.
       *
       * @Deprecated Don't use anymore
       */
      void flagsDebugOutput();

      // bool operator==(const LTC6802& obj1, const LTC6802& obj2);
      // bool operator!=(const LTC6802& obj1, const LTC6802& obj2);

    protected:
      // Disable heap allocation
      static void *operator new (size_t) throw() {return (0);}
      static void operator delete (void *) throw() {}

    private:
      /**
       * Number of LTC6802 configuration registers.
       */
      static const byte cfgRegisters = 6;

      /**
       * Number of LTC6802 temperature registers.
       */
      static const byte tmpRegisters = 5;

      /**
       * Number of LTC6802 cell registers.
       */
      static const byte cellRegisters = 18;

      /**
       * Number of LTC6802 flag registers.
       */
      static const byte flgRegisters = 3;

      /**
       * Number of maximum cells connected to LTC6802.
       */
      static const byte maxCells = 12;

      /**
       * Chip SPI address.
       */
      byte address;

      /**
       * Chip select pin.
       */
      byte csPin = 10;

      /**
       * Configuration register group read from chip.
       */
      byte CFG[cfgRegisters];

      /**
       * Tempertaure register group.
       */
      byte TMP[tmpRegisters];

      /**
       * Cell voltage register group.
       */
      byte CV[cellRegisters];

      /**
       * Flag register group.
       */
      byte FLG[flgRegisters];


      // Disable array heap allocation
      static void *operator new[] (size_t);
      static void operator delete[] (void *);

      /**
       * Read registers from chip.
       *
       * @param cmd Read command.
       * @param numOfRegisters Number of registers to read
       * @param arr Array for register values
       */
      void read(byte cmd, byte numOfRegisters, byte * arr);

      /**
       * Send measure command to chip.
       *
       * @param cmd Chip command
       * @param broadcast Send as broadcast to multiple chips
       */
      void measure(byte cmd, bool broadcast) const;

      /**
       * Read register values from chip.
       *
       * @param cmd Read command.
       * @param numOfRegisters Number of registers to read
       * @param arr Array for register values
       */
      void readValues(byte cmd, byte numOfRegisters, byte * arr);

   };

#endif
