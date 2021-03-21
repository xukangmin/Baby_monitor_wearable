/*
 * Copyright (c) 2017, NXP
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT max_max30102

#include <logging/log.h>

#include "max30102.h"

LOG_MODULE_REGISTER(MAX30102, CONFIG_SENSOR_LOG_LEVEL);

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

	/* Check the part id to make sure this is MAX30101 */
	if (i2c_reg_read_byte(data->i2c, config->i2c_addr,
			      MAX30102_REG_PART_ID, &part_id)) {
		LOG_ERR("Could not get Part ID");
		return -EIO;
	}

	if (part_id == MAX30102_PART_ID) 
	{
		LOG_INF("Part Number OK");
	}
	else
	{
		LOG_ERR("Got Part ID 0x%02x, expected 0x%02x",
			    part_id, MAX30102_PART_ID);
		return -EIO;
	}

	return 0;
}

static struct max30102_config max30102_config = {
	.i2c_label = DT_INST_BUS_LABEL(0),
	.i2c_addr = DT_INST_REG_ADDR(0),
	.fifo = 10,
#if defined(CONFIG_MAX30102_HEART_RATE_MODE)
	.mode = MAX30102_MODE_HEART_RATE,
	.slot[0] = MAX30102_SLOT_RED_LED1_PA,
	.slot[1] = MAX30102_SLOT_DISABLED,
	.slot[2] = MAX30102_SLOT_DISABLED,
	.slot[3] = MAX30102_SLOT_DISABLED,
#elif defined(CONFIG_MAX30102_SPO2_MODE)
	.mode = MAX30102_MODE_SPO2,
	.slot[0] = MAX30102_SLOT_RED_LED1_PA,
	.slot[1] = MAX30102_SLOT_IR_LED2_PA,
	.slot[2] = MAX30102_SLOT_DISABLED,
	.slot[3] = MAX30102_SLOT_DISABLED,
#else
	.mode = MAX30102_MODE_MULTI_LED,
	.slot[0] = 0,
	.slot[1] = 0,
	.slot[2] = 0,
	.slot[3] = 0,
#endif

	.spo2 = (1 << MAX30102_SPO2_ADC_RGE_SHIFT) |
		(1 << MAX30102_SPO2_SR_SHIFT) |
		(MAX30102_PW_18BITS << MAX30102_SPO2_PW_SHIFT),

	.led_pa[0] = 0,
	.led_pa[1] = 0,
	.led_pa[2] = 0,
};

static struct max30102_data max30102_data;

DEVICE_DT_INST_DEFINE(0, max30102_init, device_pm_control_nop,
		    &max30102_data, &max30102_config,
		    POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY,
		    &max30102_driver_api);
