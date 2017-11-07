/****************************************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : i2c_light.h
* Description : Header file for i2c Light
*               -light_write_read_control_register()
*               -light_configure_integration_time()
*               -light_read_identification_register()
*               -light_enable_disable_interrupt_control_register()
*               -light_read_sensor_data_value()
*****************************************************************************************************************/



#ifndef I2C_LIGHT_H
#define I2C_LIGHT_H

/*INCLUDES*/
#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


/*MACROS*/
#define DEV_ADDR_LIGHT (0x39)
#define I2C_NUM (2)
#define BUFFER_SIZE (2)
#define COMMAND_CONTROL_REG (0x80)
#define POWER_UP (0x03)
#define POWER_DOWN (0x00)
#define COMMAND_TIMING_REG (0x81)
#define INTEGRATE_TIME (0x01)
#define GAIN (0x0)
#define TIMING_REG ((GAIN<<4)|INTEGRATE_TIME)
#define IDENTIFICATION_REG (0x8A)
#define PERSISTENCE (0x4)
#define INTR (0x01)
#define COMMAND_TH_LOW_LOWER (0x82)
#define COMMAND_TH_LOW_HIGHER (0x83)
#define COMMAND_TH_HIGH_LOWER (0x84)
#define COMMAND_TH_HIGH_HIGHER (0x85)
#define COMMAND_INT_REG (0x86)
#define LOW_TH_LOWER (0x0f)
#define LOW_TH_HIGHER (0x00)
#define HIGH_TH_LOWER (0x00)
#define HIGH_TH_HIGHER (0x08)
#define INTR_VALUE ((INTR<<4)|PERSISTENCE)
#define COMMAND_ADC0_LOW (0x8C)
#define COMMAND_ADC0_HIGH (0x8D)



/*****************************************************************************************************************
* light_write_read_control_register() - Used to write/read control register of light Sensor
*           
* int32_t file - File to read/write register 
* uint8_t op - Selects Write or Read
* uint8_t write_value - If write, value to write
* uint8_t (return) - returns the read value
******************************************************************************************************************/
uint8_t light_write_read_control_register(int32_t file,uint8_t write_value,uint8_t op);



/*****************************************************************************************************************
* light_configure_integration_time() - Used to configure integration time of light Sensor
*           
* int32_t file - File to read/write register 
* uint8_t write_value - value to write
* uint8_t (return) - returns the read value
******************************************************************************************************************/
uint8_t light_configure_integration_time(int32_t file, uint8_t write_value);


/*****************************************************************************************************************
* light_read_identification_register() - Used to read identification register of light Sensor
*           
* int32_t file - File to read/write register 
* uint8_t (return) - returns the read value
******************************************************************************************************************/
uint8_t light_read_identification_register(int32_t file);


/*****************************************************************************************************************
* light_enable_disable_interrupt_control_register() - Used to enable/disable interrupt control register of light Sensor
*           
* int32_t file - File to read/write register 
* uint8_t write_value - value to write
* uint8_t (return) - returns the read value
******************************************************************************************************************/
uint8_t light_enable_disable_interrupt_control_register(int32_t file,uint8_t write_value);


/*****************************************************************************************************************
* light_read_sensor_data_value() - Used to read sensor data value of light Sensor
*           
* int32_t file - File to read/write register 
* uint16_t (return) - returns the read value
******************************************************************************************************************/
uint16_t light_read_sensor_data_value(int32_t file);

#endif
