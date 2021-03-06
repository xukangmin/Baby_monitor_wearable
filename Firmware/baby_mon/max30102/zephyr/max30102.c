/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT max_max30102

#include <logging/log.h>

#include "max30102.h"

#include "algorithm.h"


#define BUFFER_LENGTH 200

static uint16_t _i2c_addr;

static struct device *_i2c_device;

static int _activeLEDs = 0;

static sSenseBuf_t* _senseBuf;


int32_t SPO2; //SPO2
int8_t SPO2Valid; //Flag to display if SPO2 calculation is valid
int32_t heartRate; //Heart-rate
int8_t heartRateValid; //Flag to display if heart-rate calculation is valid 



static uint32_t irBuffer[BUFFER_LENGTH];
static uint32_t redBuffer[BUFFER_LENGTH];

LOG_MODULE_REGISTER(MAX30102, CONFIG_SENSOR_LOG_LEVEL);

void writeReg(uint8_t reg, const void* pBuf, uint8_t size)
{
  if(pBuf == NULL) {
    LOG_ERR("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;

  uint8_t val = *_pBuf;

 if (size == 1)
 {
	 i2c_reg_write_byte(_i2c_device, _i2c_addr, reg, val);
 }
 else
 {
	 i2c_burst_write(_i2c_device, _i2c_addr, reg, _pBuf, size);
 }

   
}

uint8_t readReg(uint8_t reg, const void* pBuf, uint8_t size)
{
  if(pBuf == NULL) {
    LOG_ERR("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;


 if (size == 1)
 {
	 i2c_reg_read_byte(_i2c_device, _i2c_addr, reg, _pBuf);
 }
 else
 {
	 i2c_burst_read(_i2c_device, _i2c_addr, reg, _pBuf, size);
 }
  

  return size;
}

void softReset(void)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.reset = 1;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);

  while(modeReg.reset != 0)
  {
	  readReg(MAX30102_MODECONFIG, &modeReg, 1);
	  k_msleep(10);
	  LOG_INF("RESETTING");
  }

}

void shutDown(void)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.shutDown = 1;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
}

void wakeUp(void)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.shutDown = 0;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
}

void setLEDMode(uint8_t ledMode)
{
  sMode_t modeReg;
  readReg(MAX30102_MODECONFIG, &modeReg, 1);
  modeReg.ledMode = ledMode;
  writeReg(MAX30102_MODECONFIG, &modeReg, 1);
}

void setADCRange(uint8_t adcRange)
{
  sParticle_t particleReg;
  readReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
  particleReg.adcRange = adcRange;
  writeReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
}

void setSampleRate(uint8_t sampleRate)
{
  sParticle_t particleReg;
  readReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
  particleReg.sampleRate = sampleRate;
  writeReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
}

void setPulseWidth(uint8_t pulseWidth)
{
  sParticle_t particleReg;
  readReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
  particleReg.pulseWidth = pulseWidth;
  writeReg(MAX30102_PARTICLECONFIG, &particleReg, 1);
}

void setPulseAmplitudeRed(uint8_t amplitude)
{
  uint8_t byteTemp = amplitude;
  writeReg(MAX30102_LED1_PULSEAMP, &byteTemp, 1);
}

void setPulseAmplitudeIR(uint8_t amplitude)
{
  uint8_t byteTemp = amplitude;
  writeReg(MAX30102_LED2_PULSEAMP, &byteTemp, 1);
}

void enableSlot(uint8_t slotNumber, uint8_t device)
{
  sMultiLED_t multiLEDReg;
  switch (slotNumber) {
  case (1):
    readReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    multiLEDReg.SLOT1 = device;
    writeReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    break;
  case (2):
    readReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    multiLEDReg.SLOT2 = device;
    writeReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
    break;
  default:
    break;
  }
}

void disableAllSlots(void)
{
  sMultiLED_t multiLEDReg;
  multiLEDReg.SLOT1 = 0;
  multiLEDReg.SLOT2 = 0;
  writeReg(MAX30102_MULTILEDCONFIG1, &multiLEDReg, 1);
}

void resetFIFO(void)
{
  uint8_t byteTemp = 0;
  writeReg(MAX30102_FIFOWRITEPTR, &byteTemp, 1);
  writeReg(MAX30102_FIFOOVERFLOW, &byteTemp, 1);
  writeReg(MAX30102_FIFOREADPTR, &byteTemp, 1);
}


void setFIFOAverage(uint8_t numberOfSamples)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.sampleAverag = numberOfSamples;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}

void enableFIFORollover(void)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.RollOver = 1;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}

void disableFIFORollover(void)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.RollOver = 0;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}

void setFIFOAlmostFull(uint8_t numberOfSamples)
{
  sFIFO_t FIFOReg;
  readReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
  FIFOReg.almostFull = numberOfSamples;
  writeReg(MAX30102_FIFOCONFIG, &FIFOReg, 1);
}


uint8_t getWritePointer(void)
{
  uint8_t byteTemp;
  readReg(MAX30102_FIFOWRITEPTR, &byteTemp, 1);
  return byteTemp;
}

uint8_t getReadPointer(void)
{
  uint8_t byteTemp;
  readReg(MAX30102_FIFOREADPTR, &byteTemp, 1);
  return byteTemp;
}


float readTemperatureC()
{

  uint8_t byteTemp = 0x01;
  writeReg(MAX30102_DIETEMPCONFIG, &byteTemp, 1);
  
  uint8_t timeout = 100;

  //uint32_t startTime = millis();
  while (timeout--) { 
    readReg(MAX30102_DIETEMPCONFIG, &byteTemp, 1);
    if ((byteTemp & 0x01) == 0) break; 
    k_msleep(1);
  }


  uint8_t tempInt;
  readReg(MAX30102_DIETEMPINT, &tempInt, 1);

  uint8_t tempFrac;
  readReg(MAX30102_DIETEMPFRAC, &tempFrac, 1);

  return (float)tempInt + ((float)tempFrac * 0.0625);
}

float readTemperatureF()
{
  float temp = readTemperatureC();
  if (temp != -999.0) temp = temp * 1.8 + 32.0;
  return (temp);
}

uint8_t getPartID()
{
  uint8_t byteTemp;
  readReg(MAX30102_PARTID, &byteTemp, 1);
  return byteTemp;
}

void getNewData(void)
{
  int32_t numberOfSamples = 0;
  uint8_t readPointer = 0;
  uint8_t writePointer = 0;
  while (1) {
    readPointer = getReadPointer();
    writePointer = getWritePointer();

    if (readPointer == writePointer) {
      //LOG_INF("no data");
    } else {
      numberOfSamples = writePointer - readPointer;
      if (numberOfSamples < 0) numberOfSamples += 32;
      int32_t bytesNeedToRead = numberOfSamples * _activeLEDs * 3;
   
        while (bytesNeedToRead > 0) {
          _senseBuf->head++;
          _senseBuf->head %= MAX30102_SENSE_BUF_SIZE;
          uint32_t tempBuf = 0;
          if (_activeLEDs > 1) { 
            uint8_t temp[6];
            uint8_t tempex;

            readReg(MAX30102_FIFODATA, temp, 6);

            for(uint8_t i = 0; i < 3; i++){
              tempex = temp[i];
              temp[i] = temp[5-i];
              temp[5-i] = tempex;
            }

            memcpy(&tempBuf, temp, 3*sizeof(temp[0]));
            tempBuf &= 0x3FFFF;
            _senseBuf->IR[_senseBuf->head] = tempBuf;
            memcpy(&tempBuf, temp+3, 3*sizeof(temp[0]));
            tempBuf &= 0x3FFFF;
            _senseBuf->red[_senseBuf->head] = tempBuf;
          } else { 
            uint8_t temp[3];
            uint8_t tempex;


            readReg(MAX30102_FIFODATA, temp, 3);
            tempex = temp[0];
            temp[0] = temp[2];
            temp[2] = tempex;

            memcpy(&tempBuf, temp, 3*sizeof(temp[0]));
            tempBuf &= 0x3FFFF;
            _senseBuf->red[_senseBuf->head] = tempBuf;
          }
          bytesNeedToRead -= _activeLEDs * 3;
        }
      return;
    }
    k_msleep(1);
  }
}


static void heartrateAndOxygenSaturation(int32_t* SPO2,int8_t* SPO2Valid,int32_t* heartRate,int8_t* heartRateValid)
{



  

  for (uint8_t i = 0 ; i < BUFFER_LENGTH ; ) {
    //LOG_INF("buffer size=%d", i);
    getNewData(); 

    int8_t numberOfSamples = _senseBuf->head - _senseBuf->tail;
    if (numberOfSamples < 0) {
      numberOfSamples += MAX30102_SENSE_BUF_SIZE;
    }

    while(numberOfSamples--) {
      redBuffer[i] = _senseBuf->red[_senseBuf->tail];
      irBuffer[i] = _senseBuf->IR[_senseBuf->tail];

      _senseBuf->tail++;
      _senseBuf->tail %= MAX30102_SENSE_BUF_SIZE;
      i++;
      if(i == BUFFER_LENGTH) break;
    }
  }

  maxim_heart_rate_and_oxygen_saturation(/**pun_ir_buffer=*/irBuffer, /*n_ir_buffer_length=*/BUFFER_LENGTH, /**pun_red_buffer=*/redBuffer, \
      /**pn_spo2=*/SPO2, /**pch_spo2_valid=*/SPO2Valid, /**pn_heart_rate=*/heartRate, /**pch_hr_valid=*/heartRateValid);
}

static int max30102_sample_fetch(const struct device *dev,
				 enum sensor_channel chan)
{
  //LOG_INF("enter heart rate calculation");
  //getNewData();
	heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);

  LOG_INF("SPO2 = %d, SPO2Valid= %d", SPO2, SPO2Valid);
  LOG_INF("heartRate = %d, heartRateValid= %d", heartRate, heartRateValid);

	return 0;
}

static int max30102_channel_get(const struct device *dev,
				enum sensor_channel chan,
				struct sensor_value *val)
{


	switch (chan) {
	case SENSOR_CHAN_RED:
		val->val1 = _senseBuf->red[_senseBuf->head];
		val->val2 = 0;
		break;

	case SENSOR_CHAN_IR:
		val->val1 = _senseBuf->IR[_senseBuf->head];
		val->val2 = 0;
		break;
	default:
		LOG_ERR("Unsupported sensor channel");
		return -ENOTSUP;
	}

	return 0;
}

static const struct sensor_driver_api max30102_driver_api = {
	.sample_fetch = max30102_sample_fetch,
	.channel_get = max30102_channel_get,
};





static int max30102_init(const struct device *dev)
{

	const struct max30102_config *config = dev->config;
	struct max30102_data *data = dev->data;


	_senseBuf = &(data->senseBuf);
	/* Get the I2C device */
	data->i2c = device_get_binding(config->i2c_label);
	if (!data->i2c) {
		LOG_ERR("Could not find I2C device");
		return -EINVAL;
	}

	_i2c_device = data->i2c;
	_i2c_addr = config->i2c_addr;

	if (getPartID() == MAX30102_EXPECTED_PARTID)
	{
		LOG_INF("part id ok");
	}
	else
	{
		LOG_ERR("Could not get Part ID");
	}

	softReset();

	LOG_INF("Sample Average=%d", config->sampleAverage);
	LOG_INF("ADC Range=%d", config->adcRange);
	LOG_INF("Sample Rate=%d", config->sampleRate);
	LOG_INF("Pulse Width=%d", config->pulseWidth);
	LOG_INF("LED Brightness=%d", config->ledBrightness);

	setFIFOAverage(config->sampleAverage);

	setADCRange(config->adcRange);

	setSampleRate(config->sampleRate);

	setPulseWidth(config->pulseWidth);

	setPulseAmplitudeRed(config->ledBrightness);
	setPulseAmplitudeIR(config->ledBrightness);

	enableSlot(1, SLOT_RED_LED);
	if (config->ledMode > MODE_REDONLY) enableSlot(2, SLOT_IR_LED);


	setLEDMode(config->ledMode);

	if (config->ledMode == MODE_REDONLY) {
		_activeLEDs = 1;
	} else {
		_activeLEDs = 2;
	}

	enableFIFORollover(); 
	resetFIFO(); 

	// /* Check the part id to make sure this is MAX30101 */
	// if (i2c_reg_read_byte(data->i2c, config->i2c_addr,
	// 		      MAX30102_REG_PART_ID, &part_id)) {
	// 	LOG_ERR("Could not get Part ID");
	// 	return -EIO;
	// }

	return 0;
}


// //Configuration Options 
// //FIFO Configuration(Register address 0x08)
// //sampleAverage(Table 3. Sample Averaging)
// #define SAMPLEAVG_1     0
// #define SAMPLEAVG_2     1
// #define SAMPLEAVG_4     2
// #define SAMPLEAVG_8     3
// #define SAMPLEAVG_16    4
// #define SAMPLEAVG_32    5

// //Mode configuration(Register address 0x09)
// //ledMode(Table 4. Mode Control)
// #define MODE_REDONLY    2
// #define MODE_RED_IR     3
// #define MODE_MULTILED   7

// //Particle sensing configuration(Register address 0x0A)
// //adcRange(Table 5. SpO2 ADC Range Control)
// #define ADCRANGE_2048   0
// #define ADCRANGE_4096   1
// #define ADCRANGE_8192   2
// #define ADCRANGE_16384  3
// //sampleRate(Table 6. SpO2 Sample Rate Control)
// #define SAMPLERATE_50   0 
// #define SAMPLERATE_100  1
// #define SAMPLERATE_200  2
// #define SAMPLERATE_400  3
// #define SAMPLERATE_800  4
// #define SAMPLERATE_1000 5
// #define SAMPLERATE_1600 6
// #define SAMPLERATE_3200 7
// //pulseWidth(Table 7. LED Pulse Width Control)
// #define PULSEWIDTH_69   0 
// #define PULSEWIDTH_118  1
// #define PULSEWIDTH_215  2
// #define PULSEWIDTH_411  3


static struct max30102_config max30102_config = {
	.i2c_label = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
	.ledBrightness = 50,
	.sampleAverage = SAMPLEAVG_4,
	.ledMode = MODE_MULTILED,
	.sampleRate = SAMPLERATE_400,
	.pulseWidth = PULSEWIDTH_411,
	.adcRange = ADCRANGE_16384
};

static struct max30102_data max30102_data;

DEVICE_DT_INST_DEFINE(0, max30102_init, device_pm_control_nop,
		    &max30102_data, &max30102_config,
		    APPLICATION, CONFIG_SENSOR_INIT_PRIORITY,
		    &max30102_driver_api);
