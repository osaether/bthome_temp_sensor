/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include "sensor.h"

#if CONFIG_SENSOR_DS18B20
#define SENSOR_DEVICE_NAME maxim_ds18b20
#elif CONFIG_SENSOR_BME680
#define SENSOR_DEVICE_NAME bosch_bme680
#else
#define SENSOR_DEVICE_NAME ti_tmp112
#endif
 

#define THREAD_STACKSIZE        1024
#define THREAD_PRIORITY         7

K_THREAD_STACK_DEFINE(sensor_thread_stack, THREAD_STACKSIZE);
static struct k_thread sensor_thread;
K_MUTEX_DEFINE(sensor_mutex);

typedef struct
{
	int32_t temperature;
	uint32_t humidity;
} sensor_data_t;

static sensor_data_t sensor_data;

static void thread_entry(void *p1, void *p2, void *p3)
{
	int ret;
	struct sensor_value sens_value;

	const struct device *dev = (const struct device *)p1;
	struct k_mutex *sens_mutex = (struct k_mutex *)p2;
	sensor_data_t *temp = (sensor_data_t *)p3;

	while (1) {
		ret = sensor_sample_fetch(dev);
		if (ret != 0) {
			printk("sensor_sample_fetch failed ret %d\n", ret);
			return;
		}

		ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &sens_value);
		if (ret != 0) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		if (k_mutex_lock(sens_mutex, K_MSEC(2000)) == 0) {
			temp->temperature = sens_value.val1 * 1E6 + sens_value.val2;
			temp->temperature = (temp->temperature + 5000)/10000;
			k_mutex_unlock(sens_mutex);
		} else {
			printk("Unable to lock mutex\n");
		}
#if CONFIG_SENSOR_BME680
		ret = sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &sens_value);
		if (ret != 0) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		if (k_mutex_lock(sens_mutex, K_MSEC(2000)) == 0) {
			temp->humidity = sens_value.val1 * 1E6 + sens_value.val2;
			temp->humidity = (temp->humidity + 5000)/10000;
			k_mutex_unlock(sens_mutex);
		} else {
			printk("Unable to lock mutex\n");
		}
#endif
		k_sleep(K_MSEC(2000));
	}
}

void sensor_init(void)
{
	const struct device *dev = DEVICE_DT_GET_ANY(SENSOR_DEVICE_NAME);

	__ASSERT(dev != NULL, "Failed to get device binding");
	__ASSERT(device_is_ready(dev), "Device %s is not ready", dev->name);
	printk("device is %p, name is %s\n", dev, dev->name);

	k_thread_create(&sensor_thread, sensor_thread_stack,
			K_THREAD_STACK_SIZEOF(sensor_thread_stack),
			thread_entry, (void *)dev, (void *)&sensor_mutex, (void*)&sensor_data,
			THREAD_PRIORITY, 0, K_FOREVER);
	k_thread_start(&sensor_thread);
}

int32_t temperature_get(void)
{
	int32_t temp = 0;

	if (k_mutex_lock(&sensor_mutex, K_MSEC(100)) == 0) {
		temp = sensor_data.temperature;
		k_mutex_unlock(&sensor_mutex);
	}

	return temp;
}

uint32_t humidity_get(void)
{
	uint32_t temp = 0;

	if (k_mutex_lock(&sensor_mutex, K_MSEC(100)) == 0) {
		temp = sensor_data.humidity;
		k_mutex_unlock(&sensor_mutex);
	}

	return temp;
}

uint32_t pressure_get(void)
{
	/* Pressure is not supported by the current sensor implementations */
	return 0;
}
