/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <drivers/sensor.h>
#include <stdio.h>
#include "max32664.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>
#include <bluetooth/services/bas.h>
#include <bluetooth/services/hrs.h>

#include <drivers/led_strip.h>
#include <device.h>
#include <drivers/spi.h>
#include <sys/util.h>

#include "sphr.h"

/* size of stack area used by each thread */
#define STACKSIZE 1024

/* scheduling priority used by each thread */
#define PRIORITY 7

/* delay between greetings (in ms) */
#define SLEEPTIME 500
#define STRIP_NUM_LEDS 1

#define DELAY_TIME K_MSEC(40)

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define COLOR_BLACK 3
#define COLOR_WHITE 4
#define COLOR_ORANGE 4

uint8_t sphr_data[4];

static const struct led_rgb colors[] = {
	{ .r = 0xff, .g = 0x00, .b = 0x00, }, /* red */
	{ .r = 0x00, .g = 0xff, .b = 0x00, }, /* green */
	{ .r = 0x00, .g = 0x00, .b = 0xff, }, /* blue */
	{ .r = 0x00, .g = 0x00, .b = 0x00, }, /* black */
	{ .r = 0xff, .g = 0xff, .b = 0xff, }, /* white */
	{ .r = 165, .g = 165, .b = 0, }, /* orange */
};

static const struct led_rgb black = {
	.r = 0x00,
	.g = 0x00,
	.b = 0x00,
};

struct led_rgb strip_color;

const struct led_rgb *color_at(size_t time, size_t i)
{
	size_t rgb_start = time % STRIP_NUM_LEDS;

	if (rgb_start <= i && i < rgb_start + ARRAY_SIZE(colors)) {
		return &colors[i - rgb_start];
	} else {
		return &black;
	}
}

K_FIFO_DEFINE(my_fifo);



struct data_item_t {
    void *fifo_reserved;   /* 1st word reserved for use by FIFO */
    int hr;
	int spo2;
	int confidence;
	int status;
};

struct data_item_t tx_data;


static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		      BT_UUID_16_ENCODE(BT_UUID_HRS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_BAS_VAL),
		      BT_UUID_16_ENCODE(BT_UUID_DIS_VAL))
};

static void connected(struct bt_conn *conn, uint8_t err)
{
	if (err) {
		printk("Connection failed (err 0x%02x)\n", err);
	} else {
		printk("Connected\n");
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	printk("Disconnected (reason 0x%02x)\n", reason);
}

static struct bt_conn_cb conn_callbacks = {
	.connected = connected,
	.disconnected = disconnected,
};

static void bt_ready(void)
{
	int err;

	printk("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}

	printk("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	printk("Pairing cancelled: %s\n", addr);
}

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = auth_cancel,
};

static void bas_notify(void)
{
	uint8_t battery_level = bt_bas_get_battery_level();

	battery_level--;

	if (!battery_level) {
		battery_level = 100U;
	}

	bt_bas_set_battery_level(battery_level);
}

static void hrs_notify(void)
{
	static uint8_t heartrate = 90U;

	/* Heartrate measurements simulation */
	heartrate++;
	if (heartrate == 160U) {
		heartrate = 90U;
	}
	//printk("mesaured heart rate=%d\n", heartrate);
	bt_hrs_notify(heartrate);
}

K_THREAD_STACK_DEFINE(threadA_stack_area, STACKSIZE);
static struct k_thread threadA_data;


void threadA(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	int err;
	struct sensor_value hr;
	struct sensor_value spo2;
	struct sensor_value status;
	struct sensor_value confidence;
	struct sensor_value temp;
	//struct sensor_value red;
	//struct sensor_value IR;
	const struct device *dev = device_get_binding(DT_LABEL(DT_INST(0, max_max32664)));

	if (dev == NULL) {
		printf("Could not get max32664 device\n");
		return;
	}

	const struct device *devTemp = device_get_binding(DT_LABEL(DT_INST(0, max_max30205)));

	if (devTemp == NULL) {
		printf("Could not get max30205 device\n");
		return;
	}

	err = bt_enable(NULL);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}





	bt_ready();

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);

	/* Implement notification. At the moment there is no suitable way
	 * of starting delayed work so we do it here
	 */

	while (1) {
		k_sleep(K_SECONDS(1));



		sensor_sample_fetch(dev);
		sensor_channel_get(dev, SENSOR_CHAN_HEARTRATE, &hr);
		sensor_channel_get(dev, SENSOR_CHAN_SPO2, &spo2);
		sensor_channel_get(dev, SENSOR_CHAN_HR_STATUS, &status);
		sensor_channel_get(dev, SENSOR_CHAN_CONFIDENCE, &confidence);

		sensor_sample_fetch(devTemp);
		sensor_channel_get(devTemp, SENSOR_CHAN_AMBIENT_TEMP, &temp);
		//sensor_channel_get(dev, SENSOR_CHAN_IR, &IR);
		/* Print green LED data*/
		//printf("red=%d\n", red.val1);
		//printf("IR=%d\n", IR.val1);
		printk("hr=%d\n",hr.val1);
		printk("spo2=%d\n",spo2.val1);
		printk("status=%d\n",status.val1);
		printk("confidence=%d\n",confidence.val1);

		printk("temp1=%d\n",temp.val1);
		printk("temp2=%d\n",temp.val2);

		printk("temp=%f\n",sensor_value_to_double(&temp));
		/* Heartrate measurements simulation */
		hrs_notify();
		tx_data.hr = hr.val1;
		tx_data.spo2 = spo2.val1;
		tx_data.status = status.val1;
		tx_data.confidence = confidence.val1;

		k_fifo_put(&my_fifo, &tx_data);
		/* Battery level simulation */
		bas_notify();

		sphr_data[0] = (uint8_t)hr.val1;
		sphr_data[1] = (uint8_t)spo2.val1;
		sphr_data[2] = (uint8_t)status.val1;
		sphr_data[3] = (uint8_t)confidence.val1;

		sphr_notify(sphr_data,4);
	}

}

K_THREAD_STACK_DEFINE(threadB_stack_area, STACKSIZE);
static struct k_thread threadB_data;


void threadB(void *dummy1, void *dummy2, void *dummy3)
{
	ARG_UNUSED(dummy1);
	ARG_UNUSED(dummy2);
	ARG_UNUSED(dummy3);

	struct data_item_t  *rx_data;

	struct data_item_t  *cur_data;

	int minimum_sleep = 100;
	
	int sleep_time = 1000;

	double sleep_timed = 1000;

	const struct device *strip;
	size_t i, time;

	strip = device_get_binding(DT_LABEL(DT_INST(0, apa_apa102)));
	if (strip) {
		printk("Found LED strip device %s", DT_LABEL(DT_INST(0, apa_apa102)));
	} else {
		printk("LED strip device %s not found", DT_LABEL(DT_INST(0, apa_apa102)));
		return;
	}
	
	int cnt = 0;
	while(1) {

		cur_data = k_fifo_get(&my_fifo, K_NO_WAIT);

		if (cur_data)
		{
			rx_data = cur_data;

			if (rx_data->status == 3)
			{
				// get valid hr data, update sleep time
   			    sleep_timed = 1000.0f * 60.0f / (float)rx_data->hr;
				
				sleep_time = (int)sleep_timed;

				if (sleep_time < 200) sleep_time = 200;

				if (sleep_time > 2000) sleep_time = 2000;

				printk("sleep time=%d\n", sleep_time);
			}

		}

		if (rx_data)
		{
			if (rx_data->status == 0)
			{
				strip_color = colors[COLOR_RED];
				led_strip_update_rgb(strip, &strip_color, STRIP_NUM_LEDS);
				k_sleep(K_MSEC(50));
				strip_color = colors[COLOR_BLACK];
				led_strip_update_rgb(strip, &strip_color, STRIP_NUM_LEDS);
				k_sleep(K_MSEC(2000));
			}
			else if (rx_data->status == 1 || rx_data->status == 2)
			{
				strip_color = colors[COLOR_ORANGE];
				led_strip_update_rgb(strip, &strip_color, STRIP_NUM_LEDS);
				k_sleep(K_MSEC(50));
				strip_color = colors[COLOR_BLACK];
				led_strip_update_rgb(strip, &strip_color, STRIP_NUM_LEDS);
				k_sleep(K_MSEC(1000));
			}
			else
			{
				strip_color = colors[COLOR_GREEN];
				led_strip_update_rgb(strip, &strip_color, STRIP_NUM_LEDS);
				k_sleep(K_MSEC(50));
				strip_color = colors[COLOR_BLACK];
				led_strip_update_rgb(strip, &strip_color, STRIP_NUM_LEDS);
				k_sleep(K_MSEC(sleep_time));
			}
			
		}

		k_sleep(K_MSEC(1));
		
	}

}

void main(void)
{


	k_thread_create(&threadA_data, threadA_stack_area,
			K_THREAD_STACK_SIZEOF(threadA_stack_area),
			threadA, NULL, NULL, NULL,
			PRIORITY, 0, K_FOREVER);
	k_thread_name_set(&threadA_data, "thread_a");

	k_thread_start(&threadA_data);


	k_thread_create(&threadB_data, threadB_stack_area,
			K_THREAD_STACK_SIZEOF(threadB_stack_area),
			threadB, NULL, NULL, NULL,
			PRIORITY, 0, K_FOREVER);
	k_thread_name_set(&threadB_data, "thread_b");

	k_thread_start(&threadB_data);
}
