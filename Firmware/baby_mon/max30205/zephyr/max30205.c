/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT max_max30205

#include <logging/log.h>

#include "max30205.h"


#define BUFFER_LENGTH 200
#define N_DECIMAL_POINTS_PRECISION 1000000

static uint16_t _i2c_addr;

static struct device *_i2c_device;

static int _activeLEDs = 0;

static float* _temperature;

LOG_MODULE_REGISTER(MAX30205, CONFIG_SENSOR_LOG_LEVEL);

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



static int max30205_sample_fetch(const struct device *dev,
				 enum sensor_channel chan)
{
	uint8_t readRaw[2] = {0};
    readReg(MAX30205_TEMPERATURE, readRaw ,2); // read two bytes
	int16_t raw = readRaw[0] << 8 | readRaw[1];  //combine two bytes
    *_temperature = raw  * 0.00390625;     // convert to temperature

	return 0;
}

static int max30205_channel_get(const struct device *dev,
				enum sensor_channel chan,
				struct sensor_value *val)
{


	switch (chan) {
	case SENSOR_CHAN_AMBIENT_TEMP:
		val->val1 = (int)(*_temperature);
		val->val2 = ((int)((*_temperature)*N_DECIMAL_POINTS_PRECISION)%N_DECIMAL_POINTS_PRECISION);
		break;
	default:
		LOG_ERR("Unsupported sensor channel");
		return -ENOTSUP;
	}

	return 0;
}

static const struct sensor_driver_api max30205_driver_api = {
	.sample_fetch = max30205_sample_fetch,
	.channel_get = max30205_channel_get,
};





static int max30205_init(const struct device *dev)
{

	const struct max30205_config *config = dev->config;
	struct max30205_data *data = dev->data;


	_temperature = &(data->temperature);
	/* Get the I2C device */
	data->i2c = device_get_binding(config->i2c_label);
	if (!data->i2c) {
		LOG_ERR("Could not find I2C device");
		return -EINVAL;
	}

	_i2c_device = data->i2c;
	_i2c_addr = config->i2c_addr;

	uint8_t byteTemp = 0x00;

	writeReg(MAX30205_CONFIGURATION, &byteTemp, 1);

	writeReg(MAX30205_THYST, &byteTemp, 1);
	
	writeReg(MAX30205_TOS, &byteTemp, 1);


	return 0;
}


static struct max30205_config max30205_config = {
	.i2c_label = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0)
};

static struct max30205_data max30205_data;

DEVICE_DT_INST_DEFINE(0, max30205_init, device_pm_control_nop,
		    &max30205_data, &max30205_config,
		    APPLICATION, CONFIG_SENSOR_INIT_PRIORITY,
		    &max30205_driver_api);
