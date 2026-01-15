/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#include "vl53l1_platform.h"
#include <time.h>
#include <math.h>

#include "app_state.h"
#include "stm32f4xx_hal_i2c.h"

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	uint8_t status = 255;
	
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
	
	return status;
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
	uint8_t status = 255;
	
	/* To be filled by customer. Return 0 if OK */
	/* Warning : For big endian platforms, fields 'RegisterAdress' and 'value' need to be swapped. */
	
	return status;
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	uint8_t status = 255;
	
	if (
		HAL_I2C_Mem_Write(
			app_state.pI2CHandle,
			dev,
			index,
			I2C_MEMADD_SIZE_16BIT,
			&data,
			1,
			CUSTOM_SENSOR_CONN_TIMEOUT
			) == HAL_OK
	) {
		return 0;
	}

	return status;
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
	uint8_t status = 255;
	
	uint8_t buffer[2];

	buffer[0] = (data >> 8) & 0xFF;
	buffer[1] = data & 0xFF;

	if (
		HAL_I2C_Mem_Write(
			app_state.pI2CHandle,
			dev,
			index,
			I2C_MEMADD_SIZE_16BIT,
			buffer,
			2,
			CUSTOM_SENSOR_CONN_TIMEOUT
			) == HAL_OK
	) {
		return 0;
	}
	
	return status;
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
	uint8_t status = 255;
	
	uint8_t buffer[4];

	buffer[0] = (data >> 24) & 0xFF;
	buffer[1] = (data >> 16) & 0xFF;
	buffer[2] = (data >> 8) & 0xFF;
	buffer[3] = data & 0xFF;

	if (
		HAL_I2C_Mem_Write(
			app_state.pI2CHandle,
			dev,
			index,
			I2C_MEMADD_SIZE_16BIT,
			buffer,
			4,
			CUSTOM_SENSOR_CONN_TIMEOUT
			) == HAL_OK
	) {
		return 0;
	}
	
	return status;
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
	uint8_t status = 255;
	
	if (
		HAL_I2C_Mem_Read(
			app_state.pI2CHandle,
			dev,
			index,
			I2C_MEMADD_SIZE_16BIT,
			data,
			1,
			CUSTOM_SENSOR_CONN_TIMEOUT
			) == HAL_OK
	) {
		return 0;
	}
	
	return status;
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
	uint8_t status = 255;

	uint8_t buffer[2];

	if (
		HAL_I2C_Mem_Read(
			app_state.pI2CHandle,
			dev,
			index,
			I2C_MEMADD_SIZE_16BIT,
			buffer,
			2,
			CUSTOM_SENSOR_CONN_TIMEOUT
			) == HAL_OK
	) {
		return 0;
	}

	*data = (buffer[0] << 8) | buffer[1];

	return status;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
	uint8_t status = 255;
	
	uint8_t buffer[4];

	if (
		HAL_I2C_Mem_Read(
			app_state.pI2CHandle,
			dev,
			index,
			I2C_MEMADD_SIZE_16BIT,
			buffer,
			4,
			CUSTOM_SENSOR_CONN_TIMEOUT
			) == HAL_OK
	) {
		return 0;
	}

	*data = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
	
	return status;
}

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms){
	(void)dev;

	HAL_Delay(wait_ms);
	
	return 0;
}
