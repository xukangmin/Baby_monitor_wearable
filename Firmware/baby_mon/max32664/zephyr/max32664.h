#ifndef _MAX32664_H
#define _MAX32664_H


#include <drivers/sensor.h>
#include <drivers/i2c.h>
#include <drivers/gpio.h>

#define SENSOR_CHAN_HEARTRATE SENSOR_CHAN_COMMON_COUNT + 1
#define SENSOR_CHAN_SPO2 SENSOR_CHAN_COMMON_COUNT + 2
#define SENSOR_CHAN_CONFIDENCE SENSOR_CHAN_COMMON_COUNT + 3
#define SENSOR_CHAN_HR_STATUS SENSOR_CHAN_COMMON_COUNT + 4

#define WRITE_FIFO_INPUT_BYTE  0x04
#define DISABLE                0x00
#define ENABLE                 0x01
#define MODE_ONE               0x01
#define MODE_TWO               0x02
#define APP_MODE               0x00
#define BOOTLOADER_MODE        0x08
#define NO_WRITE               0x00 
#define INCORR_PARAM           0xEE

#define CONFIGURATION_REGISTER 0x0A
#define PULSE_MASK             0xFC
#define READ_PULSE_MASK        0x03
#define SAMP_MASK              0xE3
#define READ_SAMP_MASK         0x1C
#define ADC_MASK               0x9F
#define READ_ADC_MASK          0x60

#define ENABLE_CMD_DELAY          45 // Milliseconds
#define CMD_DELAY                 6  // Milliseconds
#define MAXFAST_ARRAY_SIZE        7  // Number of bytes.... plus status byte
#define MAXFAST_EXTENDED_DATA     5
#define MAX30101_LED_ARRAY        12 // 4 values of 24 bit (3 byte) LED values

#define SET_FORMAT             0x00
#define READ_FORMAT            0x01 // Index Byte under Family Byte: READ_OUTPUT_MODE (0x11)
#define WRITE_SET_THRESHOLD    0x01 //Index Byte for WRITE_INPUT(0x14)
#define WRITE_EXTERNAL_TO_FIFO 0x00

#define BIO_ADDRESS 0x55

struct bioData {

  uint32_t irLed; 
  uint32_t redLed; 
  uint16_t heartRate; // LSB = 0.1bpm
  uint8_t  confidence; // 0-100% LSB = 1%
  uint16_t oxygen; // 0-100% LSB = 1%
  uint8_t  status; // 0: Success, 1: Not Ready, 2: Object Detectected, 3: Finger Detected
  float    rValue;      // -- Algorithm Mode 2 vv
  int8_t   extStatus;   // --
  uint8_t  reserveOne;  // --
  uint8_t  resserveTwo; // -- Algorithm Mode 2 ^^

};

struct version {
  // 3 bytes total
  uint8_t major; 
  uint8_t minor; 
  uint8_t revision; 

}; 

struct sensorAttr {

  uint8_t byteWord;
  uint8_t availRegisters;

};

// Status Bytes are communicated back after every I-squared-C transmission and
// are indicators of success or failure of the previous transmission.
enum READ_STATUS_BYTE_VALUE {

  SUCCESS                  = 0x00,
  ERR_UNAVAIL_CMD,
  ERR_UNAVAIL_FUNC,
  ERR_DATA_FORMAT,
  ERR_INPUT_VALUE,
  ERR_TRY_AGAIN,
  ERR_BTLDR_GENERAL        = 0x80,
  ERR_BTLDR_CHECKSUM,
  ERR_BTLDR_AUTH,
  ERR_BTLDR_INVALID_APP,
  ERR_UNKNOWN              = 0xFF

};

// The family register bytes are the larger umbrella for all the Index and
// Write Bytes listed below. You can not reference a nestled byte without first
// referencing it's larger category: Family Register Byte.
enum FAMILY_REGISTER_BYTES {
  
  HUB_STATUS               = 0x00,
  SET_DEVICE_MODE,
  READ_DEVICE_MODE,
  OUTPUT_MODE            = 0x10,  
  READ_OUTPUT_MODE,       
  READ_DATA_OUTPUT,         
  READ_DATA_INPUT,
  WRITE_INPUT,
  WRITE_REGISTER           = 0x40, 
  READ_REGISTER,
  READ_ATTRIBUTES_AFE,
  DUMP_REGISTERS,
  ENABLE_SENSOR, 
  READ_SENSOR_MODE,
  CHANGE_ALGORITHM_CONFIG  = 0x50,
  READ_ALGORITHM_CONFIG,
  ENABLE_ALGORITHM,
  BOOTLOADER_FLASH         = 0x80,
  BOOTLOADER_INFO,
  IDENTITY                 = 0xFF

};

// All the defines below are: 1. Index Bytes nestled in the larger category of the
// family registry bytes listed above and 2. The Write Bytes nestled even
// farther under their Index Bytes.

// Write Bytes under Family Byte: SET_DEVICE_MODE (0x01) and Index
// Byte: 0x00. 
enum DEVICE_MODE_WRITE_BYTES {

  EXIT_BOOTLOADER          = 0x00,
  RESET                    = 0x02,
  ENTER_BOOTLOADER         = 0x08

};

// Write Bytes under Family Byte: OUTPUT_MODE (0x10) and Index byte: SET_FORMAT
// (0x00)
enum OUTPUT_MODE_WRITE_BYTE {

  PAUSE                    = 0x00,
  SENSOR_DATA,
  ALGO_DATA,
  SENSOR_AND_ALGORITHM,
  PAUSE_TWO,
  SENSOR_COUNTER_BYTE,
  ALGO_COUNTER_BYTE,
  SENSOR_ALGO_COUNTER

};

// Index Byte under the Family Byte: READ_DATA_OUTPUT (0x12)
enum FIFO_OUTPUT_INDEX_BYTE {

  NUM_SAMPLES,
  READ_DATA

};

// Index Byte under the Family Byte: READ_DATA_INPUT (0x13)
enum FIFO_EXTERNAL_INDEX_BYTE {

  SAMPLE_SIZE,
  READ_INPUT_DATA,
  READ_SENSOR_DATA, // For external accelerometer
  READ_NUM_SAMPLES_INPUT, // For external accelerometer
  READ_NUM_SAMPLES_SENSOR

};

// Index Byte under the Family Registry Byte: WRITE_REGISTER (0x40)
enum WRITE_REGISTER_INDEX_BYTE {

  WRITE_MAX30101 = 0x03,
  WRITE_ACCELEROMETER

};

// Index Byte under the Family Registry Byte: READ_REGISTER (0x41)
enum READ_REGISTER_INDEX_BYTE {

  READ_MAX30101 = 0x03,
  READ_ACCELEROMETER

};

// Index Byte under the Family Registry Byte: READ_ATTRIBUTES_AFE (0x42)
enum GET_AFE_INDEX_BYTE {
  
  RETRIEVE_AFE_MAX30101 = 0x03,
  RETRIEVE_AFE_ACCELEROMETER

};

// Index Byte under the Family Byte: DUMP_REGISTERS (0x43)
enum DUMP_REGISTER_INDEX_BYTE {
  
  DUMP_REGISTER_MAX30101 = 0x03,
  DUMP_REGISTER_ACCELEROMETER

};

// Index Byte under the Family Byte: ENABLE_SENSOR (0x44)
enum SENSOR_ENABLE_INDEX_BYTE {
  
  ENABLE_MAX30101 = 0x03,
  ENABLE_ACCELEROMETER

};

// Index Byte for the Family Byte: READ_SENSOR_MODE (0x45)
enum READ_SENSOR_ENABLE_INDEX_BYTE {

  READ_ENABLE_MAX30101 = 0x03,
  READ_ENABLE_ACCELEROMETER

};

// Index Byte under the Family Byte: CHANGE_ALGORITHM_CONFIG (0x50)
enum ALGORITHM_CONFIG_INDEX_BYTE {

  SET_TARG_PERC            = 0x00,
  SET_STEP_SIZE            = 0x00,
  SET_SENSITIVITY          = 0x00,
  SET_AVG_SAMPLES          = 0x00,
  SET_PULSE_OX_COEF        = 0x02,

};

// Write Bytes under the Family Byte: CHANGE_ALGORITHM_CONFIG (0x50) and the
// Index Byte: ALGORITHM_CONFIG_INDEX_BYTE - SET_TARG_PERC
enum ALGO_AGC_WRITE_BYTE {
  
  AGC_GAIN_ID              = 0x00, 
  AGC_STEP_SIZE_ID,
  AGC_SENSITIVITY_ID,
  AGC_NUM_SAMP_ID,
  MAXIMFAST_COEF_ID        = 0x0B

};

// Index Bytes under the Family Byte: READ_ALGORITHM_CONFIG (0x51)
enum READ_ALGORITHM_INDEX_BYTE {

  READ_AGC_PERCENTAGE      = 0x00,
  READ_AGC_STEP_SIZE       = 0x00,
  READ_AGC_SENSITIVITY     = 0x00,
  READ_AGC_NUM_SAMPLES     = 0x00,
  READ_MAX_FAST_COEF       = 0x02

};

// Write Bytes under the Family Byte: READ_ALGORITHM_CONFIG (0x51) and Index Byte: 
// READ_ALGORITHM_INDEX_BYTE - AGC
enum READ_AGC_ALGO_WRITE_BYTE {
  
  READ_AGC_PERC_ID           = 0x00,
  READ_AGC_STEP_SIZE_ID,
  READ_AGC_SENSITIVITY_ID,
  READ_AGC_NUM_SAMPLES_ID,
  READ_MAX_FAST_COEF_ID      = 0x0B

};

// Index Byte under the Family Byte: ENABLE_ALGORITHM (0x52).
enum ALGORITHM_MODE_ENABLE_INDEX_BYTE {

  ENABLE_AGC_ALGO  = 0x00,
  ENABLE_WHRM_ALGO = 0x02

};

// Index Byte under the Family Byte: BOOTLOADER_FLASH (0x80).
enum BOOTLOADER_FLASH_INDEX_BYTE {

  SET_INIT_VECTOR_BYTES    = 0x00,
  SET_AUTH_BYTES,
  SET_NUM_PAGES,
  ERASE_FLASH,
  SEND_PAGE_VALUE

};

// Index Byte under the Family Byte: BOOTLOADER_INFO (0x81).
enum BOOTLOADER_INFO_INDEX_BYTE {

  BOOTLOADER_VERS          = 0x00,
  PAGE_SIZE

};

// Index Byte under the Family Byte: IDENTITY (0xFF).
enum IDENTITY_INDEX_BYTES {

  READ_MCU_TYPE            = 0x00,
  READ_SENSOR_HUB_VERS     = 0x03,
  READ_ALGO_VERS           = 0x07

};


struct max32664_config {
	const char *i2c_label;
	uint16_t i2c_addr;
};

struct max32664_data {
	const struct device *i2c;
  struct bioData outData;
};


#endif