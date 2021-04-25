/** @file
 *  @brief custom service
 */

/*
 * Copyright (c) 2016 Intel Corporation
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

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

static uint8_t sphr[4];
static uint8_t sphr_update;

static void sphr_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	/* TODO: Handle value */
}

static ssize_t read_sphr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
		       void *buf, uint16_t len, uint16_t offset)
{
	const char *value = attr->user_data;

	return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
				 sizeof(sphr));
}

static ssize_t write_sphr(struct bt_conn *conn, const struct bt_gatt_attr *attr,
			const void *buf, uint16_t len, uint16_t offset,
			uint8_t flags)
{
	uint8_t *value = attr->user_data;

	if (offset + len > sizeof(sphr)) {
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	memcpy(value + offset, buf, len);
	sphr_update = 1U;

	return len;
}

/* Custom Service Variables */
static struct bt_uuid_128 vnd_uuid = BT_UUID_INIT_128(
	0xf0, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
	0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

static struct bt_uuid_128 vnd_cus_uuid = BT_UUID_INIT_128(
	0xf1, 0xde, 0xbc, 0x9a, 0x78, 0x56, 0x34, 0x12,
	0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

BT_GATT_SERVICE_DEFINE(vnd_svc,
	BT_GATT_PRIMARY_SERVICE(&vnd_uuid),
	BT_GATT_CHARACTERISTIC(&vnd_cus_uuid.uuid,
			       BT_GATT_CHRC_READ |
			       BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE,
			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
			       read_sphr, write_sphr, sphr),
	BT_GATT_CCC(sphr_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);


// /* Current Time Service Declaration */
// BT_GATT_SERVICE_DEFINE(sphr_svc,
// 	BT_GATT_PRIMARY_SERVICE(BT_UUID_CTS),
// 	BT_GATT_CHARACTERISTIC(BT_UUID_CTS_CURRENT_TIME, BT_GATT_CHRC_READ |
// 			       BT_GATT_CHRC_NOTIFY | BT_GATT_CHRC_WRITE,
// 			       BT_GATT_PERM_READ | BT_GATT_PERM_WRITE,
// 			       read_sphr, write_sphr, sphr),
// 	BT_GATT_CCC(sphr_ccc_cfg_changed, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
// );

void sphr_init(void)
{
}

void sphr_notify(uint8_t* sphr_data, size_t data_size)
{	/* Current Time Service updates only when time is changed */

	bt_gatt_notify(NULL, &vnd_svc.attrs[1], sphr_data, data_size);
}
