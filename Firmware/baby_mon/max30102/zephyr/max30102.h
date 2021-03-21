/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <drivers/sensor.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>

#define MAX30102_IIC_ADDRESS  0x57 //I2C Address

//Status Registers
#define MAX30102_INTSTAT1        0x00//Interrupt Status1
#define MAX30102_INTSTAT2        0x01//Interrupt Status2
#define MAX30102_INTENABLE1      0x02//Interrupt Enable1
#define MAX30102_INTENABLE2      0x03//Interrupt Enable2
//FIFO Registers
#define MAX30102_FIFOWRITEPTR    0x04//FIFO Write Pointer, The FIFO Write Pointer points to the location where the MAX30102 writes the next sample. This pointer advances for each sample pushed on to the FIFO. It can also be changed through the I2C interface when MODE[2:0] is 010, 011, or 111.
#define MAX30102_FIFOOVERFLOW    0x05//FIFO Overflow Counter, When the FIFO is full, samples are not pushed on to the FIFO, samples are lost. OVF_COUNTER counts the number of samples lost. It saturates at 0x1F. When a complete sample is “popped” (i.e., removal of old FIFO data and shifting the samples down) from the FIFO (when the read pointer advances), OVF_COUNTER is reset to zero.
#define MAX30102_FIFOREADPTR     0x06//FIFO Read Pointer, The FIFO Read Pointer points to the location from where the processor gets the next sample from the FIFO through the I2C interface. This advances each time a sample is popped from the FIFO. The processor can also write to this pointer after reading the samples to allow rereading samples from the FIFO if there is a data communication error.
#define MAX30102_FIFODATA        0x07//FIFO Data Register, The circular FIFO depth is 32 and can hold up to 32 samples of data. The sample size depends on the number of LED channels (a.k.a. channels) configured as active. As each channel signal is stored as a 3-byte data signal, the FIFO width can be 3 bytes or 6 bytes in size.
//Configuration Registers
#define MAX30102_FIFOCONFIG      0x08//FIFO Configuration
#define MAX30102_MODECONFIG      0x09//Mode Configuration
#define MAX30102_PARTICLECONFIG  0x0A//SpO2 Configuration
#define MAX30102_LED1_PULSEAMP   0x0C//LED1 Pulse Amplitude
#define MAX30102_LED2_PULSEAMP   0x0D//LED2 Pulse Amplitude
#define MAX30102_LED3_PULSEAMP   0x0E//RESERVED
#define MAX30102_LED_PROX_AMP    0x10//RESERVED
#define MAX30102_MULTILEDCONFIG1 0x11//Multi-LED Mode Control Registers
#define MAX30102_MULTILEDCONFIG2 0x12//Multi-LED Mode Control Registers
//Die Temperature Registers
#define MAX30102_DIETEMPINT      0x1F//Die Temp Integer
#define MAX30102_DIETEMPFRAC     0x20//Die Temp Fraction
#define MAX30102_DIETEMPCONFIG   0x21//Die Temperature Config
#define MAX30102_PROXINTTHRESH   0x30//RESERVED
//Part ID Registers
#define MAX30102_REVISIONID      0xFE//Revision ID
#define MAX30102_PARTID          0xFF//Part ID:0x15
#define MAX30102_EXPECTED_PARTID  0x15

#define MAX30102_SENSE_BUF_SIZE  30



//Configuration Options 
//FIFO Configuration(Register address 0x08)
//sampleAverage(Table 3. Sample Averaging)
#define SAMPLEAVG_1     0
#define SAMPLEAVG_2     1
#define SAMPLEAVG_4     2
#define SAMPLEAVG_8     3
#define SAMPLEAVG_16    4
#define SAMPLEAVG_32    5

//Mode configuration(Register address 0x09)
//ledMode(Table 4. Mode Control)
#define MODE_REDONLY    2
#define MODE_RED_IR     3
#define MODE_MULTILED   7

//Particle sensing configuration(Register address 0x0A)
//adcRange(Table 5. SpO2 ADC Range Control)
#define ADCRANGE_2048   0
#define ADCRANGE_4096   1
#define ADCRANGE_8192   2
#define ADCRANGE_16384  3
//sampleRate(Table 6. SpO2 Sample Rate Control)
#define SAMPLERATE_50   0 
#define SAMPLERATE_100  1
#define SAMPLERATE_200  2
#define SAMPLERATE_400  3
#define SAMPLERATE_800  4
#define SAMPLERATE_1000 5
#define SAMPLERATE_1600 6
#define SAMPLERATE_3200 7
//pulseWidth(Table 7. LED Pulse Width Control)
#define PULSEWIDTH_69   0 
#define PULSEWIDTH_118  1
#define PULSEWIDTH_215  2
#define PULSEWIDTH_411  3

//Multi-LED Mode Control Registers(Register address 0x011)
#define SLOT_NONE       0
#define SLOT_RED_LED    1
#define SLOT_IR_LED     2




 /*
    Interrupt Status(0x00–0x01) (pg 12)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |  A_FULL  | PPG_RDY  |  ALC_OVF |                    NONE                    | PWR_RDY  |
    * ------------------------------------------------------------------------------------------
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2   |     b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |                           NONE                                 |DIE_TEMP_RDY|   NONE   |
    * ------------------------------------------------------------------------------------------
  */
  /*
    Interrupt Enable(0x02–0x03) (pg 13)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |A_FULL_EN |PPG_RDY_EN|ALC_OVF_EN|                         NONE                          |
    * ------------------------------------------------------------------------------------------
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2   |       b1      |   b0  |
    * ------------------------------------------------------------------------------------------
    * |                           NONE                                 |DIE_TEMP_RDY_EN|  NONE |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   NONE:1;
    uint8_t   dieTemp:1;     // Internal Temperature Ready Flag
    uint8_t   NONE1:6;
    uint8_t   NONE2:5;
    uint8_t   ALCOverflow:1; // Ambient Light Cancellation Overflow
    uint8_t   dataReady:1;   // New FIFO Data Ready
    uint8_t   almostFull:1;  // FIFO Almost Full Flag
  } __attribute__ ((packed)) sEnable_t;
  /*
    FIFO Configuration(0x08) (pg 17)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4          | b3 |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |            SMP_AVE             |FIFO_ROLLOVER_EN|               FIFO_A_FULL            |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   almostFull:4; // FIFO Almost Full Value
    uint8_t   RollOver:1;   // FIFO Rolls on Full
    uint8_t   sampleAverag:3;  // Sample Averaging
  } __attribute__ ((packed)) sFIFO_t;

  /*
    Mode configuration(0x09) (pg 18)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |   SHDN   |   RESET  |             NONE               |              MODE               |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   ledMode:6; /*!< 010:Heart Rate mode, Red only. 011:SpO2 mode, Red and IR. 111:Multi-LED mode, Red and IR*/
    uint8_t   reset:1; /*!< 1:reset */
    uint8_t   shutDown:1; /*!< 0: wake up 1: put IC into low power mode*/
  } __attribute__ ((packed)) sMode_t;

  /*
    Particle sensing configuration(0x0A) (pg 18)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |   NONE   |     SPO2_ADC_RGE    |             SPO2_SR            |        LED_PW        |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   pulseWidth:2;
    uint8_t   sampleRate:3;
    uint8_t   adcRange:3;
  } __attribute__ ((packed)) sParticle_t;

  /*
    LED Pulse Amplitude(0x0C–0x0D) (pg 20)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |                                         LED1_PA                                        |
    * ------------------------------------------------------------------------------------------
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |                                         LED2_PA                                        |
    * ------------------------------------------------------------------------------------------
  */
  /*
    Multi-LED Mode Control Registers(0x011) (pg 21)
    * ------------------------------------------------------------------------------------------
    * |    b7    |    b6    |    b5    |    b4    |    b3    |    b2    |    b1     |    b0    |
    * ------------------------------------------------------------------------------------------
    * |   NONE   |              SLOT2             |   NONE   |             SLOT1               |
    * ------------------------------------------------------------------------------------------
  */
  typedef struct {
    uint8_t   SLOT1:4;
    uint8_t   SLOT2:4;
  } __attribute__ ((packed)) sMultiLED_t;

  /*!
   *@brief Circular buffer for storing samples 
   */
  typedef struct {
    uint32_t red[MAX30102_SENSE_BUF_SIZE];
    uint32_t IR[MAX30102_SENSE_BUF_SIZE];
    uint8_t head;
    uint8_t tail;
  } sSenseBuf_t;

struct max30102_config {
	const char *i2c_label;
	uint16_t i2c_addr;
	uint8_t ledBrightness;
	uint8_t sampleAverage;
	uint8_t ledMode;
	uint8_t sampleRate;
	uint8_t pulseWidth;
	uint8_t adcRange;
};

struct max30102_data {
	const struct device *i2c;
	sSenseBuf_t senseBuf;//Buffer for storing multiple groups of array
};
