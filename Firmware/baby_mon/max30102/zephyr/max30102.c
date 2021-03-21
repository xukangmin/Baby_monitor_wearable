/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT max_max30102

#include <logging/log.h>

#include "max30102.h"


static uint16_t _i2c_addr;

const struct device *_i2c_device;

LOG_MODULE_REGISTER(MAX30102, CONFIG_SENSOR_LOG_LEVEL);

void writeReg(uint8_t reg, const void* pBuf, uint8_t size)
{
  if(pBuf == NULL) {
    LOG_ERR("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;

   i2c_burst_write(_i2c_device, _i2c_addr, reg, _pBuf, size);
}

uint8_t readReg(uint8_t reg, const void* pBuf, uint8_t size)
{
  if(pBuf == NULL) {
    LOG_ERR("pBuf ERROR!! : null pointer");
  }
  uint8_t * _pBuf = (uint8_t *)pBuf;


  i2c_burst_read(_i2c_device, _i2c_addr, reg, _pBuf, size);

  return size;
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



float readTemperatureC()
{

  uint8_t byteTemp = 0x01;
  writeReg(MAX30102_DIETEMPCONFIG, &byteTemp, 1);

  uint32_t startTime = millis();
  while (millis() - startTime < 100) { 
    readReg(MAX30102_DIETEMPCONFIG, &byteTemp, 1);
    if ((byteTemp & 0x01) == 0) break; 
    delay(1);
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




static int max30102_sample_fetch(const struct device *dev,
				 enum sensor_channel chan)
{
	return 0;
}

static int max30102_channel_get(const struct device *dev,
				enum sensor_channel chan,
				struct sensor_value *val)
{
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
	uint8_t part_id;
	uint8_t mode_cfg;
	uint32_t led_chan;
	int fifo_chan;

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

	// /* Check the part id to make sure this is MAX30101 */
	// if (i2c_reg_read_byte(data->i2c, config->i2c_addr,
	// 		      MAX30102_REG_PART_ID, &part_id)) {
	// 	LOG_ERR("Could not get Part ID");
	// 	return -EIO;
	// }

	return 0;
}

static struct max30102_config max30102_config = {
	.i2c_label = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
	.ledBrightness = 0x1F,
	.sampleAverage = SAMPLEAVG_4,
	.ledMode = MODE_MULTILED,
	.sampleRate = SAMPLERATE_400,
	.pulseWidth = PULSEWIDTH_411,
	.adcRange = ADCRANGE_4096
};

static struct max30102_data max30102_data;

DEVICE_DT_INST_DEFINE(0, max30102_init, device_pm_control_nop,
		    &max30102_data, &max30102_config,
		    POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &max30102_driver_api);
