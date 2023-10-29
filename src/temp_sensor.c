/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include "temp_sensor.h"

#define THREAD_STACKSIZE        1024
#define THREAD_PRIORITY         7

K_THREAD_STACK_DEFINE(sensor_thread_stack, THREAD_STACKSIZE);
static struct k_thread sensor_thread;
K_MUTEX_DEFINE(sensor_mutex);

static int32_t temperature;

static void thread_entry(void *p1, void *p2, void *p3)
{
	int ret;
	struct sensor_value temp_value;

	const struct device *dev = (const struct device *)p1;
	struct k_mutex *temp_mutex = (struct k_mutex *)p2;
	int32_t *temp = (int32_t *)p3;

	while (1) {
		ret = sensor_sample_fetch(dev);
		if (ret != 0) {
			printk("sensor_sample_fetch failed ret %d\n", ret);
			return;
		}

		ret = sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp_value);
		if (ret != 0) {
			printk("sensor_channel_get failed ret %d\n", ret);
			return;
		}
		if (k_mutex_lock(temp_mutex, K_MSEC(2000)) == 0) {
			*temp = temp_value.val1 * 1E6 + temp_value.val2;
			*temp = (*temp + 5000)/10000.0;
			k_mutex_unlock(temp_mutex);
		} else {
			printk("Unable to lock mutex\n");
		}
		k_sleep(K_MSEC(2000));
	}
}

void temp_init(void)
{
	const struct device *dev = DEVICE_DT_GET_ANY(CONFIG_SENSOR_DEVICE_NAME);

	__ASSERT(dev != NULL, "Failed to get device binding");
	__ASSERT(device_is_ready(dev), "Device %s is not ready", dev->name);
	printk("device is %p, name is %s\n", dev, dev->name);

	k_thread_create(&sensor_thread, sensor_thread_stack,
			K_THREAD_STACK_SIZEOF(sensor_thread_stack),
			thread_entry, (void *)dev, (void *)&sensor_mutex, (void*)&temperature,
			THREAD_PRIORITY, 0, K_FOREVER);
	k_thread_start(&sensor_thread);
}

int32_t temp_get(void)
{
	int32_t temp = 0;

	if (k_mutex_lock(&sensor_mutex, K_MSEC(100)) == 0) {
		temp = temperature;
		k_mutex_unlock(&sensor_mutex);
	}

	return temp;
}
