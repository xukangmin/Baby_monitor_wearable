/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT max_max30205

#include <logging/log.h>

#include "max30205.h"


#define BUFFER_LENGTH 200

static uint16_t _i2c_addr;

const struct device *_i2c_device;

static int _activeLEDs = 0;

static sSenseBuf_t* _senseBuf;

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
  //LOG_INF("enter heart rate calculation");
  //getNewData();

	return 0;
}

static int max30205_channel_get(const struct device *dev,
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

static const struct sensor_driver_api max30205_driver_api = {
	.sample_fetch = max30205_sample_fetch,
	.channel_get = max30205_channel_get,
};





static int max30205_init(const struct device *dev)
{

	const struct max30205_config *config = dev->config;
	struct max30205_data *data = dev->data;


	_senseBuf = &(data->senseBuf);
	/* Get the I2C device */
	data->i2c = device_get_binding(config->i2c_label);
	if (!data->i2c) {
		LOG_ERR("Could not find I2C device");
		return -EINVAL;
	}

	_i2c_device = data->i2c;
	_i2c_addr = config->i2c_addr;


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
