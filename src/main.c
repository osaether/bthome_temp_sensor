/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/uuid.h>

#include "temp_sensor.h"

#define BTHOME_SERVICE_UUID     0xfcd2  /* BTHome service UUID */
#define SERVICE_DATA_LEN        7
#define IDX_TEMPL               4       /* Index of lo byte of temp in service data */
#define IDX_TEMPH               5       /* Index of hi byte of temp in service data */

#define ADV_PARAM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_USE_IDENTITY, \
				  BT_GAP_ADV_SLOW_INT_MIN, \
				  BT_GAP_ADV_SLOW_INT_MAX, NULL)

static uint8_t service_data[SERVICE_DATA_LEN] = {
	BT_UUID_16_ENCODE(BTHOME_SERVICE_UUID),
	0x40,
	0x02,	/* Temperature */
	0x00,	/* Low byte */
	0x00,   /* High byte */
};

static struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR),
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME, sizeof(CONFIG_BT_DEVICE_NAME) - 1),
	BT_DATA(BT_DATA_SVC_DATA16, service_data, ARRAY_SIZE(service_data))
};

static void bt_ready(int err)
{
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
		return;
	}

	printk("Bluetooth initialized\n");

	/* Start advertising */
	err = bt_le_adv_start(ADV_PARAM, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		printk("Advertising failed to start (err %d)\n", err);
		return;
	}
}

static int init(void)
{
	int err;

	/* Initialize temperature sensor and start sampling thread */
	temp_init();
	
	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err != 0) {
		printk("Bluetooth init failed (err %d)\n", err);
		return err;
	}
	return 0;
}

int main(void)
{
	int err;
	static int32_t temperature;

	printk("Starting BTHome temp sensor\n");

	err = init();
	if (err != 0) {
		return 0;
	}

	for (;;) {
		temperature = temp_get();
		service_data[IDX_TEMPL] = (uint8_t)((uint32_t)temperature & 0xff);
		service_data[IDX_TEMPH] = (uint8_t)(((uint32_t)temperature >> 8) & 0xff);

		err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);
		if (err) {
			printk("Failed to update advertising data (err %d)\n", err);
			return 0;
		}
		k_sleep(K_MSEC(BT_GAP_ADV_SLOW_INT_MIN));
	}
	return 0;
}
