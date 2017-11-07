/****************************************************************************************************************
* Authors : Vishal Vishnani
* Date : 10/06/2017
* 
* File : i2c_temp.h
* Description : Header file for i2c Temperature
*               -temp_write_pointer_register()
*               -temp_configure_conv_rate()
*               -temp_configure_shutdown_mode()
*               -temp_read_write_configuration_register()
*               -read_temperature()
*****************************************************************************************************************/



#ifndef I2C_TEMP_H
#define I2C_TEMP_H

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
#define DEV_ADDR_TEMP (0x48)
#define I2C_NUM (2)
#define BUFFER_SIZE (2)
#define TEMP_REG (0x00)
#define CONFIG_REG (0x01)


#define EXTENDED_MODE ((0x1)<<12)
#define SHUTDOWN_MODE (0x1)
#define THERMOSTAT_MODE ((0x1)<<1)
#define POLARITY_MODE ((0x1)<<2)
#define ONE_SHOT_MODE ((0x1)<<7)
#define CONV_RATE_1BY4 ((0x00)<<14)
#define CONV_RATE_ONE ((0x01)<<14)
#define CONV_RATE_FOUR ((0x2)<<14)
#define CONV_RATE_EIGHT ((0x03)<<14)



/*****************************************************************************************************************
* temp_write_pointer_reg() - Used to write pointer register of Temperature Sensor
*           
* int32_t file - File to write register 
* uint8_t register_type - Register/Address to be written
******************************************************************************************************************/
void temp_write_pointer_reg(int32_t file,uint8_t register_type);


/******************************************************************************************************************
* temp_write_pointer_reg() - Used to configure conversion rate of Temperature Sensor
*           
* int32_t file - File to write register 
* uint8_t command_reg - Register/Address to be written
* uint16_t(return) - value read after writing is returned
*******************************************************************************************************************/
uint16_t temp_configure_conv_rate(int32_t file, uint8_t command_reg, uint16_t write_value,uint8_t set_value);


/*******************************************************************************************************************
* temp_configure_shutdown_mode() - Used to enable/disable shutdown of Temperature Sensor
*           
* int32_t file - File to write register 
* uint8_t command_reg - Register/Address to be written
* uint16_t mode_value - Used to decide which mode (SHUTDOWN)
* uint8_t set_value - Used to decide to ENABLE/DISABLE
* uint16_t(return) - value read after writing is returned
*******************************************************************************************************************/
uint16_t temp_configure_shutdown_mode(int32_t file,uint8_t command_reg, uint16_t mode_value,uint8_t set_value);


/*******************************************************************************************************************
* temp_read_write_configuration_register() - Used to modify configuration of Temperature Sensor
*           
* int32_t file - File to write register 
* uint8_t op - Select between Write or Read
* uint16_t wrte_value - If write, value to write
* uint8_t set_value - Used to decide to ENABLE/DISABLE
* uint16_t(return) - value read after writing is returned
********************************************************************************************************************/
uint16_t temp_read_write_configuration_register(int32_t file,uint8_t op, uint16_t write_value,uint8_t set_value);



/*******************************************************************************************************************
* read_temperature() - Used to read ADC values of Temperature Sensor
*           
* int32_t file - File to read register 
* uint8_t command_reg - Register/Address to read
* int16_t(return) - value read is returned
********************************************************************************************************************/
int16_t read_temperature(int32_t file, uint8_t command_reg);

#endif
